#include <stdbool.h>
#include "xil_io.h"
#include "xparameters.h"
#include "xuartps.h"
#include "lwip/init.h"
#include "lwip/tcp.h"
#include "lwip/dhcp.h"
#include "lwip/timeouts.h"
#include "netif/xadapter.h"
#include "lfsr_16bits.h"

#include "platform.h"
#include "serv.h"
#include "state.h"
#include "vga.h"
#include "chat.h"

#define BUF_SIZE 1024

typedef struct tcp_pcb tcp_pcb;

static err_t echo_accept_callback(void *arg, tcp_pcb *pcb, err_t err);
static err_t echo_recv_callback(void *arg, tcp_pcb *pcb, struct pbuf *p, err_t err);
static err_t chat_accept_callback(void *arg, tcp_pcb *pcb, err_t err);
static err_t chat_recv_callback(void *arg, tcp_pcb *pcb, struct pbuf *p, err_t err);
static void chat_err_callback(void *arg, err_t err);
static void chat_send_key();

extern volatile int dhcp_timeout_counter;
extern int state;
extern char otp_key[KEY_LEN];
extern const char *key_header;
struct netif netif;
static tcp_pcb *serv_pcb;
static int status_x_offset = 0;
static int status_y_offset = 0;

// Can probably find a more fitting home for this function
void ethernet_init()
{
	ip_addr_t ipaddr, netmask, gw;
	unsigned char mac_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

	IP4_ADDR(&ipaddr, 192, 168, 0, 23);
	IP4_ADDR(&netmask, 255, 255, 255, 0);
	IP4_ADDR(&gw, 192, 168, 0, 21);

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

	if (serv_pcb != NULL) {
		xil_printf("Chat server already open. Replacing...\n\r");
		tcp_close(serv_pcb);
		tcp_recv(serv_pcb, NULL);
	}

	if (!pcb) {
		xil_printf("ERROR: In function %s: Out of memory for PCB\n\r", __FUNCTION__);
	}

	status = tcp_bind(pcb, IP_ADDR_ANY, port);
	if (status != ERR_OK) {
		xil_printf("ERROR: In function %s: Unable to bind to port %d\n\r", __FUNCTION__, port);
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
	char c;
	char port_str[6];
	err_t err;

	xemacif_input(&netif);

	if (state == STATE_CALL_SERVER) {
		chat_loop(serv_pcb);
	}
	else if (state == STATE_ACCEPT) {
		// Accept or decline request from UART
		if (XUartPs_IsReceiveData(XPAR_PS7_UART_1_BASEADDR)) {
			c = XUartPs_ReadReg(XPAR_PS7_UART_1_BASEADDR, XUARTPS_FIFO_OFFSET);
			xil_printf("%c\n\r", c);
			// Accept the request logic
			// Cursor logic can replace c == 'y'
			if (c == 'y' || c == 'Y'){
				state = STATE_CALL_SERVER;
				xil_printf("Chat has begun\n\n\r");
				vga_switch_to_CHAT();
				vga_print_string(status_x_offset, status_y_offset, ipaddr_ntoa((ip_addr_t *)&(serv_pcb->remote_ip)));
				status_y_offset += ALPHABET_CHAR_LENGTH;
				sprintf(port_str, "%d", serv_pcb->remote_port);
				vga_print_string(status_x_offset, status_y_offset, port_str);
				err = tcp_write(serv_pcb, (void *)&c, 1, TCP_WRITE_FLAG_COPY);
				if (err != ERR_OK) {
					xil_printf("ERROR: tcp_write() error: Code %d\n\r", err);
				}
				err = tcp_output(serv_pcb);
				if (err != ERR_OK) {
					xil_printf("ERROR: tcp_output() error: Code %d\n\r", err);
				}

				// Send encryption key
				chat_send_key();
			}
			// Decline the request logic
			else if (c == 'n' || c == 'N') {
				state = STATE_MENU;
				xil_printf("Refusing connection. Returning to menu\n\r");
				vga_switch_to_IP();
				tcp_write(serv_pcb, (void *)&c, 1, TCP_WRITE_FLAG_COPY);
				tcp_output(serv_pcb);
				tcp_close(serv_pcb);
				tcp_recv(serv_pcb, NULL);
			}
			else {
				xil_printf("Invalid selection, try again: ");
			}
		}

		// Accept or decline request from push buttons
		// Check platform.c
	}

}

static err_t echo_accept_callback(void *arg, tcp_pcb *pcb, err_t err)
{
	LWIP_UNUSED_ARG(err);

	tcp_recv(pcb, echo_recv_callback);

	return ERR_OK;
}

static err_t echo_recv_callback(void *arg, tcp_pcb *pcb, pbuf *p, err_t err)
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

	serv_pcb = pcb;

	xil_printf("Incoming Connection Request. Accept? [y/n]: ");
	vga_print_string(HORIZONTAL_PIXEL_MAX/4
			, VERTICAL_PIXEL_MAX/2
			, "Incoming Connection Request. Accept? [y/n]");
	tcp_recv(pcb, chat_recv_callback);

	if (err != ERR_OK) {
		xil_printf("ERROR: Cannot accept connection: Code %d\n\r", err);
		return err;
	}
	else {
		state = STATE_ACCEPT;
	}

	return ERR_OK;
}

static err_t chat_recv_callback(void *arg, tcp_pcb *pcb, pbuf *p, err_t err)
{
	LWIP_UNUSED_ARG(arg);

	int status;

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

	if (state == STATE_CALL_SERVER) {
		status = chat_rcv(pcb, p, err);
	}

	pbuf_free(p);
	return status;
}

static void chat_err_callback(void *arg, err_t err)
{
	tcp_close(serv_pcb);
	tcp_recv(serv_pcb, NULL);
	xil_printf("ERROR: TCP server error: Code %d\n\rReturning to menu\n\r", *(int *)arg);
	state = STATE_MENU;
}

// Generates and sends a new OTP encryption key to the client, valid for 4096 characters
static void chat_send_key()
{
	err_t err;
	char key[KEY_LEN + OFFSET_SZ];

	memcpy(key, key_header, OFFSET_SZ);

	for (int i = OFFSET_SZ; i < KEY_LEN + OFFSET_SZ; i++) {
		key[i] = (char)LFSR_16BITS_mReadReg(XPAR_LFSR_16BITS_0_S00_AXI_BASEADDR, LFSR_16BITS_S00_AXI_SLV_REG0_OFFSET);
	}

	strcpy(otp_key, key + OFFSET_SZ);

	err = tcp_write(serv_pcb, (void *)key, KEY_LEN + OFFSET_SZ, TCP_WRITE_FLAG_COPY);
	if (err != ERR_OK) {
		xil_printf("ERROR: tcp_write() error: Code %d\n\r", err);
	}
	err = tcp_output(serv_pcb);
	if (err != ERR_OK) {
		xil_printf("ERROR: tcp_output() error: Code %d\n\r", err);
	}
}
