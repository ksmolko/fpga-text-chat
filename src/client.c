#include "xil_io.h"
#include "state.h"
#include "client.h"


typedef struct tcp_pcb tcp_pcb;

static err_t chat_connect_callback(void *arg, tcp_pcb *pcb, err_t err);

extern int state;

void client_connect(const ip_addr_t *ip, u16 port)
{
    int status;
    tcp_pcb *pcb;
    pcb = tcp_new_ip_type(IPADDR_TYPE_V4);

    if (!pcb) {
		xil_printf("ERROR: In function %s: Out of memory for PCB\n\r", __FUNCTION__);
        return;
	}

    status = tcp_bind(pcb, IPADDR_TYPE_V4, port);
	if (status != ERR_OK) {
		xil_printf("ERROR: In function %s: Unable to bind to port %d\n\r", __FUNCTION__, port);
        return;
	}
	else {
		xil_printf("Client bind Successful on port %d\n\r", port);
	}

    xil_printf("Remote IP: %d.%d.%d.%d\n\r", ip4_addr1(ip), ip4_addr2(ip), ip4_addr3(ip), ip4_addr4(ip));
    xil_printf("Remote Port: %d\n\r", port);

    status = tcp_connect(pcb, ip, port, chat_connect_callback);
    if (status != ERR_OK) {
        xil_printf("Error: Code %d\n\r", status);
    }
    else {
        xil_printf("Connection request sent...\n\r");
    }
}

void client_loop()
{
    // Stub
}

static err_t chat_connect_callback(void *arg, tcp_pcb *pcb, err_t err)
{
    state = STATE_CALL;
    xil_printf("Connection request accepted: You are now in the call state\n\r");
    return ERR_OK;
}
