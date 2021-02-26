#include <stdlib.h>
#include "xuartps.h"
#include "xparameters.h"
#include "xil_io.h"
#include "vga.h"
#include "menu.h"

#define UART_BASEADDR XPAR_PS7_UART_1_BASEADDR


void menu_init()
{
	u32 CntrlRegister;

	CntrlRegister = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_CR_OFFSET);
	XUartPs_WriteReg(UART_BASEADDR, XUARTPS_CR_OFFSET,
		  ((CntrlRegister & ~XUARTPS_CR_EN_DIS_MASK) |
		   XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN));
}

void menu_loop()
{
    u8 inp = 0x00;
    // Wait for input from UART via the terminal
	if (XUartPs_IsReceiveData(UART_BASEADDR)){
        inp = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
        xil_printf("%c\n", inp);

        switch(inp) {
        case 's':
            xil_printf("Changing screen");
            vga_change();
            break;
        default:
            break;
        }
    }
}
