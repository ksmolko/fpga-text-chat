#pragma once

#include "lwip/tcp.h"

#define LOCAL_CHAT_PORT 25567
#define MSG_MAX_LEN 128
#define HEADER_SZ 4
#define KEY_LEN 4096
#define KEY_HEADER 0x00000001


typedef struct tcp_pcb tcp_pcb;
typedef struct pbuf pbuf;

void chat_loop(tcp_pcb *pcb);
err_t chat_rcv(tcp_pcb *pcb, pbuf *p, err_t err);
