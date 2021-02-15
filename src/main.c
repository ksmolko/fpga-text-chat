#include "xil_cache_l.h"

#include "interrupts.h"
#include "vga.h"
#include "ethernet.h"


void init()
{
	// Disable Caches (prevents tearing in images)
	Xil_L1DCacheDisable();
	Xil_L2CacheDisable();

	interrupts_init();
	vga_init();
	ethernet_init();
}

void tick()
{
	interrupts_loop();
	vga_loop();
}

int main()
{
	init();
	while(1) {
		tick();
	}
	return 0;
}

