/*****************************************************************************
* Model: blinky.qm
* File:  ..//blinky.h
*
* This code has been generated by QM tool (see state-machine.com/qm).
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*****************************************************************************/
/* @(/2/1) .................................................................*/
#ifndef blinky_h
#define blinky_h

#include "qp_port.h"
#include "amulet_api.h"

enum BlinkySignals {
    SWITCH_SIG = Q_USER_SIG,
};

/* @(/1/2) .................................................................*/
void Blinky_ctor(void);

extern QActive * const AO_Blinky;


#endif                    /* blinky_h */
