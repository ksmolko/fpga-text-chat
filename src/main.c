#include "xil_cache_l.h"

#include "platform.h"
#include "vga.h"
#include "serv.h"
#include "shell.h"


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
	echoserv_init();
}

void tick()
{
	vga_loop();
	shell_loop();
	echoserv_loop();
}

int main()
{
	init();
	while(1) {
		tick();
	}
	return 0;
}
