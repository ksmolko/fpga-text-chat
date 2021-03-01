#pragma once

#define ECHO_SERV 0
#define CHAT_SERV 1


void ethernet_init();
void serv_init(int serv_type, u16 port);
void serv_loop();
