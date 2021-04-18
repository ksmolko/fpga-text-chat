#include "xil_io.h"
#include "xparameters.h"
#include "xuartps.h"
#include "netif/xadapter.h"
#include "state.h"
#include "chat.h"
#include "vga.h"
#include "client.h"


static err_t chat_connect_callback(void *arg, tcp_pcb *pcb, err_t err);
static err_t chat_recv_callback(void *arg, tcp_pcb *pcb, struct pbuf *p, err_t err);
static void chat_err_callback(void *arg, err_t err);

extern int state;
extern struct netif netif;
static tcp_pcb *client_pcb;
static int status_x_offset = 0;
static int status_y_offset = 0;

void client_init()
{
	int status;
	client_pcb = tcp_new_ip_type(IPADDR_TYPE_V4);

	if (!client_pcb) {
		xil_printf("ERROR: In function %s: Out of memory for PCB\n\r", __FUNCTION__);
		return;
	}

	status = tcp_bind(client_pcb, IP_ADDR_ANY, LOCAL_CHAT_PORT);
	if (status != ERR_OK) {
		xil_printf("ERROR: In function %s: Unable to bind to port %d\n\r", __FUNCTION__, LOCAL_CHAT_PORT);
		return;
	}
}

void client_connect(const ip_addr_t *ip, u16 port)
{
	int status;

	xil_printf("Remote IP: %d.%d.%d.%d\n\r", ip4_addr1(ip), ip4_addr2(ip), ip4_addr3(ip), ip4_addr4(ip));
	xil_printf("Remote Port: %d\n\r", port);

	tcp_err(client_pcb, chat_err_callback);
	status = tcp_connect(client_pcb, ip, port, chat_connect_callback);

	if (status != ERR_OK) {
		xil_printf("Error: Code %d\n\r", status);
	}
	else {
		state = STATE_REQUEST;
		xil_printf("Connection request sent...\n\r");
		vga_print_string(HORIZONTAL_PIXEL_MAX/2 - HORIZONTAL_PIXEL_MAX/12
			, VERTICAL_PIXEL_MAX/2
			, "Connection request Sent...");
	}
}

void client_loop()
{
	xemacif_input(&netif);

	if (state == STATE_CALL_CLIENT) {
		chat_loop(client_pcb);
	}
}

static err_t chat_connect_callback(void *arg, tcp_pcb *pcb, err_t err)
{
	tcp_recv(pcb, chat_recv_callback);
	return ERR_OK;
}

static err_t chat_recv_callback(void *arg, tcp_pcb *pcb, pbuf *p, err_t err)
{
	LWIP_UNUSED_ARG(arg);

	int status;
	char port_str[6];

	if (!p) {
		tcp_close(pcb);
		tcp_recv(pcb, NULL);
		pbuf_free(p);
		return ERR_OK;
	}

	char test[10] = "";
	if (state == STATE_REQUEST) {
		if (err == ERR_OK && p != NULL) {
			pbuf_copy_partial(p, test, p->tot_len, 0);
			xil_printf("Response received: %s\n\r", test);
			tcp_recved(pcb, p->len);

			if (tcp_sndbuf(pcb) > p->len) {
				if (pbuf_memcmp(p, 0, "y", 1) == 0 || pbuf_memcmp(p, 0, "Y", 1) == 0) {
					state = STATE_CALL_CLIENT;
					vga_switch_to_CHAT();
					vga_print_string(status_x_offset, status_y_offset, ipaddr_ntoa((ip_addr_t *)&(pcb->remote_ip)));
					status_y_offset += ALPHABET_CHAR_LENGTH;
					sprintf(port_str, "%d", pcb->remote_port);
					vga_print_string(status_x_offset, status_y_offset, port_str);
					xil_printf("Connection request accepted. Call has begun\n\n\r");
				}
				else if(pbuf_memcmp(p, 0, "n", 1) == 0 || pbuf_memcmp(p, 0, "N", 1) == 0) {
					state = STATE_MENU;
					xil_printf("Connection request refused.\n\r");
					tcp_close(pcb);
					tcp_recv(pcb, NULL);
				}
				else {

					state = STATE_MENU;
					xil_printf("Indeterminate connection state. Returning to menu.");
					tcp_close(pcb);
					tcp_recv(pcb, NULL);
				}
			}
			else {
				xil_printf("ERROR: In function %s: Buffer too large for tcp_sndbuf\n\r", __FUNCTION__);
			}
		}
	}
	else if(state == STATE_CALL_CLIENT) {
		status = chat_rcv(pcb, p, err);
	}

	pbuf_free(p);
	return status;
}

static void chat_err_callback(void *arg, err_t err)
{
	tcp_close(client_pcb);
	tcp_recv(client_pcb, NULL);
	xil_printf("ERROR: TCP client error: Code %d\n\rReturning to menu\n\r", err);
	state = STATE_MENU;
}
