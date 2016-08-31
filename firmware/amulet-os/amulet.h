/* *
 *
 * Copyright 2016 by the Trustees of Dartmouth College and Clemson University, and
 * distributed under the terms of the "Dartmouth College Non-Exclusive Research Use
 * Source Code License Agreement" (for NON-COMMERCIAL research purposes only), as
 * detailed in a file named LICENSE.pdf within this repository.
 */

/**
 *  @file amulet.h
 *  @brief Main header file + select IDs for sensors, actuators, etc.
 *
 *  This contains the prototypes for the Amulet API and eventually any
 *  macros, constants, or global variables that app developers will need.
 *
 *  @author Travis Peters
 *  @author andresmf
 */
#ifndef AMULET_H
#define	AMULET_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>


// This just adds in a few methods and some data, we need a way to add arrays of data?
//#define SIFT_DATA_HACK


/**
 * Definition for how many interrupts are generated per second on the MSP430.
 *
 * @note Assuming sourced from ACLK on LXFT crystal
 */
#define AMULET_TICKS_PER_SEC    20U //32768U

/**
 * QP Includes.
 */
#include "qp_port.h"

/**
 * BSP Includes.
 */
#include "bsp_setup.h"

/**
 * Amulet (Event Layer) Includes.
 *
 * All other core modules are included below so that files in the upper layer(s)
 * only have to include core.h to access the core.
 */
#include "amulet_datatypes.h"

#include "core_ui.h"
#include "core_app_manager.h"
#include "core_message.h"
#include "core_mode.h"
#include "core_sensors.h"
#include "core_storage.h"
#include "core_algorithm.h"
#include "core_graph.h"
#include "analytics.h"

/**
 * Every app should be defined with a "type" -- TYPE_UNDEFINED by default.
 * The amulet system is especially interested in identifying the "emergency"
 * app(s).
 *
 * @note There should ideally be some mechanism to prevent just any app from
 * identifying as an "emergency" type application since these applications have
 * special privileges when compared to other types of applications.
 */
typedef enum types {
    TYPE_UNDEFINED     = 0x00,   /**< The app's "type" doesn't fit into any other type category (default) */
    TYPE_DEBUG_APP     = 0x01,   /**< Debug/test realated app */
    TYPE_HEALTH_APP    = 0x02,   /**< Health realated app */
    TYPE_FITNESS_APP   = 0x04,   /**< Fitness related app */
    TYPE_SAFETY_APP    = 0x08,   /**< Safety related app */
    TYPE_EMA_APP       = 0x10,   /**< An Ecological Momentary Assessment (EMA) app */
    TYPE_SYSTEM_APP    = 0x20,   /**< An app meant to convey system information or improve system functionality */
    TYPE_EMERGENCY_APP = 0x40    /**< A special app meant to handle emergency situations */
} AppType;

/**
 * This ClockType is used to get value of data and time (see: AmuletClock()).
 */
typedef enum clocktypes {
    YEAR     = 0x00,
    MONTH    = 0x01,
    DAY      = 0x02,
    HOUR     = 0x04,
    MINUTE   = 0x08,
    SECOND   = 0x10,
    WEEKDAY  = 0x12
} ClockType;

/**
 * Used for UI to set Fonts
 */
typedef enum fonttypes {
    BOLD_FONT = 0x00,
    MEDIUM_FONT = 0x01,
    REGULAR_FONT = 0x02
} FontType;

/**
 * Used for UI to get the font sizes
 */
typedef enum fonttsizes {
    BOLD_FONT_SIZE = 22,
    MEDIUM_FONT_SIZE = 16,
    REGULAR_FONT_SIZE = 12
} FontSize;


/* ************************************************************************** *
 *                             Amulet System Signals                          *
 * ************************************************************************** */

