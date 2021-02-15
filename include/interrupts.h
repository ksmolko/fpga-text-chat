#pragma once

#include "xscugic.h"
#include "xtmrctr.h"


void interrupts_init();
void interrupts_loop();
void Timer_InterruptHandler(XTmrCtr *data, u8 TmrCtrNumber);
int SetUpInterruptSystem(XScuGic *XScuGicInstancePtr);
int ScuGicInterrupt_Init(u16 DeviceId,XTmrCtr *TimerInstancePtr);
