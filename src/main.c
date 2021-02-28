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
	serv_init(CHAT_SERV, CHAT_PORT);
}

void tick()
{
	vga_loop();
	shell_loop();
	serv_loop();
}

int main()
{
	init();
	while(1) {
		tick();
	}
	return 0;
}
