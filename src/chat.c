#include <stdbool.h>
#include "xparameters.h"
#include "xuartps.h"
#include "netif/xadapter.h"
#include "state.h"
#include "vga.h"
#include "chat.h"
#include "audio.h"

#define CMD_CLOSE "/close"
#define CMD_RECORD "/record"
#define CMD_SEND_AUDIO "/sendaudio"
#define RECEIVED_HEADER "Other: "
#define RECEIVED_HEADER_LEN 8
#define AUDIO_HEADER "audio"

extern struct netif netif;
extern int state;

static int chat_box_x_offset = 0;
static int chat_box_y_offset = VERTICAL_PIXEL_MAX/10;
static int chat_buf_x_offset = 0;
static int chat_buf_y_offset = VERTICAL_PIXEL_MAX*4/5 - ALPHABET_CHAR_LENGTH;

int isRecording = NOT_RECORDING;

int audio_recved_bytes_offset = 0;

void chat_loop(tcp_pcb *pcb)
{
	static char buf[MSG_MAX_LEN] = "";
	err_t err;
	char c;

	if (XUartPs_IsReceiveData(XPAR_PS7_UART_1_BASEADDR)) {
		c = XUartPs_ReadReg(XPAR_PS7_UART_1_BASEADDR, XUARTPS_FIFO_OFFSET);
		if (c == '\r' || c == '\n') {
			if (strncmp(buf, CMD_RECORD, strlen(CMD_RECORD)) == 0) {
				isRecording = RECORDING;
				vga_print_string(chat_box_x_offset, chat_box_y_offset, buf);
				buf[0] = '\0';
				vga_clear_chat_buf();
				return;
			}

			if (strncmp(buf, CMD_SEND_AUDIO, strlen(CMD_SEND_AUDIO)) == 0) {
				if (isRecording == FINISH_RECORDING) {
					// Only send audio data if have finish recording
					int offset = 0;
					//
					while (offset < AUDIO_MEMORY_MAX_OFFSET) {
						// TODO: encode the head of the packet to notify it contains audio data
						memcpy((int *)AUDIO_PACKET_SEND_BUFFER_DATA, (int *)(AUDIO_MEMORY_BASE + offset), AUDIO_PACKET_MAX_SIZE);
						// Sending maximum packet capacity
						err = tcp_write(pcb, (void *)AUDIO_PACKET_SEND_BUFFER, sizeof(u16_t), TCP_WRITE_FLAG_COPY);
						if (err != ERR_OK) {
							xil_printf("ERROR: tcp_write() error: Code %d\n\r", err);
						}
						err = tcp_output(pcb);
						if (err != ERR_OK) {
							xil_printf("ERROR: tcp_output() error: Code %d\n\r", err);
						}

						offset += AUDIO_PACKET_MAX_SIZE;
					}
					// After sending all audio data
					isRecording = NOT_RECORDING;
				}

				vga_print_string(chat_box_x_offset, chat_box_y_offset, buf);
				buf[0] = '\0';
				vga_clear_chat_buf();
				return;
			}

			// TODO: need to use the buffer from keyboard logic
			// Because sending audio is a different function from sending message
			// Might need to make a separate function for sending audio
			xil_printf("\n\r");
			err = tcp_write(pcb, (void *)buf, strlen(buf) + 1, TCP_WRITE_FLAG_COPY);
			if (err != ERR_OK) {
				xil_printf("ERROR: tcp_write() error: Code %d\n\r", err);
			}
			err = tcp_output(pcb);
			if (err != ERR_OK) {
				xil_printf("ERROR: tcp_output() error: Code %d\n\r", err);
			}
			vga_print_string(chat_box_x_offset, chat_box_y_offset, buf);
			buf[0] = '\0';
			vga_clear_chat_buf();
			chat_box_y_offset += ALPHABET_CHAR_LENGTH;
			chat_buf_y_offset = VERTICAL_PIXEL_MAX*4/5 - ALPHABET_CHAR_LENGTH;
			chat_buf_x_offset = 0;
			if (chat_box_y_offset + ALPHABET_CHAR_LENGTH >= VERTICAL_PIXEL_MAX*4/5 - ALPHABET_CHAR_LENGTH) {
				// TODO: Shift messages up
			}
		}
		else {
			if(strlen(buf) < MSG_MAX_LEN - 1) {
				xil_printf("%c", c);
				vga_print_character(chat_buf_x_offset, chat_buf_y_offset, c);
				chat_buf_x_offset += ALPHABET_CHAR_LENGTH;
				if (chat_buf_x_offset - ALPHABET_CHAR_LENGTH >= HORIZONTAL_PIXEL_MAX) {
					chat_buf_x_offset = 0;
					chat_buf_y_offset += ALPHABET_CHAR_LENGTH;
				}
				strncat(buf, &c, 1);
			}
		}
	}
}

