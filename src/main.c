#include <stdio.h>
#include <stdbool.h>
#include "xil_types.h"
#include "xtmrctr.h"
#include "xparameters.h"

#include "xil_io.h"
#include "xil_exception.h"
#include "xil_cache_l.h"
#include "xscugic.h"


volatile bool TIMER_INTR_FLG;
XScuGic InterruptController; /* Instance of the Interrupt Controller */
static XScuGic_Config *GicConfig;/* The configuration parameters of thecontroller */
//static XGpioPs my_Gpio;
int NUM_BYTES_BUFFER = 5242880;

void Timer_InterruptHandler(XTmrCtr *data, u8 TmrCtrNumber)
{
	XTmrCtr_Stop(data,TmrCtrNumber);
	XTmrCtr_Reset(data,TmrCtrNumber);
	//Update Stuff
	TIMER_INTR_FLG = true;
}

int SetUpInterruptSystem(XScuGic *XScuGicInstancePtr){
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
	(Xil_ExceptionHandler) XScuGic_InterruptHandler,
	XScuGicInstancePtr);
	Xil_ExceptionEnable();
	return XST_SUCCESS;
}

/*Setup all interrupts of the system*/
int ScuGicInterrupt_Init(u16 DeviceId,XTmrCtr *TimerInstancePtr)
{
	int Status;
	GicConfig = XScuGic_LookupConfig(DeviceId);
	if (NULL == GicConfig) {
		return XST_FAILURE;
	}
	Status = XScuGic_CfgInitialize(&InterruptController, GicConfig,
	GicConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	Status = SetUpInterruptSystem(&InterruptController);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	/*Connect a device driver handler that will be called when an interrupt for the device occurs, the device driver handler performs the specific interrupt processing for the device*/
	Status = XScuGic_Connect(&InterruptController,
	61,
	(Xil_ExceptionHandler)XTmrCtr_InterruptHandler,
	(void *)TimerInstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

void setColours(int colour, int* image_pointer[5]){

	int red = 0xFF0000FF;
	int green = 0xFF00FF00;
	int blue = 0xFFFF0000;
	int yellow = 0x0000FFFF;
	int purple = 0x00FF00FF;

	if(colour == 1){
		red = 0xFFECFF00;
		green = 0xFFF000FF;
		blue = 0xFFFFFFFF;
		yellow = 0xFF0087FF;
		purple = 0xFF97FF00;
	}
	/*else if (colour == 2){
		red = 0xFF000024;
		green = 0xFF002400;
		blue = 0xFF240000;
		yellow = 0x000024FF;
		purple = 0x002400FF;
	}*/

	int i = 0;

	for(; i < NUM_BYTES_BUFFER / 20; i++){
		memcpy(image_pointer[0] + i, &red, 4);
		memcpy(image_pointer[1] + i, &yellow, 4);
		memcpy(image_pointer[2] + i, &purple, 4);
		memcpy(image_pointer[3] + i, &blue, 4);
		memcpy(image_pointer[4] + i, &green, 4);
	}

	for(; i < NUM_BYTES_BUFFER * 2 / 20; i++){
		memcpy(image_pointer[0] + i, &green, 4);
		memcpy(image_pointer[1] + i, &red, 4);
		memcpy(image_pointer[2] + i, &yellow, 4);
		memcpy(image_pointer[3] + i, &purple, 4);
		memcpy(image_pointer[4] + i, &blue, 4);
	}

	for(; i < NUM_BYTES_BUFFER * 3 / 20; i++){
		memcpy(image_pointer[0] + i, &blue, 4);
		memcpy(image_pointer[1] + i, &green, 4);
		memcpy(image_pointer[2] + i, &red, 4);
		memcpy(image_pointer[3] + i, &yellow, 4);
		memcpy(image_pointer[4] + i, &purple, 4);
	}

	for(; i < NUM_BYTES_BUFFER * 4 / 20; i++){
		memcpy(image_pointer[0] + i, &purple, 4);
		memcpy(image_pointer[1] + i, &blue, 4);
		memcpy(image_pointer[2] + i, &green, 4);
		memcpy(image_pointer[3] + i, &red, 4);
		memcpy(image_pointer[4] + i, &yellow, 4);
	}

	for(; i < NUM_BYTES_BUFFER * 5 / 20; i++){
		memcpy(image_pointer[0] + i, &yellow, 4);
		memcpy(image_pointer[1] + i, &purple, 4);
		memcpy(image_pointer[2] + i, &blue, 4);
		memcpy(image_pointer[3] + i, &green, 4);
		memcpy(image_pointer[4] + i, &red, 4);
	}
}

int main()
{
	// Disable Caches (prevents tearing in images)
	Xil_L1DCacheDisable();
	Xil_L2CacheDisable();

	XTmrCtr TimerInstancePtr;
	int xStatus;
	//-----------Setup Timer Interrupt---------------------------------------

	xStatus = XTmrCtr_Initialize(&TimerInstancePtr,XPAR_AXI_TIMER_0_DEVICE_ID);

	XTmrCtr_SetHandler(&TimerInstancePtr,
	(XTmrCtr_Handler)Timer_InterruptHandler,
	&TimerInstancePtr);

	//Reset Values
	XTmrCtr_SetResetValue(&TimerInstancePtr,
	0, //Change with generic value
	//0xFFF0BDC0);
	//0x23C34600);
	0xDC3CB9FF);
	//Interrupt Mode and Auto reload
	XTmrCtr_SetOptions(&TimerInstancePtr,
	XPAR_AXI_TIMER_0_DEVICE_ID,
	(XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION ));

	xStatus=ScuGicInterrupt_Init(XPAR_PS7_SCUGIC_0_DEVICE_ID,&TimerInstancePtr);

	/*Enable the interrupt for the device and then cause (simulate) an interrupt so the handlers will be called*/
	XScuGic_Enable(&InterruptController, 61);
	XScuGic_SetPriorityTriggerType(&InterruptController, 61, 0xa0, 3);
	int img = 0;
	char buf = 0;
	int * image_buffer_pointer = (int *)0x00900000;
	int * image_pointer[5] = {(int *)0x018D2008, (int *)0x020BB00C, (int *)0x028A4010, (int *)0x0308D014, (int *)0x03876018};

	memset(image_pointer[0], 0, NUM_BYTES_BUFFER);
	memset(image_pointer[1], 0, NUM_BYTES_BUFFER);
	memset(image_pointer[2], 0, NUM_BYTES_BUFFER);
	memset(image_pointer[3], 0, NUM_BYTES_BUFFER);
	memset(image_pointer[4], 0, NUM_BYTES_BUFFER);

	setColours(0, image_pointer);

	while(1) {
		XTmrCtr_Start(&TimerInstancePtr,0);
		while(TIMER_INTR_FLG == false){
		}

		TIMER_INTR_FLG = false;

		if (buf == 0) {
			memcpy(image_buffer_pointer, image_pointer[0], NUM_BYTES_BUFFER);
		}

		buf = inbyte();

		if(buf == '1'){
			setColours(0, image_pointer);
			memcpy(image_buffer_pointer, image_pointer[img], NUM_BYTES_BUFFER);

		}
		else if(buf == '2'){
			setColours(1, image_pointer);
			memcpy(image_buffer_pointer, image_pointer[img], NUM_BYTES_BUFFER);

		}
		else if(buf == 'w'){ // Shift up
			if(img == 0){
				memcpy(image_buffer_pointer, image_pointer[4], NUM_BYTES_BUFFER);
				img = 4;
			}
			else {
				memcpy(image_buffer_pointer, image_pointer[img-1], NUM_BYTES_BUFFER);
				img--;
			}
		}
		else if(buf == 's'){ // Shift down
			if(img == 4){
				memcpy(image_buffer_pointer, image_pointer[0], NUM_BYTES_BUFFER);
				img = 0;
			}
			else {
				memcpy(image_buffer_pointer, image_pointer[img+1], NUM_BYTES_BUFFER);
				img++;
			}
		}
	}

	return 0;
}