/**
 * The value for the first Amulet OS internal signal.
 *
 * @note It is important to note that the Amulet system runs atop the QP
 * framework, an event driven framework composed of concurrent finite state
 * machines; To transition between different states in the state machines,
 * signals are generated and sent to applications. The application-level signals
 * do not start from 0, however, but are offset by the constant Q_USER_SIG (this
 * is because QP reserves the lowest few signals for internal use and provides
 * this constant as an offset from which user-level signals can start). The
 * internal Amulet system signals will start at Q_USER_SIG; Amulet applications
 * can start defining signals at
 */
#define AMULET_INTERNAL_Q_SIG_START_SIG Q_USER_SIG

/**
 * CoreSignals are signals generated by the core layer that are available to
 * the amulet system, but not applications.
 */
enum CoreSignal {
    CORE_STIMER_SIG = AMULET_INTERNAL_Q_SIG_START_SIG,
    CORE_CTIMER_SIG,
    CORE_NOTIFY_OTHER_SIG,
    CORE_LOG_APPEND_SIG,
    CORE_LOG_READ_SIG,
    __END_OF_CORE_SIGNALS
};

/**
 * AmuletSignals are signals generated by the event layer that are available
 * to all applications.
 *
 * @todo Add signals for:
 * - data from M0 acc sensor
 * - data from IMU acc sensor
 * - data from IMU gyro sensor
 * - data from IMU mag sensor
 * - data from heart rate sensor
 * - ...
 */
enum AmuletSignal {
    AMULET_DEVICE_DATA_XL_SIG = __END_OF_CORE_SIGNALS,
    AMULET_DEVICE_DATA_HR_SIG,
    AMULET_DEVICE_DATA_SIG,
    AMULET_REQ_CHANGE_SIG, // (timer signal)
    AMULET_BUTTON_TAP_SIG,
    AMULET_BUTTON_HOLD_SIG,
    AMULET_BOTH_HOLD_SIG,
    AMULET_APPEND_SUCCESS_SIG,
    AMULET_APPEND_FAIL_SIG,
    AMULET_READ_SUCCESS_SIG,
    AMULET_READ_FAIL_SIG,
    AMULET_SLIDER_UP_SIG,
    AMULET_SLIDER_DOWN_SIG,
    __END_OF_AMULET_SIGNALS
};

/**
 * The value for the first Amulet OS application signal.
 */
#define AMULET_APPLICATIONS_Q_SIG_START_SIG __END_OF_AMULET_SIGNALS

/* ************************************************************************** *
 *                                 Amulet Events                              *
 * ************************************************************************** */

/**
 * QM structure for representing a generic amulet event.
 */
typedef struct {
    QEvt super;
} AmuletEvt;

/**
 * QM structure for representing button related events.
 */
typedef struct {
    QEvt super;
    uint8_t buttonId;
} AmuletButtonEvt;

/**
 * QM structure for representing sensor related events.
 */
typedef struct {
    QEvt super;
    // uint8_t imu_a;
    // uint8_t imu_b;
    // uint8_t imu_c;
    // uint16_t expiration;
    // uint16_t window_size;
    uint8_t sensor_id;
    // uint16_t window_interval;
    // uint8_t caller_id;
    uint8_t * data;
    uint8_t sensor_type;
    uint16_t length;
} AmuletSensorsEvt;

/* ************************************************************************** *
 *                                Amulet Resources                            *
 * ************************************************************************** */

/**
 * All of the Amulet Resources (Sensors) that are available on the amulet (both
 * on the communication board as well as the app board).
 *
 * Some of these names provide the bridge of IPC. Usually, one byte of IPC
 * message will have this field indicating the type of the message.
 * IPC.c in the bsp layer will receive the IPC raw message and put it into a
 * buffer. Then processIPC function in core_message.c will be called to process the
 * buffer and redirect to Amulet Sensor Manager if it is a sensor type message.
 * Then the Amulet Sensor Manager will check field of the redirected message to
 * check if there is/are any app(s) subscribed to the sensor. If there are active
 * subscription, the sensor data will then be sent to the app.
 *
 * @todo The integers for this part are random. Work with Ron to fix it (As the M0
 * part will need to use the same number).
 * @todo WE NEED TO RESOLVE THE SENSOR SUBSCRIPTION INTERFACE.
 *  - how do we specify the current HW configuration so that apps know WHAT is available?
 *  - how do we determine which sensors apps need, so that if apps don't need a sensor, we
 *    don't have to include that code in the firmware? (e.g., if no app uses HR, we don't
 *    need HR sensor related services)
 */
