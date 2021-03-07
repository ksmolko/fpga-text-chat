#pragma once

#include "lwip/tcp.h"

#define LOCAL_CHAT_PORT 25567
#define MSG_MAX_LEN 1024


typedef struct tcp_pcb tcp_pcb;
typedef struct pbuf pbuf;

void chat_loop(tcp_pcb *pcb);
err_t chat_rcv(tcp_pcb *pcb, pbuf *p, err_t err);
