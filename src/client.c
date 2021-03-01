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
	}

    status = tcp_bind(pcb, IPADDR_TYPE_V4, port);
	if (status != ERR_OK) {
		xil_printf("ERROR: In function %s: Unable to bind to port %d\n\r", __FUNCTION__, port);
	}
	else {
		xil_printf("Client bind Successful on port %d\n\r", port);
	}

    tcp_connect(pcb, ip, port, chat_connect_callback);
}

void client_loop()
{
    // Stub
}

static err_t chat_connect_callback(void *arg, tcp_pcb *pcb, err_t err)
{
    state = STATE_CALL;
    xil_printf("You are now in the call state\n\r");
    return ERR_OK;
}
