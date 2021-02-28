#include <stdlib.h>
#include <string.h>
#include "xuartps.h"
#include "xparameters.h"
#include "xil_io.h"
#include "vga.h"
#include "shell.h"

#define CMD_LEN 256
#define CMD_SWITCH_SCREEN "switch"
#define UART_BASEADDR XPAR_PS7_UART_1_BASEADDR


void shell_init()
{
	u32 CntrlRegister;

	CntrlRegister = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_CR_OFFSET);
	XUartPs_WriteReg(UART_BASEADDR, XUARTPS_CR_OFFSET,
		  ((CntrlRegister & ~XUARTPS_CR_EN_DIS_MASK) |
		   XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN));
}

void shell_loop()
{
    static char buf[CMD_LEN] = "";
    char c;

    // Wait for input from UART via the terminal
	if (XUartPs_IsReceiveData(UART_BASEADDR)){
        c = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
        xil_printf("%c", c);
        if (c == '\r' || c == '\n'){
            xil_printf("\n\r");
            if (strcmp(buf, CMD_SWITCH_SCREEN) == 0) {
                xil_printf("Changing screen\n\r");
                vga_change();
            }
            buf[0] = '\0';
        }
        else {
            if(strlen(buf) < CMD_LEN - 1){
                strncat(buf, &c, 1);
            }
        }
    }
}