enum InternalResource {
    LIGHT,                  // Light sensor located on the MSP430
    IMU,                    // IMU located on the MSP430 (Includes ACCELEROMETER, GYROSCOPE, and MAGNETOMETER)
    ACCELEROMETER = 4,    // Low-power accelerometer located on the M0
    AUDIO,                  // Microphone located on the MSP430
    TEMPERATURE,             // Temperature sensor located on the MSP430
    TIMER = 21,             // Timer service provided by amulet-os/QM
    BATTERY                  // Battery from adc
};

enum AnalyticsResource {
    BLE_EVENT = 1,
    UI_EVENT = 2
};

enum ExternalResource {
    HEARTRATE = 132,          // External heart rate sensor (Mio or Zephyr HR devices)
    RUNNING_SPEED = 133 // External running speed, cadence, stride, and distance (BLE devices)
};

// RESOURCES THAT DON'T NEED TO BE SUBSCRIBED TO:
// - DISPLAY -- each app can use the display, but only the "front" app has control of the display; the only exception is that each app can optionally specify a "status" in the status (top) line of the display.
// - BUTTONS -- each app will receive button press (tap) events. the button hold event, currently, is special and is only sent to the emergency response application
// - LEDs -- each app can use the LEDs. we currently don't have rules defined for who can use the LEDs...first come, first serve; preemption is possible.
// - HAPTICBUZZER -- each app can use the haptic buzzer; again, no rules defined.
// - THUMBWHEEL -- each app can use the thumb wheel; again, no rules defined.
// - SDCARD -- each app can use the SD card; each app currently has one log just for itself (the aft automatically creates a LOG ID for each app based on the apps present in the firmware).

/* ************************************************************************** *
 *                                   SENSORS
 * ************************************************************************** */

/**
 * Subscribe to a particular sensor and specify configuration parameters.
 *
 * @code
 * AmuletSubscribeInternalSensor(ACCELEROMETER, 0, 0, 0, 0, 0);
 * @endcode
 *
 * @note For all deprecated arguments, just pass 0. The interface for sensor
 * subscriptions is currently defined for more fine-grained control which is
 * not yet implemented. Only the desired sensor to subscribe to needs to be
 * specified.
 *
 * @param sensor_id The sensor to subscribe to (defined in amulet.h).
 * @param expiration (depreciated) The Sensor Manager will stop sending data and events after this expiration (in seconds).
 * @param window_size (depreciated) The sampling window size (in seconds).
 * @param window_interval (depreciated) The sampling interval (in seconds); must be greater than or equal to the window_size.
 *
 * @return None.
 */
 void AmuletSubscribeInternalSensor(uint8_t sensor_id, uint16_t expiration,
   uint16_t window_size, uint16_t window_interval, uint8_t requestor);

/**
 * Unsubscribe from a particular sensor
 *
 * @code
 * AmuletUnsubscribeInternalSensor(ACCELEROMETER);
 * @endcode
 *
 * @param sensor_id The sensor to subscribe to (defined in amulet.h).
 */
void AmuletUnsubscribeInternalSensor(uint8_t sensor_id, uint8_t requestor);

/**
 * Get the current heart rate (HR). Once subscribed to the HR sensor
 * (see: AmuletSubscribeInternalSensor), the  DEVICE_DATA_HR signal will be sent
 * every time there are new HR samples available.
 *
 * @note Available sensors are defined in amulet.h.
 * @note The Sensor Signals are defined in the SensorSignals enum (see: core_api.h).
 *
 *
 * @return The current heart rate as a uint8_t.
 */
uint16_t AmuletGetHR(uint8_t requestor);

