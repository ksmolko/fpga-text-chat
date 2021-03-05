#include <stdbool.h>
#include "xil_io.h"
#include "xparameters.h"
#include "xuartps.h"
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

extern volatile int dhcp_timeout_counter;
extern int state;
struct netif netif;
static tcp_pcb *chat_pcb = NULL;

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

	if (chat_pcb != NULL) {
		xil_printf("Chat server already open. Replacing...\n\r");
		tcp_close(chat_pcb);
		tcp_recv(chat_pcb, NULL);
	}

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
		chat_pcb = pcb;
	}
	else {
		xil_printf("ERROR: In function %s: Invalid serv_type", __FUNCTION__);
		tcp_close(pcb);
	}
}

void serv_loop()
{
	char c;

	xemacif_input(&netif);

	if (state == STATE_ACCEPT){
		if (XUartPs_IsReceiveData(XPAR_PS7_UART_1_BASEADDR)){
			c = XUartPs_ReadReg(XPAR_PS7_UART_1_BASEADDR, XUARTPS_FIFO_OFFSET);
			xil_printf("%c\n\r", c);
			if (c == 'y' || c == 'Y'){
				state = STATE_CALL_SERV;
				xil_printf("Chat has begun\n\n\r");
				tcp_write(chat_pcb, (void *)&c, 1, TCP_WRITE_FLAG_COPY);
				tcp_output(chat_pcb);
			}
			else if (c == 'n' || c == 'N'){
				state = STATE_MENU;
				xil_printf("Refusing connection. Returning to menu\n\r");
				tcp_write(chat_pcb, (void *)&c, 1, TCP_WRITE_FLAG_COPY);
				tcp_output(chat_pcb);
			}
			else {
				xil_printf("Invalid selection, try again: ");
			}
		}
	}
}

static err_t echo_accept_callback(void *arg, tcp_pcb *pcb, err_t err)
{
	LWIP_UNUSED_ARG(err);

	tcp_recv(pcb, echo_recv_callback);

	return ERR_OK;
}

static err_t echo_recv_callback(void *arg, tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	LWIP_UNUSED_ARG(arg);

	if (!p) {
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

	state = STATE_ACCEPT;
	xil_printf("Incoming Connection Request. Accept? [y/n]: ");
	tcp_recv(pcb, chat_recv_callback);
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

	if (state == STATE_MENU){
		tcp_close(pcb);
		tcp_recv(pcb, NULL);
		return ERR_OK;
	}

	// Stub

	pbuf_free(p);
	return ERR_OK;
}

static void chat_err_callback(void *arg, err_t err)
{
	xil_printf("ERROR: TCP server error: Code %d", *(int *)arg);
}
