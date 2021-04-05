#include "xil_cache_l.h"

#include "platform.h"
#include "vga.h"
#include "serv.h"
#include "shell.h"
#include "client.h"
#include "state.h"
#include "chat.h"
#include "audio.h"


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
	client_init();

	//Configure the IIC data structure
	IicConfig(XPAR_XIICPS_0_DEVICE_ID);

	//Configure the Audio Codec's PLL
	AudioPllConfig();

	//Configure the Line in and Line out ports.
	//Call LineInLineOutConfig() for a configuration that
	//enables the HP jack too.
	AudioConfigureJacks();
	chat_audio_init();
}

void tick()
{
	vga_loop();


	if (state == STATE_MENU || state == STATE_MENU_CONNECT || state == STATE_MENU_LISTEN) {
		shell_loop();
		serv_loop();
	}
	else if (state == STATE_REQUEST || state == STATE_CALL_CLIENT) {
		client_loop();
	}
	else if (state == STATE_ACCEPT || state == STATE_CALL_SERVER) {
		serv_loop();
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