/**
 * Get the battery reading from the battery adc
 *
 * @return battery percent returned as a uint8_t.
 */
uint8_t AmuletGetBatteryLevel(uint8_t requestor);

/**
 * Get the light reading from the light sensor
 *
 * @return Acceleration data returned as a uint16_t.
 */
uint16_t AmuletGetLightLevel(uint8_t requestor);

/**
 * Get the temperature from the temp sensor
 *
 * @return temperature data returned as a uint16_t.
 */
uint16_t AmuletGetTemperature(uint8_t requestor);

/**
 * Get the audio level from the microphone
 *
 * @return audio data returned as a uint16_t.
 */
uint16_t AmuletGetAudio(uint8_t requestor);

/**
 * Get the adc value from the passed sensor
 *
 * @return adc data returned as a uint16_t.
 */
uint16_t AmuletGetADC(enum InternalResource ir, uint8_t requestor);

/**
 * Get analytics results from the underlying platform about user interactinos and BLE
 *
 * @return the analytics value since this was last called for the internal resource
 */
uint16_t AmuletAnalytics(enum AnalyticsResource ir, uint8_t requestor);

/**
 * Status of BLE pairing
 * @return true if BLE is paired, false if not
 */
bool AmuletIsBLEPaired();


/**
 * TIANLONG? Is this even necessary anymore?
 * Get acceleration along the x axis.
 *
 * @return Acceleration data returned as a uint16_t.
 */
int16_t AmuletGetAccelX(uint8_t idx, uint8_t requestor);

/**
 * TIANLONG? Is this even necessary anymore?
 * Get acceleration along the y axis.
 *
 * @return Acceleration data returned as a uint16_t.
 */
int16_t AmuletGetAccelY(uint8_t idx, uint8_t requestor);

/**
 * TIANLONG? Is this even necessary anymore?
 * Get acceleration along the z axis.
 *
 * @return Acceleration data returned as a uint16_t.
 */
int16_t AmuletGetAccelZ(uint8_t idx, uint8_t requestor);

/* ************************************************************************** *
 *                                ACTUATORS
 * ************************************************************************** */

/**
 * Actuate a device. (deprecated)
 *
 * @param actuatorId The actuator to actuate.
 * @param actuationParams Any configuration parameters to supplement the actuation.
 * @param nparams The number of parameters being passed in actuationParams.
 *
 * @return None.
 */
// void AmuletActuate(uint8_t actuatorId, __uint8_t_array actuationParams, uint8_t nparams, uint8_t requestor);

/* ************************************************************************** *
 *                                   CLOCK
 * ************************************************************************** */

/**
 * Get time/date information.
 *
 * @todo The clock for AmuletClock() needs to be set to correct time!

 * @code
 * // Get today's date (day/month/year)
 * uint8_t day = AmuletClock(DAY);
 * uint8_t month = AmuletClock(MONTH);
 * uint8_t year = AmuletClock(YEAR);
 * @endcode
 *
 * @param type Available types are YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, WEEKDAY.
 *
 * @return The corresponding value according to the amulet system (NOTE: WEEKDAY and MONTH is zero-based).
 */
uint8_t AmuletClock(ClockType type, uint8_t requestor);

/**
 * Set time/date information.
 *

 * @code
 * // Set today's date (day/month/year)
 * AmuletSetClock(DAY, day);
 * AmuletSetClock(MONTH, month);
 * AmuletSetClock(YEAR, year);
 * @endcode
 *
 * @param type Available types are YEAR, MONTH, DAY, HOUR, MINUTE, SECOND,
 WEEKDAY.
 *
 * @return The corresponding value according to the amulet system (NOTE: WEEKDAY
 and MONTH is zero-based).
 */
void AmuletSetClock(ClockType type, uint16_t value, uint8_t requstor);

