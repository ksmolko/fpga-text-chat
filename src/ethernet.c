#include "xil_io.h"
#include "netif/xadapter.h"
#include "lwip/tcp.h"
#include "lwip/init.h"
#include "ethernet.h"


void ethernet_init()
{
	ip_addr_t ipaddr, netmask, gw;
	struct netif netif;
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
	netif_set_up(&netif);

	xil_printf("IP Address: %d.%d.%d.%d\n\r", ip4_addr1(&ipaddr), ip4_addr2(&ipaddr), ip4_addr3(&ipaddr), ip4_addr4(&ipaddr));
	xil_printf("Netmask: %d.%d.%d.%d\n\r", ip4_addr1(&netmask), ip4_addr2(&netmask), ip4_addr3(&netmask), ip4_addr4(&netmask));
	xil_printf("Gateway: %d.%d.%d.%d\n\r", ip4_addr1(&gw), ip4_addr2(&gw), ip4_addr3(&gw), ip4_addr4(&gw));
}
