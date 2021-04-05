#pragma once

#include "lwip/tcp.h"

#define LOCAL_CHAT_PORT 25567
#define MSG_MAX_LEN 128
#define OFFSET_SZ 4
#define MSG_TYPE_OFFSET 0
#define KEY_POS_OFFSET OFFSET_SZ
#define MSG_OFFSET OFFSET_SZ //* 2
#define KEY_LEN 512


typedef struct tcp_pcb tcp_pcb;
typedef struct pbuf pbuf;

void chat_loop(tcp_pcb *pcb);
err_t chat_rcv(tcp_pcb *pcb, pbuf *p, err_t err);
