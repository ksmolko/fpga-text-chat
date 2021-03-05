#include "xil_cache_l.h"

#include "platform.h"
#include "vga.h"
#include "serv.h"
#include "shell.h"
#include "client.h"
#include "state.h"


int state = STATE_MENU;

void init()
{
	// Disable Caches (prevents tearing in images)
	Xil_L1DCacheDisable();
	Xil_L2CacheDisable();

	timer_init();
	interrupts_init();
	vga_init();
	shell_init();
	ethernet_init();
	serv_init(ECHO_SERV, ECHO_PORT);
}

void tick()
{
	vga_loop();

	if (state == STATE_MENU) {
		shell_loop();
		serv_loop();
	}
	else if (state == STATE_REQUEST) {
		client_loop();
	}
	else if (state == STATE_ACCEPT) {
		serv_loop();
	}
	else if (state == STATE_CALL_SERV) {
		serv_loop();
	}
	else if (state == STATE_CALL_CLIENT) {
		client_loop();
	}
}

int main()
{
	init();
	while(1){
		tick();
	}
	return 0;
}
