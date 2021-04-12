#include <stdbool.h>
#include "xil_exception.h"
#include "xparameters.h"
#include "xscugic.h"
#include "xscutimer.h"
#include "lwip/tcp.h"
#include "lwip/dhcp.h"
#include "netif/xadapter.h"
#include "platform.h"
#include "xgpio.h"
#include "keyboard_logic.h"
#include "state.h"


// Parameter definitions
#define INTC_DEVICE_ID 		XPAR_PS7_SCUGIC_0_DEVICE_ID
#define BTNS_DEVICE_ID		XPAR_AXI_GPIO_0_DEVICE_ID
#define INTC_GPIO_INTERRUPT_ID XPAR_FABRIC_AXI_GPIO_0_IP2INTC_IRPT_INTR

#define BTN_INT 			XGPIO_IR_CH1_MASK

#define BTN_RELEASE 0x00
#define BTNU 0x10
#define BTNC 0x1
#define BTND 0x2
#define BTNL 0x4
#define BTNR 0x8


extern int state;		// From main.c

int is_in_kb = 0;
void tcp_tmr(void);

static XScuTimer tmr_instance;
static XGpio BTNInst;
static XScuGic INTCInst;
volatile int dhcp_timeout_counter = 24;


static int btn_value;



void BTN_Intr_Handler(void *InstancePtr)
{
	// Disable GPIO interrupts
	XGpio_InterruptDisable(&BTNInst, BTN_INT);
	// Ignore additional button presses
	if ((XGpio_InterruptGetStatus(&BTNInst) & BTN_INT) !=
			BTN_INT) {
			return;
		}
	btn_value = XGpio_DiscreteRead(&BTNInst, 1);

	switch (btn_value) {
	case BTN_RELEASE:

		break;
	case BTNC:

		// Cursor logic goes here

		// KB logic goes here
		if (is_in_kb == 1) {
			Press_center();
		}
		break;
	case BTNU:
		if (state == STATE_MENU || state == STATE_MENU_CONNECT || state == STATE_MENU_LISTEN) {
			if (is_in_kb == 1) {
				Press_up();
			}
		}
		if (state == STATE_CALL_CLIENT || state == STATE_CALL_SERVER) {
			if (is_in_kb == 1) {
				Press_up();
			} else {
				xil_printf("Called Recording()\n\r");
				is_in_kb = 1;
			}
		}

		break;
	case BTND:
		if (state == STATE_MENU || state == STATE_MENU_CONNECT || state == STATE_MENU_LISTEN) {
			if (is_in_kb == 1) {
				Press_down();
			}
		}
		if (state == STATE_CALL_CLIENT || state == STATE_CALL_SERVER) {
			if (is_in_kb == 1) {
				Press_down();
			} else {
				xil_printf("Called Keyboard_init()\n\r");
				Keyboard_init();
				is_in_kb = 1;
			}
		}
		break;
	case BTNL:
		// IP Screen
		if (state == STATE_MENU || state == STATE_MENU_CONNECT || state == STATE_MENU_LISTEN) {
			// Connect
			if (is_in_kb == 1) {
				Press_left();
			} else {
				Keyboard_init();
				is_in_kb = 1;
				state = STATE_MENU_CONNECT;
			}
		}

		// Chat Screen
		if (state == STATE_CALL_CLIENT || state == STATE_CALL_SERVER) {
			if (is_in_kb == 1) {
				Press_left();
			} else {
					xil_printf("Called History()\n\r");
					Keyboard_init();
					is_in_kb = 1;
				}
			}

		break;
	case BTNR:
		// IP Screen
		if (state == STATE_MENU || state == STATE_MENU_CONNECT || state == STATE_MENU_LISTEN) {
			// Listen
			if (is_in_kb == 1) {
				Press_right();
			} else {
				Keyboard_init();
				is_in_kb = 1;
				state = STATE_MENU_LISTEN;
			}
		}

		// Chat Screen
		if (state == STATE_CALL_CLIENT || state == STATE_CALL_SERVER) {
			if (is_in_kb == 1) {
				Press_right();
			} else {
				xil_printf("Called History()\n\r");
				Keyboard_init();
				is_in_kb = 1;
			}
		}

		break;
	default:
		break;
	}

    (void)XGpio_InterruptClear(&BTNInst, BTN_INT);
    // Enable GPIO interrupts
    XGpio_InterruptEnable(&BTNInst, BTN_INT);
}

