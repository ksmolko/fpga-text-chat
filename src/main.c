#include <stdio.h>
#include <stdbool.h>
#include "xil_types.h"
#include "xtmrctr.h"
#include "xparameters.h"

#include "xil_io.h"
#include "xil_exception.h"
#include "xil_cache_l.h"
#include "xscugic.h"

#include "netif/xadapter.h"
#include "lwip/tcp.h"

#define IMG_BUF_PTR 0x00900000
#define NUM_BYTES_BUFFER 5242880


volatile bool TIMER_INTR_FLG;
XScuGic InterruptController; /* Instance of the Interrupt Controller */
static XScuGic_Config *GicConfig;/* The configuration parameters of thecontroller */
int img = 0;
char buf = 0;
int * image_pointer[5] = {(int *)0x018D2008, (int *)0x020BB00C, (int *)0x028A4010, (int *)0x0308D014, (int *)0x03876018};
XTmrCtr TimerInstancePtr;

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

void setup()
{
	// Disable Caches (prevents tearing in images)
	Xil_L1DCacheDisable();
	Xil_L2CacheDisable();

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

	memset(image_pointer[0], 0, NUM_BYTES_BUFFER);
	memset(image_pointer[1], 0, NUM_BYTES_BUFFER);
	memset(image_pointer[2], 0, NUM_BYTES_BUFFER);
	memset(image_pointer[3], 0, NUM_BYTES_BUFFER);
	memset(image_pointer[4], 0, NUM_BYTES_BUFFER);

	setColours(0, image_pointer);

	/************* Ethernet Setup *************/
	ip_addr_t *ipaddr, *netmask, *gw;
	struct netif *netif;
	unsigned char mac_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

	IP4_ADDR(ipaddr, 192, 168, 0, 20);
	IP4_ADDR(netmask, 255, 255, 255, 0);
	IP4_ADDR(gw, 192, 168, 0, 1);

	lwip_init();

	if (!xemac_add(netif, ipaddr, netmask, gw, mac_address, XPAR_XEMACPS_0_BASEADDR)) {
		xil_printf("Error adding N/W interface\n\r");
		exit(0);
	}

	netif_set_default(&netif);
	netif_set_up(&netif);

	xil_printf("IP Address: %d.%d.%d.%d\n\r", ip4_addr1(ipaddr), ip4_addr2(ipaddr), ip4_addr3(ipaddr), ip4_addr4(ipaddr));
	xil_printf("Netmask: %d.%d.%d.%d\n\r", ip4_addr1(netmask), ip4_addr2(netmask), ip4_addr3(netmask), ip4_addr4(netmask));
	xil_printf("Gateway: %d.%d.%d.%d\n\r", ip4_addr1(gw), ip4_addr2(gw), ip4_addr3(gw), ip4_addr4(gw));
}

void tick()
{
	XTmrCtr_Start(&TimerInstancePtr,0);
	while(TIMER_INTR_FLG == false){
	}

	TIMER_INTR_FLG = false;

	if (buf == 0) {
		memcpy((void*)IMG_BUF_PTR, image_pointer[0], NUM_BYTES_BUFFER);
	}

	buf = inbyte();

	if(buf == '1'){
		setColours(0, image_pointer);
		memcpy((void*)IMG_BUF_PTR, image_pointer[img], NUM_BYTES_BUFFER);

	}
	else if(buf == '2'){
		setColours(1, image_pointer);
		memcpy((void*)IMG_BUF_PTR, image_pointer[img], NUM_BYTES_BUFFER);

	}
	else if(buf == 'w'){ // Shift up
		if(img == 0){
			memcpy((void*)IMG_BUF_PTR, image_pointer[4], NUM_BYTES_BUFFER);
			img = 4;
		}
		else {
			memcpy((void*)IMG_BUF_PTR, image_pointer[img-1], NUM_BYTES_BUFFER);
			img--;
		}
	}
	else if(buf == 's'){ // Shift down
		if(img == 4){
			memcpy((void*)IMG_BUF_PTR, image_pointer[0], NUM_BYTES_BUFFER);
			img = 0;
		}
		else {
			memcpy((void*)IMG_BUF_PTR, image_pointer[img+1], NUM_BYTES_BUFFER);
			img++;
		}
	}
}

int main()
{
	setup();
	while(1) {
		tick();
	}
	return 0;
}