void AmuletDateTimeText(__char_array target, uint8_t requestor);
/**
 * Get a text representation for the clock (date, weekday, and time) in this moment.
 *
 * @todo The clock for AmuletClockText() need to be set to correct time

 * @code
 * uint8_t emode1[13];
 * uint8_t emode2[13];
 * uint8_t emode3[15];
 * AmuletClockText(emode1,emode2,emode3);
 * AmuletDisplayMessage(emode1, 4);
 * @endcode
 *
 * @param target1 An array where the "Date" text can be written.
 * @param target2 An array where the "Weekday" text can be written.
 * @param target3 An array where the "Time" text can be written.
 *
 * @return None.
 */
void AmuletClockText(__char_array target1, __char_array target2, __char_array target3, uint8_t requestor);

/**
 * Get a text representation for the current Date.
 *
 * @param target An array where the "Date" text can be written.
 *
 * @return None.
 */
void AmuletDateText(__char_array target, uint8_t requestor);

/**
 * Get the text representation for the current day of the week.
 *
 * @param target An array where the "Weekday" text can be written.
 *
 * @return None.
 */
void AmuletWeekText(__char_array target, uint8_t requestor);

/* ************************************************************************** *
 *                                   TIMER
 * ************************************************************************** */

/**
 * Set a timer. Once the timer is set, the AMULET_REQ_CHANGE signal will be
 * sent every @param{interval} seconds.
 *
 * @code
 * AmuletTimer(2); //Set timer to notify me every two seconds
 * @endcode
 *
 * @param interval The desired timer interval (in second), in uint16_t type
 *
 * @return None.
 */
void AmuletTimer(uint16_t interval, uint8_t requestor);

/**
 * Cancel a timer that was previously set.
 *
 * @return None.
 */
void AmuletTimerCancel(uint8_t requestor);

/* ************************************************************************** *
 *                                   UI
 * ************************************************************************** */

//
// DISPLAY
//
void AmuletBoldText(uint8_t x, uint8_t y, __char_array message, uint8_t requestor);
void AmuletMediumText(uint8_t x, uint8_t y, __char_array message, uint8_t requestor);
void AmuletRegularText(uint8_t x, uint8_t y, __char_array message, uint8_t requestor);

void AmuletBoldCenteredText(uint8_t y,  __char_array message, uint8_t requestor);
void AmuletMediumCenteredText(uint8_t y,  __char_array message, uint8_t requestor);
void AmuletRegularCenteredText(uint8_t y,  __char_array message, uint8_t requestor);

void AmuletDrawRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t requestor);

void AmuletClearRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t requestor);

void AmuletFillRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t requestor);

void AmuletDrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t requestor);

/**
 * Draw vertical line on the screen, you should refresh the screen after draw
 * 
 * @param x the X coordinate of the starting point
 * @param y the Y coordinate of the starting point
 * @param h length of the line
 */
void AmuletDrawFastVLine(int16_t x, int16_t y, int16_t h, uint8_t requestor);

/**
 * Draw horizontal line on the screen, you should refresh the screen after draw
 * 
 * @param x the X coordinate of the starting point
 * @param y the Y coordinate of the starting point
 * @param h length of the line
 */
void AmuletDrawFastHLine(int16_t x, int16_t y, int16_t w, uint8_t requestor);


uint8_t AmuletTextWidth(__char_array message, uint8_t requestor);

uint8_t AmuletTextHeight(uint8_t requestor);

void AmuletPushChangesToDisplay(uint8_t requestor);

void AmuletDrawHeartImage(uint8_t x,uint8_t y, uint8_t requestor);

void AmuletDrawWalkerImage(uint8_t x,uint8_t y, uint8_t requestor);

void AmuletDrawSunImage(uint8_t x,uint8_t y, uint8_t requestor);

uint8_t AmuletDrawingAreaWidth();

uint8_t AmuletDrawingAreaHeight();

// Graphing Functions (Steven Hearndon):
int8_t AmuletAddGraph(GraphType type, uint8_t x, uint8_t y, uint8_t w, uint8_t h,
                uint8_t data_min, uint8_t data_max, uint8_t requestor);

void AmuletAddGraphData(uint8_t data, uint8_t graph_id, uint8_t requestor);

