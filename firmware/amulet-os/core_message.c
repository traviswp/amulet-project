/* *
 *
 * Copyright 2016 by the Trustees of Dartmouth College and Clemson University, and
 * distributed under the terms of the "Dartmouth College Non-Exclusive Research Use
 * Source Code License Agreement" (for NON-COMMERCIAL research purposes only), as
 * detailed in a file named LICENSE.pdf within this repository.
 */

/*
 * core_message.c
 *
 *  Created on: Oct 11, 2013
 *  Andres,tl
 */

#include "core_message.h"
//#define BSP_DEBUG_IPC
#define IPC_PREAMBLE_SIZE 0 //In the old version, it is 4. All ipc message start
                            //with preamble like "@ @ @ @
extern uint8_t circularstart; // The size of the circular IPC outbound buffer is 
                              // defined at bsp_setup.c
extern uint8_t circularend;
                  
// TODO Confirm with Ron that this IPC format is still valid.
// Process the received IPC message and redirect to corresponding Managers
void ProcessIPC(uint8_t *buffer, uint16_t length){
  // We will add other if-else statements here to process other kind of IPC
  // events.
  // TODO Maybe it makes more sense to convert it to a State Machine, but does
  // state machine improve speed?
  ipcMessage message;
  BytesToBuffer(&message, buffer);
	if(IPC_EVENT == message.majorType)  {
    // If its target is sensor manager, here we use the same manager to
    // manage both external and internal sensors.
    // TODO Combine these two targetID.
		if(EXT_SENSOR_MAN_M4 == message.targetID || INT_SENSOR_MAN_M4 == message.targetID) {
      // If they are sensor data.
			if(EXTERNAL_SENSOR_DATA  == message.minorType || INTERNAL_SENSOR_DATA  == message.minorType){
        NotifyApp(&buffer[7+IPC_PREAMBLE_SIZE], length-7-IPC_PREAMBLE_SIZE);////nparams 8 special symbols and 8 bytes with other info
			}
		}
	}
}

void BytesToBuffer(ipcMessage *message, uint8_t *buffer) {
  message->majorType = buffer[2 + IPC_PREAMBLE_SIZE];
  message->minorType = buffer[3 + IPC_PREAMBLE_SIZE];
  uint16_t byte4 = buffer[4 + IPC_PREAMBLE_SIZE];
  uint16_t byte5 = buffer[5 + IPC_PREAMBLE_SIZE];
  message->targetID = buffer[6 + IPC_PREAMBLE_SIZE];
  message->messageID = byte5 << 8;
  message->messageID = message->messageID | byte4;
}

void MessageToBytes(ipcMessage *message, uint8_t *bytes) {
	// Each message start with { '@', '@', '@', '@' };
	uint16_t i = 0;
	uint16_t length = 8 + message->nparams;
	bytes[0] = '@';
	bytes[1] = '@';
	bytes[2] = '@';
	bytes[3] = '@';
	bytes[4] = (uint8_t) length;
	bytes[5] = (uint8_t) (length >> 8);
	bytes[6] = message->majorType;
	bytes[7] = message->minorType;
	bytes[8] = (uint8_t) message->messageID;
	bytes[9] = (uint8_t) (message->messageID >> 8);
	bytes[10] = message->targetID;
	for(i = 0; i<message->nparams; i++){
		bytes[i+11] = message->configParams[i];
	}
	bytes[10+message->nparams + 1] = '%';
  // Each message end with { '&', '&', '&', '&' };
  bytes[10+message->nparams + 2] = '&';
	bytes[10+message->nparams + 3] = '&';
	bytes[10+message->nparams + 4] = '&';
	bytes[10+message->nparams + 5] = '&';
}

void enqueueIPC(uint8_t sensor_id, uint16_t expiration, 
  uint16_t window_size, uint16_t window_interval, uint8_t requestor,
  uint8_t majorType, uint8_t minorType, uint8_t targetID, uint8_t nparams,
	uint16_t messageID){  
    
  
  //If sensor is an extern sensor then we should handle it the id differently  
  circularIPCMsg[circularend].configParams[0] = sensor_id;
  if(sensor_id==HEARTRATE)
    circularIPCMsg[circularend].configParams[0] = 1;
    
  circularIPCMsg[circularend].configParams[1] = (uint8_t) expiration; //exp
  circularIPCMsg[circularend].configParams[2] = (uint8_t) (expiration) >> 8; //exp
  circularIPCMsg[circularend].configParams[3] = (uint8_t) window_size; 
  circularIPCMsg[circularend].configParams[4] = (uint8_t) (window_size) >> 8; 
  circularIPCMsg[circularend].configParams[5] = (uint8_t) window_interval; 
  circularIPCMsg[circularend].configParams[6] = (uint8_t) (window_interval) >> 8;
  circularIPCMsg[circularend].configParams[7] = requestor;  

  circularIPCMsg[circularend].messageID = messageID; //TODO:this needs to generated by the network manager?
  circularIPCMsg[circularend].majorType = majorType;
  circularIPCMsg[circularend].minorType = minorType;
  circularIPCMsg[circularend].targetID = targetID;
  circularIPCMsg[circularend].nparams = nparams;
  
  __bic_SR_register(GIE);               //Disable Interrupts
  circularend = ((circularend + 1) % IPC_CIRCULAR_SIZE);
  __bis_SR_register(GIE);              
  
}

void ipcSend(ipcMessage * message){
	uint8_t messageBytes[MAX_IPC_MESSAGE_SIZE];
	MessageToBytes(message, messageBytes);
}
