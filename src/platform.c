#include <stdbool.h>
#include "xil_exception.h"
#include "xparameters.h"
#include "xscutimer.h"
#include "lwip/dhcp.h"
#include "platform.h"


static XScuTimer tmr_instance;
volatile int dhcp_timeout_counter = 24;

void interrupt_handler()
{
	static bool odd = true;
	static int dhcp_timer = 0;

	odd = !odd;

	if (odd){
		dhcp_timer++;
		dhcp_timeout_counter--;
		dhcp_fine_tmr();
		if (dhcp_timer >= 120){
			dhcp_coarse_tmr();
			dhcp_timer = 0;
		}
	}
}

void timer_init()
{
	int status;
	int load_value = XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 8; // 250ms

	XScuTimer_Config *tmr_cfg;
	tmr_cfg = XScuTimer_LookupConfig(XPAR_SCUTIMER_DEVICE_ID);
	status = XScuTimer_CfgInitialize(&tmr_instance, tmr_cfg, tmr_cfg->BaseAddr);

	if (status != XST_SUCCESS){
		xil_printf("ERROR: ScuTimer Cfg initialization failed\n\r");
		return;
	}

	status = XScuTimer_SelfTest(&tmr_instance);
	if (status != XST_SUCCESS){
		xil_printf("ERROR: ScuTimer self test failed\n\r");
		return;
	}

	XScuTimer_EnableAutoReload(&tmr_instance);
	XScuTimer_LoadTimer(&tmr_instance, load_value);
}

void interrupts_init()
{
	Xil_ExceptionInit();
	XScuGic_DeviceInitialize(XPAR_SCUGIC_SINGLE_DEVICE_ID);
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT, (Xil_ExceptionHandler)XScuGic_DeviceInterruptHandler, (void *)XPAR_SCUGIC_SINGLE_DEVICE_ID);
	XScuGic_RegisterHandler(XPAR_SCUGIC_SINGLE_DEVICE_ID, XPAR_SCUTIMER_INTR, (Xil_ExceptionHandler)interrupt_handler, NULL);
}

void interrupts_enable()
{
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);
	XScuTimer_EnableInterrupt(&tmr_instance);
	XScuTimer_Start(&tmr_instance);
	return;
}
