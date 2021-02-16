#include "xil_io.h"
#include "lwip/init.h"
#include "lwip/tcp.h"
#include "lwip/dhcp.h"
#include "lwip/timeouts.h"
#include "netif/xadapter.h"
#include "platform.h"
#include "serv.h"

#define BUF_SIZE 1024

static err_t accept_callback(void *arg, struct tcp_pcb *pcb, err_t err);
static err_t recv_callback(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
void tcp_tmr(void);

extern volatile int dhcp_timeout_counter;
struct tcp_pcb *pcb;
struct netif netif;

void ethernet_init()
{
	ip_addr_t ipaddr, netmask, gw;
	unsigned char mac_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

	IP4_ADDR(&ipaddr, 192, 168, 0, 20);
	IP4_ADDR(&netmask, 255, 255, 255, 0);
	IP4_ADDR(&gw, 192, 168, 0, 1);

	lwip_init();

	if (!xemac_add(&netif, &ipaddr, &netmask, &gw, mac_address, XPAR_XEMACPS_0_BASEADDR)) {
		xil_printf("Error adding N/W interface\n\r");
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

void echoserv_init()
{
	int status;
	u16_t port = 7;
	pcb = tcp_new_ip_type(IPADDR_TYPE_V4);

	if (!pcb) {
		xil_printf("ERROR: Out of memory for PCB\n\r");
	}

	status = tcp_bind(pcb, IP4_ADDR_ANY, port);
	if (status != ERR_OK) {
		xil_printf("ERROR: Unable to bind to port %d\n\r", port);
	}
	else {
		xil_printf("Bind Successful\n\r");
	}

	pcb = tcp_listen(pcb);
	tcp_accept(pcb, accept_callback);
}

void echoserv_loop()
{
	tcp_tmr();
	xemacif_input(&netif);
}

static err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(err);

	tcp_recv(newpcb, recv_callback);
	tcp_arg(newpcb, NULL);

	return ERR_OK;
}

static err_t recv_callback(void *arg, struct tcp_pcb *newpcb, struct pbuf *p, err_t err)
{
	LWIP_UNUSED_ARG(arg);

	if(!p) {
		tcp_close(newpcb);
		tcp_recv(newpcb, NULL);
		return ERR_OK;
	}

	if (err == ERR_OK && p != NULL) {
		tcp_recved(newpcb, p->len);

		if (tcp_sndbuf(newpcb) > p->len) {
			err = tcp_write(newpcb, p->payload, p->len, TCP_WRITE_FLAG_COPY);
		}
		else {
			xil_printf("ERROR: Buffer too large for tcp_sndbuf\n\r");
		}
	}

	pbuf_free(p);
	return ERR_OK;
}
