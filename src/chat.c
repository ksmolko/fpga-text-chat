#include "xparameters.h"
#include "xuartps.h"
#include "netif/xadapter.h"
#include "state.h"
#include "chat.h"

#define CMD_CLOSE "/close"


extern struct netif netif;
extern int state;

void chat_loop(tcp_pcb *pcb)
{
	static char buf[MSG_MAX_LEN] = "";
	err_t err;
	char c;

	if (XUartPs_IsReceiveData(XPAR_PS7_UART_1_BASEADDR)) {
		c = XUartPs_ReadReg(XPAR_PS7_UART_1_BASEADDR, XUARTPS_FIFO_OFFSET);
		if (c == '\r' || c == '\n') {
			xil_printf("\n\r");
			err = tcp_write(pcb, (void *)buf, strlen(buf) + 1, TCP_WRITE_FLAG_COPY);
			if (err != ERR_OK) {
				xil_printf("ERROR: tcp_write() error: Code %d\n\r", err);
			}
			err = tcp_output(pcb);
			if (err != ERR_OK) {
				xil_printf("ERROR: tcp_output() error: Code %d\n\r", err);
			}
			buf[0] = '\0';
		}
		else {
			if(strlen(buf) < MSG_MAX_LEN - 1) {
				xil_printf("%c", c);
				strncat(buf, &c, 1);
			}
		}
	}
}

err_t chat_rcv(tcp_pcb *pcb, pbuf *p, err_t err)
{
	char *msg;

	if (err == ERR_OK && p != NULL) {
		do {
			msg = (char *)(p->payload);
			tcp_recved(pcb, p->len);
			if (strncmp(msg, CMD_CLOSE, strlen(CMD_CLOSE)) == 0) {
				tcp_close(pcb);
				tcp_recv(pcb, NULL);
				pbuf_free(p);
				xil_printf("\n\rConnection closed by other user\n\rReturning to menu\n\r");
				state = STATE_MENU;
				return ERR_OK;
			}
			else {
				xil_printf("\n\rOther: %s\n\r", msg);
			}
		} while (p->next != NULL);
	}

	return ERR_OK;
}
