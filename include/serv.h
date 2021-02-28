#pragma once

#define ECHO_SERV 0
#define CHAT_SERV 1
#define ECHO_PORT 7
#define CHAT_PORT 30000


void ethernet_init();
void serv_init();
void serv_loop();