void AmuletDrawGraph(uint8_t graph_id, uint8_t requestor);

/**
 * Write text to the application's display buffer.
 *
 * @note An application's display buffer is only rendered when it is the
 * "front" running application.
 *
 * @code
 * uint8_t msg[14] = "Clock test";
 * AmuletDisplayMessage(msg, 3);
 * @endcode
 *
 * @param message The desired text to display.
 * @param line The line number where the desired text will be written.
 *
 * @return None.
 */
void AmuletDisplayMessage(__char_array message, uint8_t line, uint8_t requestor);

/**
 * Clear the entire display buffer.
 *
 * @return None.
 */
void AmuletDisplayClr(uint8_t requestor);

/**
 * Clear a specific line of the display buffer.
 *
 * @param lineNumber The line on the display to clear.
 *
 * @return None.
 */
void AmuletDisplayClrLN(uint8_t lineNumber, uint8_t requestor);

/**
 * Set the "status" text for the app that will be displayed in the top (status)
 * line on the display.
 *
 * Every app gets MAX_STATUS_LEN characters in the top (status) line. If the
 * message passed to this routine exceeds MAX_STATUS_LEN, then the message will
 * be truncated. If the message is less than MAX_STATUS_LEN, then the message will
 * be padded with white space.
 *
 * @note Every call to this routine results in a call to refresh the display.
 * @note This function an be used by your app to notify the user of an alert if your
 * app is running in the background.
 *
 * @param message The text to display in the top (status) line of the display.
 *
 * @return None.
 */
//void AmuletSetAppStatus(__uint8_t_array message, uint8_t requestor);

/**
 * Clear the status text for the calling app.
 *
 * @return None.
 */
//void AmuletClearAppStatus(uint8_t requestor);

//
// BUTTONS
//

/**
 * Uses the current event (passed in by the caller) to determine the
 * ID that was pressed as part of a button event (TAP/HOLD).
 *
 * @todo Need to add check that the current event is in fact an AmuletButtonEvt.
 *
 * @param e  a reference to the current QEvt object.
 */
//uint8_t AmuletGetButtonPressedId(QEvt const * const e, uint8_t requestor);

/**
 * Check the physical location of a particular button on the Amulet.
 *
 * @todo Is the AmuletButtonSide() function still needed?! Is anyone using this?
 *
 * @param id The ID of the button in question.
 * @return an enum value corresponding to the side of the case where the button is located (see: ui.h).
 */
ButtonSide AmuletButtonSide(uint8_t id, uint8_t requestor);

//
// LEDs
//

/**
 * Turn a specific LED off.
 *
 * @param ledId the ID of the LED that is to be turned off (see: LED enum)
 *
 * @return None.
 */
void AmuletLEDOff(LED ledId, uint8_t requestor);

/**
 * Turn a specific LED on.
 *
 * @param ledId the ID of the LED that is to be turned on (see: LED enum)
 *
 * @return None.
 */
void AmuletLEDOn(LED ledId, uint8_t requestor);

/**
 * "Blink" a specific LED.
 *
 * @todo I think this still needs to be implemented properly with amulet system timers...
 * @todo how do we handle if one app starts to blink the LED but then goes into the background?
 * are LEDs just for forward-facing apps? Just for background apps?
 *
 * @param ledId the ID of the LED that is to be blinked (see: LED enum)
 *
 * @return None.
 */
void AmuletLEDBlink(LED ledId, uint8_t requestor);


/**
 * 
 * Turns on the capacitive touch timers
 * @param requestor [description]
 */
void AmuletTurnOnCapTouch(uint8_t requestor);

/**
 * Turns off the capacitive touch timers
 * @param requestor [description]
 */
void AmuletTurnOffCapTouch(uint8_t requestor);

//
// HAPTIC BUZZER
//

/**
 * Trigger a single, 0.1 second "buzz" from the haptic buzzer.
 *
 * @return None.
 */
void AmuletHapticSingleBuzz(uint8_t requestor);

