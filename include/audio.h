#ifndef __AUDIO_H_
#define __AUDIO_H_

#include "lwipopts.h"

#define AUDIO_MEMORY_BASE			0x07000000
#define AUDIO_MEMORY_UNIT_OFFSET	8		// 8 bytes
#define AUDIO_LENGTH				10
#define AUDIO_SAMPLES_PER_SEC		48000	// Sampling at 48000 kHz
#define AUDIO_MEMORY_MAX_OFFSET		AUDIO_SAMPLES_PER_SEC*AUDIO_LENGTH*AUDIO_MEMORY_UNIT_OFFSET

#define AUDIO_PACKET_HEADER_SIZE		8
#define AUDIO_PACKET_HEADER				"audio"
#define AUDIO_PACKET_MAX_SIZE			65536 - AUDIO_PACKET_HEADER_SIZE

#define AUDIO_PACKET_SEND_BUFFER		AUDIO_MEMORY_BASE+AUDIO_MEMORY_MAX_OFFSET
#define AUDIO_PACKET_SEND_BUFFER_DATA	AUDIO_PACKET_SEND_BUFFER + AUDIO_PACKET_HEADER_SIZE

#define AUDIO_PACKET_RECV_BASE			AUDIO_PACKET_SEND_BUFFER+AUDIO_PACKET_MAX_SIZE+2000

/* Prototype Functions */
unsigned char IicConfig(unsigned int DeviceIdPS);
void AudioPllConfig();
void AudioWriteToReg(unsigned char u8RegAddr, unsigned char u8Data);
void AudioConfigureJacks();
void LineinLineoutConfig();
void audio_record();
void audio_playback();
void audio_stream();
void chat_audio_record();
void chat_audio_init();
void chat_audio_playback_recv();

#endif