err_t chat_rcv(tcp_pcb *pcb, pbuf *p, err_t err)
{
	static bool first = true;
	char *msg;
	char vga_printout[MSG_MAX_LEN + RECEIVED_HEADER_LEN] = "Other: ";
	u16_t bytes_received;
	if (state == STATE_CALL_SERVER) {
		first = false;
	}
	// Have to know whether it's receiving a message or audio data
	// Might want to get rid of the while loop to implement receiving audio data
//	if (err == ERR_OK && p != NULL) {
//		do {
//			msg = (char *)(p->payload);
//			tcp_recved(pcb, p->len);
//
//			if (strncmp(msg, CMD_CLOSE, strlen(CMD_CLOSE)) == 0) {
//				// if received a close message
//				// Closing TCP port
//				tcp_close(pcb);
//				tcp_recv(pcb, NULL);
//				pbuf_free(p);
//				xil_printf("\n\rConnection closed by other user\n\rReturning to menu\n\r");
//				vga_switch_to_IP();
//				state = STATE_MENU;
//				return ERR_OK;
//			}
//			else if (strncmp(msg, CMD_AUDIO_HEADER, strlen(CMD_AUDIO_HEADER)) == 0) {
//				// received an audio header
//				// expecting to get all audio data before listening for other message
//				// Might need to ask Kyle how his loop works
//
//			}
//			else {
//				if (first) {
//					first = false;
//				}
//				else {
//					// Print out received message here
//					xil_printf("\n\r%s%s\n\r", RECEIVED_HEADER, msg);
//					strcat(vga_printout, msg);
//					vga_print_string(chat_buf_x_offset, chat_box_y_offset, vga_printout);
//					chat_box_y_offset += ALPHABET_CHAR_LENGTH;
//				}
//			}
//		} while (p->next != NULL);
//	}

	if (err == ERR_OK && p != NULL) {
		msg = malloc(p->tot_len);
		bytes_received = pbuf_copy_partial(p, (char *)msg, p->tot_len, 0);
		xil_printf("Bytes Received: %lu\n\r", bytes_received);

		// Create a buffer for checking packet header
		char * packet_header = malloc(AUDIO_PACKET_HEADER_SIZE);
		memcpy(packet_header, msg, AUDIO_PACKET_HEADER_SIZE);

		if (strncmp(packet_header, AUDIO_PACKET_HEADER, strlen(AUDIO_PACKET_HEADER)) == 0) {
			// Header of an audio packet
			// Right now, I'm assuming that the audio packets will be sent in order
			xil_printf("Receiving audio data\n\r");
			memcpy((void *)AUDIO_PACKET_RECV_BASE, msg + AUDIO_PACKET_HEADER_SIZE, AUDIO_PACKET_MAX_SIZE);
			audio_recved_bytes_offset += AUDIO_PACKET_MAX_SIZE;
			if (audio_recved_bytes_offset >= AUDIO_MEMORY_MAX_OFFSET) {
				xil_printf("Finish receiving audio data\n\r");
			}
		}
		if (strncmp(msg, CMD_CLOSE, strlen(CMD_CLOSE)) == 0) {
			// if received a close message
			// Closing TCP port
			tcp_close(pcb);
			tcp_recv(pcb, NULL);
			pbuf_free(p);
			xil_printf("\n\rConnection closed by other user\n\rReturning to menu\n\r");
			vga_switch_to_IP();
			state = STATE_MENU;
			return ERR_OK;
		}
		else {
			if (first) {
				first = false;
			}
			else {
				// Print out received message here
				xil_printf("\n\r%s%s\n\r", RECEIVED_HEADER, msg);
				strcat(vga_printout, msg);
				vga_print_string(chat_buf_x_offset, chat_box_y_offset, vga_printout);
				chat_box_y_offset += ALPHABET_CHAR_LENGTH;
			}
		}
		// Sending ACK back to the receiver
		tcp_recved(pcb, p->tot_len);
		msg = NULL;
		free(msg);
		free(packet_header);
	}
	return ERR_OK;
}
