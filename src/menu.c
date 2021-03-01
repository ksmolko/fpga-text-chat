#include <stdlib.h>
#include "xuartps.h"
#include "xparameters.h"
#include "xil_io.h"
#include "vga.h"
#include "menu.h"
#include "keyboard_logic.h"

#define UART_BASEADDR XPAR_PS7_UART_1_BASEADDR


void menu_init()
{
	xil_printf("Initializing menu\n\r");
	u32 CntrlRegister;

	CntrlRegister = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_CR_OFFSET);
	XUartPs_WriteReg(UART_BASEADDR, XUARTPS_CR_OFFSET,
		  ((CntrlRegister & ~XUARTPS_CR_EN_DIS_MASK) |
		   XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN));

	Keyboard_init();
}

void menu_loop()
{
    u8 inp = 0x00;
    // Wait for input from UART via the terminal
	if (XUartPs_IsReceiveData(UART_BASEADDR)){
        inp = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);

        switch(inp) {
        case 'p':
            xil_printf("Changing screen\n\r");
            vga_change();
            break;
        case 'w':
        	xil_printf("Pressing w\n\r");
        	Press_up();
        	break;
        case 'a':
        	xil_printf("Pressing a\n\r");
        	Press_left();
        	break;
        case 's':
        	xil_printf("Pressing s\n\r");
        	Press_down();
        	break;
        case 'd':
        	xil_printf("Pressing d\n\r");
        	Press_right();
        	break;
        case 'x':
        	xil_printf("Pressing x\n\r");
        	Press_center();
        	break;
        default:
            break;
        }
    }
}
