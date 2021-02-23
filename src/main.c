#include "xil_cache_l.h"

#include "platform.h"
#include "vga.h"
#include "serv.h"

#include "xuartps.h"
#include <xparameters.h>
#include <xil_printf.h>
#include <xil_io.h>
#include "stdlib.h"

#define UART_BASEADDR XPAR_PS7_UART_1_BASEADDR

void menu();
void init()
{
	// Disable Caches (prevents tearing in images)
	Xil_L1DCacheDisable();
	Xil_L2CacheDisable();

	timer_init();
	interrupts_init();
	vga_init();
	//ethernet_init();
	//echoserv_init();
}

void tick()
{
	vga_loop();
	//echoserv_loop();
}

int main()
{
	init();
//	while(1) {
//		tick();
//	}
	menu();
	return 0;
}

void menu() {
	u8 inp = 0x00;
	u32 CntrlRegister;

	CntrlRegister = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_CR_OFFSET);
	XUartPs_WriteReg(UART_BASEADDR, XUARTPS_CR_OFFSET,
		  ((CntrlRegister & ~XUARTPS_CR_EN_DIS_MASK) |
		   XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN));

	xil_printf("\r\n\r\n");
	xil_printf("Milestone 1 demo\r\n");
	xil_printf("Enter 's' to toggle the screen\r\n");
	xil_printf("----------------------------------------\r\n");

	// Wait for input from UART via the terminal
	while (!XUartPs_IsReceiveData(UART_BASEADDR)) {

	}
	inp = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
	xil_printf("%c\n", inp);
	switch(inp) {
	case 's':
		xil_printf("Changing screen");
		vga_change();
		menu();
		break;
	default:
		menu();
		break;
	}
}
