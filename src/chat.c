#include <stdbool.h>
#include "xparameters.h"
#include "xuartps.h"
#include "netif/xadapter.h"
#include "state.h"
#include "vga.h"
#include "chat.h"

#define CMD_CLOSE "/close"
#define RECEIVED_HEADER "Other: "
#define RECEIVED_HEADER_LEN 8


extern struct netif netif;
extern int state;

static int chat_box_x_offset = 0;
static int chat_box_y_offset = VERTICAL_PIXEL_MAX/10;
static int chat_buf_x_offset = 0;
static int chat_buf_y_offset = VERTICAL_PIXEL_MAX*4/5 - ALPHABET_CHAR_LENGTH;

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
			vga_print_string(chat_box_x_offset, chat_box_y_offset, buf);
			buf[0] = '\0';
			vga_clear_chat_buf();
			chat_box_y_offset += ALPHABET_CHAR_LENGTH;
			chat_buf_y_offset = VERTICAL_PIXEL_MAX*4/5 - ALPHABET_CHAR_LENGTH;
			chat_buf_x_offset = 0;
			if (chat_box_y_offset + ALPHABET_CHAR_LENGTH >= VERTICAL_PIXEL_MAX*4/5 - ALPHABET_CHAR_LENGTH) {
				// TODO: Shift messages up
			}
		}
		else {
			if(strlen(buf) < MSG_MAX_LEN - 1) {
				xil_printf("%c", c);
				vga_print_character(chat_buf_x_offset, chat_buf_y_offset, c);
				chat_buf_x_offset += ALPHABET_CHAR_LENGTH;
				if (chat_buf_x_offset - ALPHABET_CHAR_LENGTH >= HORIZONTAL_PIXEL_MAX) {
					chat_buf_x_offset = 0;
					chat_buf_y_offset += ALPHABET_CHAR_LENGTH;
				}
				strncat(buf, &c, 1);
			}
		}
	}
}

err_t chat_rcv(tcp_pcb *pcb, pbuf *p, err_t err)
{
	static bool first = true;
	char *msg;
	char vga_printout[MSG_MAX_LEN + RECEIVED_HEADER_LEN] = "Other: ";

	if (state == STATE_CALL_SERVER) {
		first = false;
	}

	if (err == ERR_OK && p != NULL) {
		do {
			msg = (char *)(p->payload);
			tcp_recved(pcb, p->len);
			if (strncmp(msg, CMD_CLOSE, strlen(CMD_CLOSE)) == 0) {
				tcp_close(pcb);
				tcp_recv(pcb, NULL);
				pbuf_free(p);
				xil_printf("\n\rConnection closed by other user\n\rReturning to menu\n\r");
				vga_switch_to_IP();
				state = STATE_MENU;
				return ERR_OK;
			}
			else {
				if (first) {
					first = false;
				}
				else {
					xil_printf("\n\r%s%s\n\r", RECEIVED_HEADER, msg);
					strcat(vga_printout, msg);
					vga_print_string(chat_buf_x_offset, chat_box_y_offset, vga_printout);
					chat_box_y_offset += ALPHABET_CHAR_LENGTH;
				}
			}
		} while (p->next != NULL);
	}

	return ERR_OK;
}
