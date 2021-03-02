#include <stdbool.h>
#include "xil_io.h"
#include "lwip/init.h"
#include "lwip/tcp.h"
#include "lwip/dhcp.h"
#include "lwip/timeouts.h"
#include "netif/xadapter.h"
#include "platform.h"
#include "serv.h"
#include "state.h"

#define BUF_SIZE 1024

typedef struct tcp_pcb tcp_pcb;

static err_t echo_accept_callback(void *arg, tcp_pcb *pcb, err_t err);
static err_t echo_recv_callback(void *arg, tcp_pcb *pcb, struct pbuf *p, err_t err);
static err_t chat_accept_callback(void *arg, tcp_pcb *pcb, err_t err);
static err_t chat_recv_callback(void *arg, tcp_pcb *pcb, struct pbuf *p, err_t err);
static void chat_err_callback(void *arg, err_t err);
void tcp_tmr(void);

extern volatile int dhcp_timeout_counter;
extern int state;
static struct netif netif;

void ethernet_init()
{
	ip_addr_t ipaddr, netmask, gw;
	unsigned char mac_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

	IP4_ADDR(&ipaddr, 192, 168, 0, 20);
	IP4_ADDR(&netmask, 255, 255, 255, 0);
	IP4_ADDR(&gw, 192, 168, 0, 1);

	lwip_init();

	if (!xemac_add(&netif, &ipaddr, &netmask, &gw, mac_address, XPAR_XEMACPS_0_BASEADDR)) {
		xil_printf("ERROR: In function %s: Error adding N/W interface\n\r", __FUNCTION__);
		return;
	}

	netif_set_default(&netif);
	interrupts_enable();
	netif_set_up(&netif);

	dhcp_inform(&netif);

	xil_printf("IP Address: %d.%d.%d.%d\n\r", ip4_addr1(&ipaddr), ip4_addr2(&ipaddr), ip4_addr3(&ipaddr), ip4_addr4(&ipaddr));
	xil_printf("Netmask: %d.%d.%d.%d\n\r", ip4_addr1(&netmask), ip4_addr2(&netmask), ip4_addr3(&netmask), ip4_addr4(&netmask));
	xil_printf("Gateway: %d.%d.%d.%d\n\r", ip4_addr1(&gw), ip4_addr2(&gw), ip4_addr3(&gw), ip4_addr4(&gw));
}

void serv_init(int serv_type, u16 port)
{
	int status;
	tcp_pcb *pcb;
	pcb = tcp_new_ip_type(IPADDR_TYPE_V4);

	if (!pcb) {
		xil_printf("ERROR: In function %s: Out of memory for PCB\n\r", __FUNCTION__);
	}

	status = tcp_bind(pcb, IP_ADDR_ANY, port);
	if (status != ERR_OK) {
		xil_printf("ERROR: In function %s: Unable to bind to port %d\n\r", __FUNCTION__, port);
	}
	else {
		if (serv_type == ECHO_SERV) {
			xil_printf("Echo server bind successful on port %d\n\r", port);
		}
		else if (serv_type == CHAT_SERV) {
			xil_printf("Chat server bind successful on port %d\n\r", port);
		}
	}

	pcb = tcp_listen(pcb);
	tcp_arg(pcb, NULL);
	tcp_err(pcb, chat_err_callback);

	if (pcb == NULL) {
		xil_printf("ERROR: In function %s: PCB is NULL");
	}
	else if (serv_type == ECHO_SERV) {
		tcp_accept(pcb, echo_accept_callback);
		xil_printf("Echo server started on port %d\n\r", port);
	}
	else if (serv_type == CHAT_SERV) {
		xil_printf("Chat server started on port %d\n\r", port);
		tcp_accept(pcb, chat_accept_callback);
	}
	else {
		xil_printf("ERROR: In function %s: Invalid serv_type", __FUNCTION__);
		tcp_close(pcb);
	}
}

void serv_loop()
{
	tcp_tmr();
	xemacif_input(&netif);
}

static err_t echo_accept_callback(void *arg, tcp_pcb *pcb, err_t err)
{
	LWIP_UNUSED_ARG(err);
	xil_printf("Echo connection request\n\r");

	tcp_recv(pcb, echo_recv_callback);

	return ERR_OK;
}

static err_t echo_recv_callback(void *arg, tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	LWIP_UNUSED_ARG(arg);

	xil_printf("Echo respond\n\r");

	if(!p) {
		tcp_close(pcb);
		tcp_recv(pcb, NULL);
		return ERR_OK;
	}

	if (err == ERR_OK && p != NULL) {
		tcp_recved(pcb, p->len);

		if (tcp_sndbuf(pcb) > p->len) {
			err = tcp_write(pcb, p->payload, p->len, TCP_WRITE_FLAG_COPY);
		}
		else {
			xil_printf("ERROR: In function %s: Buffer too large for tcp_sndbuf\n\r", __FUNCTION__);
		}
	}

	pbuf_free(p);
	return ERR_OK;
}

static err_t chat_accept_callback(void *arg, tcp_pcb *pcb, err_t err)
{
	LWIP_UNUSED_ARG(err);

	/*char c;
	bool valid = false;

	xil_printf("Incoming Connection Request. Accept? [y/n]: ");
	c = inbyte();
	xil_printf("\n\r");

	while (!valid) {
		if (c == 'y' || c == 'Y') {
			tcp_recv(pcb, chat_recv_callback);
			state = STATE_CALL;
			valid = true;
		}
		else if (c == 'n' || c == 'N') {
			tcp_close(pcb);
			tcp_recv(pcb, NULL);
			valid = true;
		}
		else {
			xil_printf("Invalid selection, try again: ");
		}
	}*/

	xil_printf("Incoming connection request\n\r");
	tcp_recv(pcb, chat_recv_callback);
	return ERR_OK;
}

static err_t chat_recv_callback(void *arg, tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	// Stub
	return ERR_OK;
}

static void chat_err_callback(void *arg, err_t err)
{
	xil_printf("ERROR: TCP server error: Code %d", *(int *)arg);
}
