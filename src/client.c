#include "xil_io.h"
#include "netif/xadapter.h"
#include "state.h"
#include "client.h"

#define LOCAL_CHAT_PORT 25567


typedef struct tcp_pcb tcp_pcb;

static err_t chat_connect_callback(void *arg, tcp_pcb *pcb, err_t err);
static err_t chat_recv_callback(void *arg, tcp_pcb *pcb, struct pbuf *p, err_t err);
static void chat_err_callback(void *arg, err_t err);

extern int state;
extern struct netif netif;

void client_connect(const ip_addr_t *ip, u16 port)
{
	int status;
	tcp_pcb *pcb;
	pcb = tcp_new_ip_type(IPADDR_TYPE_V4);

	if (!pcb) {
		xil_printf("ERROR: In function %s: Out of memory for PCB\n\r", __FUNCTION__);
		return;
	}

	status = tcp_bind(pcb, IP_ADDR_ANY, port);
	if (status != ERR_OK) {
		xil_printf("ERROR: In function %s: Unable to bind to port %d\n\r", __FUNCTION__, port);
		return;
	}
	else {
		xil_printf("Client bind Successful on port %d\n\r", port);
	}

	xil_printf("Remote IP: %d.%d.%d.%d\n\r", ip4_addr1(ip), ip4_addr2(ip), ip4_addr3(ip), ip4_addr4(ip));
	xil_printf("Remote Port: %d\n\r", port);

	tcp_err(pcb, chat_err_callback);
	status = tcp_connect(pcb, ip, port, chat_connect_callback);

	if (status != ERR_OK) {
		xil_printf("Error: Code %d\n\r", status);
	}
	else {
		state = STATE_REQUEST;
		xil_printf("Connection request sent...\n\r");
	}
}

void client_loop()
{
	xemacif_input(&netif);
}

static err_t chat_connect_callback(void *arg, tcp_pcb *pcb, err_t err)
{
	tcp_recv(pcb, chat_recv_callback);
	xil_printf("Connection request received\n\r");
	return ERR_OK;
}

static err_t chat_recv_callback(void *arg, tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	LWIP_UNUSED_ARG(arg);

	if (!p) {
		tcp_close(pcb);
		tcp_recv(pcb, NULL);
		return ERR_OK;
	}

	if (state == STATE_REQUEST) {
		if (err == ERR_OK && p != NULL) {
			tcp_recved(pcb, p->len);

			if (tcp_sndbuf(pcb) > p->len) {
				if (pbuf_memcmp(p, 0, "y", 1) == 0 || pbuf_memcmp(p, 0, "Y", 1) == 0) {
					state = STATE_CALL_CLIENT;
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

	pbuf_free(p);
	return ERR_OK;
}

static void chat_err_callback(void *arg, err_t err)
{
	xil_printf("ERROR: TCP client error: Code %d\n\r", err);
}