/**
 * Trigger two back-to-back, 0.1 second "buzz"es from the haptic buzzer.
 *
 * @return None.
 */
void AmuletHapticDoubleBuzz(uint8_t requestor);

/* ************************************************************************** *
 *                                   SYSTEM
 * ************************************************************************** */

/**
 * Check if app is "front" (i.e., currently has control of the display).
 *
 * @return boolean A boolean value indicating if the requesting app is the "front" app.
 */
bool AmuletIsForegroundApp(uint8_t requestor);

/**
 * Set the current system mode to 'Normal' mode.
 *
 * @return None.
 */
void AmuletEnterNormalMode(uint8_t requestor);

/**
 * Determine if system is in 'Normal' mode.
 *
 * @return boolean indicating if system mode is set to 'Normal' mode.
 */
bool AmuletInNormalMode(uint8_t requestor);

/**
 * Set the current system mode to 'EmergencyMaybe' mode.
 *
 * @return None.
 */
void AmuletEnterEmergencyMaybeMode(uint8_t requestor);

/**
 * Determine if system is in 'EmergencyMaybe' mode.
 *
 * @return boolean indicating if system mode is set to 'EmergencyMaybe' mode.
 */
bool AmuletInEmergencyMaybeMode(uint8_t requestor);

/**
 * Set the current system mode to 'Emergency' mode.
 *
 * @return None.
 */
void AmuletEnterEmergencyMode(uint8_t requestor);

/**
 * Determine if the system is in 'Emergency' mode.
 *
 * @return boolean indicating if system mode is set to 'Emergency' mode.
 */
bool AmuletInEmergencyMode(uint8_t requestor);

/**
 * Set the current system mode to 'Debug' mode.
 *
 * @return None.
 */
void AmuletEnterDebugMode(uint8_t requestor);

/**
 * Determine if the system is in 'Debug' mode.
 *
 * @return boolean indicating if system mode is set to 'Debug' mode.
 */
bool AmuletInDebugMode(uint8_t requestor);

/**
 * Temporary Hack: Allow a pre-approved app to request to take control of the
 * UI. We don't currently have any rules defining how the OS should grant/deny
 * such requests so for this simple version, control of the UI is *always*
 * granted to the requestor if they have the appropriate permission to make this
 * request (i.e., the app's manifest file contains privilege the necessary
 * privilege to call this function).
 *
 * @return boolean indicating if the foreground app was updated and the display
 *  was refreshed with the new foreground app's display buffer.
 */
bool AmuletRequestMoveToFront(uint8_t requestor);

/* ************************************************************************** *
 *                                   STORAGE
 * ************************************************************************** */

/**
 * Check if the SD card exists.
 *
 * @return return A uint8_t value: 0 means the SD is present, otherwise it does not.
 */
uint8_t AmuletSDPresent(uint8_t requestor);

/**
 * Append a line to your app's log file on the SD card. An AMULET_APPEND_SUCCESS
 * signal will be generated if the log append is successful; likewise an
 * AMULET_APPEND_FAIL signal will be generated if the log append is unsuccessful.
 *
 * @note The AFT will auto-fill the logKey and requestor parameters---as part of
 * the AFT firmware generation, app IDs (i.e., the requestor ID) and log IDs
 * (i.e., logKey) are automatically created.
 *
 * @code
 * char name[18] = "Name: John Doe\n";
 * AmuletLogAppend(name);
 * @endcode
 *
 * @param log_name The log to write to -- this value will be automatically inserted
          by the AFT so application developers need not specify this value manually.
 * @param line_contents A character array containing the contents to write to the log.
 *
 * @return A uint8_t value representing the result of the read: 0 indicates a
 * successful read, while a 1 indicates an unsuccessful read.
 */
uint8_t AmuletLogAppend(uint8_t log_name, __char_array line_contents, uint8_t requestor);