// interrupt_handler() gets called every 250ms
void interrupt_handler()
{
	static bool odd = true;
	static int dhcp_timer = 0;

	tcp_tmr();

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

void interrupts_init()
{
	XScuTimer_Config *tmr_cfg;
	XScuGic_Config *IntcConfig;
	int status;

	// Interrupt controller initialisation
	// Look up the config information for the GIC
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	// Initialise the GIC using the config information
	status = XScuGic_CfgInitialize(&INTCInst, IntcConfig, IntcConfig->CpuBaseAddress);
	if (status != XST_SUCCESS){
		xil_printf("ERROR: XScuGic Cfg initialization failed\n\r");
		return;
	}

	// Timer related
	// Look up the the config information for the timer
	tmr_cfg = XScuTimer_LookupConfig(XPAR_SCUTIMER_DEVICE_ID);
	// Initialise the timer using the config information
	status = XScuTimer_CfgInitialize(&tmr_instance, tmr_cfg, tmr_cfg->BaseAddr);
	if (status != XST_SUCCESS){
		xil_printf("ERROR: ScuTimer Cfg initialization failed\n\r");
		return;
	}

	// Self Test for Timer
	status = XScuTimer_SelfTest(&tmr_instance);
	if (status != XST_SUCCESS){
		xil_printf("ERROR: ScuTimer self test failed\n\r");
		return;
	}

	// GPIO related
	// Initialise Push Buttons
	status = XGpio_Initialize(&BTNInst, BTNS_DEVICE_ID);
	if (status != XST_SUCCESS){
		xil_printf("ERROR: XGpio Cfg initialization failed\n\r");
		return;
	}

	// Set all buttons direction to inputs
	XGpio_SetDataDirection(&BTNInst, 1, 0xFF);




	// Initialize Exception handling on the ARM processor
	Xil_ExceptionInit();

	// Connect the supplied Xilinx general interrupt handler
	// to the interrupt handling logic in the processor.
	// All interrupts go through the interrupt controller, so the
	// ARM processor has to first "ask" the interrupt controller
	// which peripheral generated the interrupt.  The handler that
	// does this is supplied by Xilinx and is called "XScuGic_InterruptHandler"
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, &INTCInst);

	// Assign (connect) our interrupt handler for our Timer
	status = XScuGic_Connect(&INTCInst,
								XPAR_SCUTIMER_INTR,
								(Xil_ExceptionHandler)interrupt_handler,
								NULL);
	if (status != XST_SUCCESS){
		xil_printf("ERROR: XScuGic connect timer failed\n\r");
		return;
	}



	// Assign (connect) our interrupt handler for our push buttons
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, &INTCInst);
	status = XScuGic_Connect(&INTCInst,
							 	 INTC_GPIO_INTERRUPT_ID,
						  	  	 (Xil_ExceptionHandler)BTN_Intr_Handler,
						  	  	 &BTNInst);
	if (status != XST_SUCCESS){
		xil_printf("ERROR: XScuGic connect gpio failed\n\r");
		return;
	}

	// Enable the interrupt *input* on the GIC for the timer's interrupt
	XScuGic_Enable(&INTCInst, XPAR_SCUTIMER_INTR);



	// Enable the interrupt *output* in the timer.
	XScuTimer_EnableInterrupt(&tmr_instance);


	// Load the timer with a value that represents one second of real time
	int load_value = XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 8; // 250ms
	XScuTimer_LoadTimer(&tmr_instance, load_value);

	// Enable Auto reload mode on the timer.  When it expires, it re-loads
	// the original value automatically.  This means that the timing interval
	// is never skewed by the time taken for the interrupt handler to run
	XScuTimer_EnableAutoReload(&tmr_instance);

	// Enable GPIO interrupts interrupt
	// Enable the interrupt *input* on the GIC for the push buttons gpio
	XScuGic_Enable(&INTCInst, INTC_GPIO_INTERRUPT_ID);

	XGpio_InterruptEnable(&BTNInst, 1);
	XGpio_InterruptGlobalEnable(&BTNInst);

	// Enable interrupts in the ARM Processor.
	Xil_ExceptionEnable();
	xil_printf("Finish interrupts_init()\n\r");
}

void interrupts_enable()
{
	// Start the timer
	xil_printf("Starting the timer interrupt\n\r");
	XScuTimer_Start(&tmr_instance);
	return;
}

