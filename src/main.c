#include "xil_cache_l.h"

#include "platform.h"
#include "vga.h"
#include "serv.h"
#include "menu.h"


void init()
{
	// Disable Caches (prevents tearing in images)
	Xil_L1DCacheDisable();
	Xil_L2CacheDisable();

	timer_init();
	interrupts_init();
	vga_init();
	menu_init();
	ethernet_init();
	echoserv_init();
}

void tick()
{
	vga_loop();
	menu_loop();
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
