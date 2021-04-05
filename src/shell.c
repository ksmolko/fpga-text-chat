#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "xuartps.h"
#include "xparameters.h"
#include "xil_io.h"
#include "vga.h"
#include "shell.h"
#include "serv.h"
#include "state.h"
#include "client.h"
#include "audio.h"

#define CMD_MAX_LEN 256
#define CMD_CONNECT "connect"
#define CMD_SWITCH_SCREEN "switch"
#define CMD_LISTEN "listen"
#define CMD_RECORD "record"
#define CMD_PLAYBACK "playback"
#define CMD_STREAM "stream"
#define CMD_RECV_PLAYBACK "recv_playback"
#define STATUS_FAIL 0
#define STATUS_SUCCESS 1
#define IP_USAGE "Usage: connect [ip] [port]\n\r"
#define LISTEN_USAGE "Usage: listen [port]\n\r"

#define UART_BASEADDR XPAR_PS7_UART_1_BASEADDR

#define PROMPT_IP "ENTER THE REMOTE IP AND PORT: "


extern int state;

static void shell_loop_connect();

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
	static char buf[CMD_MAX_LEN] = "";
	int status = STATUS_SUCCESS;
	u16 port;
	char c;
	char *token;
	char *context;
	ip_addr_t ip;
	IP4_ADDR(&ip, 0, 0, 0, 0);

	// Wait for input from UART via the terminal
	if (state == STATE_MENU_CONNECT) {
		shell_loop_connect();
	}
	else if (XUartPs_IsReceiveData(UART_BASEADDR)) {
		c = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
		if (c == '\r' || c == '\n') {
			xil_printf("\n\r");
			token = strtok_r(buf, " ", &context);

			// List of all commands
			if (strcmp(token, CMD_SWITCH_SCREEN) == 0) {
				xil_printf("Changing screen\n\r");
				vga_change();
			}
			else if (strcmp(token, CMD_CONNECT) == 0) {
				token = strtok_r(NULL, " ", &context);

				// Parse connect command
				if (token == NULL) {
					status = STATUS_FAIL;
					state = STATE_MENU_CONNECT;
					vga_switch_to_connect();
				}
				else {
					status = ipaddr_aton(token, &ip);
					if (!status) {
						xil_printf(IP_USAGE);
					}
					else {
						token = strtok_r(NULL, " ", &context);
						if (token == NULL) {
							status = STATUS_FAIL;
							xil_printf(IP_USAGE);
						}
						else {
							port = atoi(token);
							if (port == 0) {
								status = STATUS_FAIL;
								xil_printf(IP_USAGE);
							}
						}
					}
				}
				if (status == STATUS_SUCCESS) {
					client_connect(&ip, port);
				}
			}
			else if (strcmp(token, CMD_LISTEN) == 0) {
				token = strtok_r(NULL, " ", &context);

				if (token == NULL) {
					xil_printf(LISTEN_USAGE);
				}
				else {
					port = atoi(token);
					serv_init(CHAT_SERV, port);
				}
			}
			else if (strcmp(token, CMD_RECORD) == 0) {
				audio_record();
			}
			else if (strcmp(token, CMD_PLAYBACK) == 0) {
				audio_playback();
			}
			else if (strcmp(token, CMD_STREAM) == 0) {
				audio_stream();
			}
			else if (strcmp(token, CMD_RECV_PLAYBACK) == 0) {
				chat_audio_playback_recv();
			}

			buf[0] = '\0';
		}
		else {
			if(strlen(buf) < CMD_MAX_LEN - 1) {
				xil_printf("%c", c);
				strncat(buf, &c, 1);
			}
		}
	}
}

static void shell_loop_connect()
{
	static bool prompted = false;
	static int vga_x_offset = 0;
	static int vga_y_offset = 0;
	int status = STATUS_SUCCESS;
	char c;
	char *token;
	char *context;
	char buf[CMD_MAX_LEN] = "";
	ip_addr_t ip;
	u16 port;


	if (!prompted) {
		xil_printf(PROMPT_IP);
		vga_print_string(vga_x_offset, vga_y_offset, PROMPT_IP);
		vga_y_offset += ALPHABET_CHAR_LENGTH;
		prompted = true;
	}

	if (XUartPs_IsReceiveData(UART_BASEADDR)) {
		c = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
		if (c == '\r' || c == '\n') {
			xil_printf("\n\r");
			token = strtok_r(buf, ":", &context);
			vga_x_offset = 0;
			vga_y_offset += ALPHABET_CHAR_LENGTH;
			status = ipaddr_aton(token, &ip);
			if (!status) {
				xil_printf("Invalid IP: Try again\n\r");
			}
			else {
				token = strtok_r(NULL, ":", &context);
				port = atoi(token);
				if (port == 0) {
					status = STATUS_FAIL;
				}
				if (status == STATUS_SUCCESS) {
					client_connect(&ip, port);
				}
			}
			buf[0] = '\0';
		}
		else {
			if(strlen(buf) < CMD_MAX_LEN - 1) {
				xil_printf("%c", c);
				vga_print_character(vga_x_offset, vga_y_offset, c);
				vga_x_offset += ALPHABET_CHAR_LENGTH;
				strncat(buf, &c, 1);
			}
		}
	}
}
