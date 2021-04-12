#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "xuartps.h"
#include "xparameters.h"
#include "xil_io.h"
#include "otp_crypt.h"
#include "vga.h"
#include "shell.h"
#include "serv.h"
#include "state.h"
#include "client.h"
#include "keyboard_logic.h"

#define CMD_MAX_LEN 256
#define CMD_CONNECT "connect"
#define CMD_SWITCH_SCREEN "switch"
#define CMD_LISTEN "listen"
#define STATUS_FAIL 0
#define STATUS_SUCCESS 1
#define IP_USAGE "Usage: connect [ip] [port]\n\r"
#define LISTEN_USAGE "Usage: listen [port]\n\r"
#define UART_BASEADDR XPAR_PS7_UART_1_BASEADDR

#define PROMPT_IP "ENTER THE REMOTE IP AND PORT: "


extern int state;
extern keyboard_state kb_state;

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
	if (XUartPs_IsReceiveData(UART_BASEADDR)) {
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
					state = STATE_MENU_LISTEN;
					port = atoi(token);
					serv_init(CHAT_SERV, port);
				}
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

	char character = ' ';
	char ipAddr_c[15] = "";
	char port_c[10] = "";
	char white_space = ' ';
	int is_got_space = 0;
	if (kb_state.ip_kb_enter_pressed == 1) {
		if (state == STATE_MENU_CONNECT) {
			for (char i = 0; i < kb_state.buffer_offset; ++i) {
				memcpy(&character, (void*)BUFFER_BASE_ADDR + i, 1);
				if (memcmp(&character, &white_space, 1) == 0) {
					is_got_space = 1;
				}
				if (is_got_space == 0) {
					strncat(ipAddr_c, &character, 1);
				} else {
					strncat(port_c, &character, 1);
				}
			}

			xil_printf("IP Addr: %s\n\r", ipAddr_c);
			xil_printf("Port num: %s\n\r", port_c);

			status = ipaddr_aton(ipAddr_c, &ip);
			port = atoi(port_c);
			client_connect(&ip, port);
		} else if (state == STATE_MENU_LISTEN) {
			for (char i = 0; i < kb_state.buffer_offset; ++i) {
				memcpy(&character, (void*)BUFFER_BASE_ADDR + i, 1);
				strncat(port_c, &character, 1);
			}
			xil_printf("Port num: %s\n\r", port_c);
			port = atoi(port_c);
			serv_init(CHAT_SERV, port);
		}
	}
}
