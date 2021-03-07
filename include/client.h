#pragma once

#include "lwip/tcp.h"


void client_init();
void client_connect(const ip_addr_t *ip, u16 port);
void client_loop();