/**
 * Read lines from your app's log file on the SD card.
 *
 * @note The AFT will auto-fill the logKey and requestor parameters---as part of
 * the AFT firmware generation, app IDs (i.e., the requestor ID) and log IDs
 * (i.e., logKey) are automatically created.

 * @code
 * char lineContents[20];
 * uint8_t response = AmuletLogRead(lineContents, readLine, 1);
 * @endcode
 *
 * @param log_name The log to write to -- this value will be automatically inserted
          by the AFT so application developers need not specify this value manually.
 * @param line_contents A character array where the read log contents can be written.
 * @param start_line The line from which the log read will start.
 * @param n_lines The number of line to read from the log to the buffer.
 *
 * @return A uint8_t value representing the result of the read: 0 indicates a
 * successful read, while a 1 indicates an unsuccessful read.
 */
uint8_t AmuletLogRead(uint8_t log_name, __char_array line_contents, uint8_t start_line, int n_lines, uint8_t requestor);

/* ************************************************************************** *
 *                                 QP WRAPPERS
 * ************************************************************************** */

/**
 * Send a signal to explicitly transition to another state defined in this
 * application's finite state machine (FSM).
 *
 * @note The AFT will auto-fill the 'me' and 'requestor' parameters so the
 * application developer need not know/supply those details.
 * @note This function prevents applications from having to write code that
 * is dependent on the underlying state machine implementation.
 *
 * @code
 * AmuletStateTransition(SomeSignalDefinedInMyApp);
 * @endcode
 *
 * @param sig The signal that is to be posted.
 *
 * @return None.
 */
void AmuletStateTransition(QActive * const me, int sig, uint8_t requestor);

/* ************************************************************************** *
 *                          UTILITY & DEBUG FUNCTIONS
 * ************************************************************************** */

/**
 * Converts an integer value to a null-terminated string using the specified base
 * and stores the result in the array given by the 'target' parameter.
 *
 * @note AmuletITOA is merely a wrapper of the system itoa.
 * @note CORE DEVELOPERS: please change this function and its implementation if
 * you want to convert longer interger---this is very easy.
 *
 * Be aware this function can only convert non-negative integer.
 *
 * @param source The original integer you want to convert.
 * @param target The result as a uint8_t array.
 *
 * @return None.
 */
void AmuletITOA(int16_t source, __char_array target, uint8_t requestor);

/**
 * Concat the second string to the end of the first string
 *
 * @param target The original string will be modified.
 * @param to_add The string will be appended to the original string.
 *
 * @return None.
 */
void AmuletConcat(__char_array target, __char_array to_add, uint8_t requestor);

/**
 * Composes a string with the same text that would be printed if format was used
 * on printf, but instead of being printed, the content is stored as a C string
 * in the 'target' buffer.
 *
 * @note AmuletSprintf is a custom verson of sprintf for Amulet.
 *
 * @param target A character array where the resulting string is stored.
 * @param fmt The desired C string style format string.
 *
 * @return None.
 */
void AmuletSprintf(__char_array target, __char_array fmt, ... );

/**
  Return the RR interval(1/1024s) as 16 bit unsigned integer
*/
uint16_t AmuletGetRR(uint8_t requestor);


float AmuletGetStdDev(__float_array rrs,  float total_time, uint8_t numBeats, uint8_t requestor);

//@Author: Hang
float AmuletStringToFloat(__char_array source, uint8_t requstor);
uint16_t AmuletATOI(__char_array source, uint8_t requstor);

//@Author: Daniel
uint16_t AmuletReadfromFile(uint8_t log_name, __char_array line_contents, uint16_t start_line, int n_lines, uint8_t requestor);
void AmuletFloatToString(float value, __char_array target, uint8_t requestor);

/**
 * Seconds since the amulet started up
 */
uint32_t AmuletSeconds(uint8_t requestor);


void AmuletGetECG(__float_array target, uint8_t requestor);
void AmuletGetABP(__float_array target, uint8_t requestor);
void AmuletGetRPKS(__uint8_t_array target, uint8_t requestor);
void AmuletGetSPKS(__uint8_t_array target, uint8_t requestor);

#endif	/* AMULET_H */
