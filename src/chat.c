#include <stdbool.h>
#include "xparameters.h"
#include "xuartps.h"
#include "netif/xadapter.h"
#include "otp_crypt.h"
#include "state.h"
#include "vga.h"
#include "chat.h"
#include "keyboard_logic.h"

#define CMD_CLOSE "/close"
#define RECEIVED_HEADER "Other: "
#define RECEIVED_HEADER_LEN 8


extern struct netif netif;
extern int state;
extern keyboard_state kb_state;		// from keyboard_logic.c

static int chat_box_x_offset = 0;
static int chat_box_y_offset = VERTICAL_PIXEL_MAX/10;
static int chat_buf_x_offset = 0;
static int chat_buf_y_offset = VERTICAL_PIXEL_MAX*4/5 - ALPHABET_CHAR_LENGTH;

char otp_key[KEY_LEN] = { 0 };
//int otp_key_pos = 0;
const char *key_header = "0001";
const char *msg_header = "0002";

static void crypt(char* msg, char* key, char* out_msg);

void chat_loop(tcp_pcb *pcb)
{
	static char buf[MSG_MAX_LEN] = "";
	char sndbuf[MSG_MAX_LEN + MSG_OFFSET] = "";
	char enc_msg[MSG_MAX_LEN];
	err_t err;
	char c;

	if (XUartPs_IsReceiveData(XPAR_PS7_UART_1_BASEADDR)) {
		c = XUartPs_ReadReg(XPAR_PS7_UART_1_BASEADDR, XUARTPS_FIFO_OFFSET);
		// This can be the enter function of the keyboard
		// e.g. if enter is pressed
		if (c == '\r' || c == '\n') {
			xil_printf("\n\r");

			if (strncmp(buf, CMD_CLOSE, strlen(CMD_CLOSE)) == 0) {
				tcp_write(pcb, (void *)CMD_CLOSE, strlen(CMD_CLOSE), TCP_WRITE_FLAG_COPY);
				tcp_output(pcb);
				state = STATE_MENU;
			}
			else {
				// Encrypt message
				crypt(buf, otp_key /*+ otp_key_pos*/, enc_msg);
				xil_printf("Encrypted message: %s\n\r", enc_msg);

				// Prepend message header to message
				memcpy(sndbuf, msg_header, OFFSET_SZ);
				//memcpy(sndbuf + KEY_POS_OFFSET, (void *)&otp_key_pos, OFFSET_SZ);
				memcpy(sndbuf + MSG_OFFSET, (void *)enc_msg, strlen(enc_msg));

				// Shift key
				//otp_key_pos += strlen(buf);

				// Write to packet and send message
				err = tcp_write(pcb, (void *)sndbuf, strlen(sndbuf) + 1, TCP_WRITE_FLAG_COPY);
				if (err != ERR_OK) {
					xil_printf("ERROR: tcp_write() error: Code %d\n\r", err);
				}
				err = tcp_output(pcb);
				if (err != ERR_OK) {
					xil_printf("ERROR: tcp_output() error: Code %d\n\r", err);
				}
			}

			// Print to VGA
			vga_print_string(chat_box_x_offset, chat_box_y_offset, buf);
			buf[0] = '\0';
			sndbuf[0] = '\0';
			vga_clear_chat_buf();
			chat_box_y_offset += ALPHABET_CHAR_LENGTH;
			chat_buf_y_offset = VERTICAL_PIXEL_MAX*4/5 - ALPHABET_CHAR_LENGTH;
			chat_buf_x_offset = 0;
			if (chat_box_y_offset + ALPHABET_CHAR_LENGTH >= VERTICAL_PIXEL_MAX*4/5 - ALPHABET_CHAR_LENGTH) {
				// TODO: Shift messages up
				// Clear the entire message window and start again
			}
		}
		else {
			// Every time a char is pressed
			if(strlen(buf) < MSG_MAX_LEN - 1) {
				xil_printf("%c", c);
				// Printing the single char on vga
				vga_print_character(chat_buf_x_offset, chat_buf_y_offset, c);
				chat_buf_x_offset += ALPHABET_CHAR_LENGTH;
				// vga printing spacing
				if (chat_buf_x_offset - ALPHABET_CHAR_LENGTH >= HORIZONTAL_PIXEL_MAX) {
					chat_buf_x_offset = 0;
					chat_buf_y_offset += ALPHABET_CHAR_LENGTH;
				}
				// concat the char to the end of the buffer
				strncat(buf, &c, 1);
			}
		}
	}

	// If statement for on screen kb
	if (kb_state.chat_kb_enter_pressed == 1) {
		// Get the message from keyboard's buffer
		memcpy(&buf, (void*)BUFFER_BASE_ADDR, kb_state.buffer_offset);

		// Resetting onscreen keyboard data
		Keyboard_reset();
		kb_state.chat_kb_enter_pressed = 0;

		// Encrypt message
		crypt(buf, otp_key /*+ otp_key_pos*/, enc_msg);
		xil_printf("Encrypted message: %s\n\r", enc_msg);

		// Prepend message header to message
		memcpy(sndbuf, msg_header, OFFSET_SZ);
		//memcpy(sndbuf + KEY_POS_OFFSET, (void *)&otp_key_pos, OFFSET_SZ);
		memcpy(sndbuf + MSG_OFFSET, (void *)enc_msg, strlen(enc_msg));

		// Shift key
		//otp_key_pos += strlen(buf);

		// Write to packet and send message
		err = tcp_write(pcb, (void *)sndbuf, strlen(sndbuf) + 1, TCP_WRITE_FLAG_COPY);
		if (err != ERR_OK) {
			xil_printf("ERROR: tcp_write() error: Code %d\n\r", err);
		}
		err = tcp_output(pcb);
		if (err != ERR_OK) {
			xil_printf("ERROR: tcp_output() error: Code %d\n\r", err);
		}

		// Print to VGA
		vga_print_string(chat_box_x_offset, chat_box_y_offset, buf);
		buf[0] = '\0';
		sndbuf[0] = '\0';
		vga_clear_chat_buf();
		chat_box_y_offset += ALPHABET_CHAR_LENGTH;
		// Resetting the offset location
		chat_buf_y_offset = VERTICAL_PIXEL_MAX*4/5 - ALPHABET_CHAR_LENGTH;
		chat_buf_x_offset = 0;
		if (chat_box_y_offset + ALPHABET_CHAR_LENGTH >= VERTICAL_PIXEL_MAX*4/5 - ALPHABET_CHAR_LENGTH) {
			// TODO: Shift messages up
			// Clear the entire message window and start again
		}
	}
}

err_t chat_rcv(tcp_pcb *pcb, pbuf *p, err_t err)
{
	//static bool first = true;
	char *msg;
	char dec_msg[MSG_MAX_LEN];
	char vga_printout[MSG_MAX_LEN + RECEIVED_HEADER_LEN] = "Other: ";

	/*if (state == STATE_CALL_SERVER) {
		first = false;
	}*/

	if (err == ERR_OK && p != NULL) {
		//msg = (char *)(p->payload);
		//tcp_recved(pcb, p->len);

		msg = malloc(p->tot_len);
		pbuf_copy_partial(p, msg, p->tot_len, 0);
		
		if (strncmp(msg, CMD_CLOSE, strlen(CMD_CLOSE)) == 0) {
			tcp_close(pcb);
			tcp_recv(pcb, NULL);
			pbuf_free(p);
			xil_printf("\n\rConnection closed by other user\n\rReturning to menu\n\r");
			vga_switch_to_IP();
			state = STATE_MENU;
			return ERR_OK;
		}
		else if (memcmp((void *)msg, (void *)key_header, OFFSET_SZ) == 0) {
			memcpy(otp_key, msg + OFFSET_SZ, KEY_LEN);
		}
		else {
			/*if (first) {
				first = false;
			}*/
			//else {
			if(memcmp((void *)msg, (void *)msg_header, OFFSET_SZ) == 0) {
				// Synchronize position in key
				//otp_key_pos = *(int *)(msg + KEY_POS_OFFSET);

				// Decrypt message
				xil_printf("Received encrypted message: %s\n\r", msg + MSG_OFFSET);
				crypt(msg + MSG_OFFSET, otp_key /*+ otp_key_pos*/, dec_msg);

				xil_printf("\n\r%s%s\n\r", RECEIVED_HEADER, dec_msg);

				// Print message to VGA
				strcat(vga_printout, dec_msg);
				vga_print_string(chat_buf_x_offset, chat_box_y_offset, vga_printout);
				chat_box_y_offset += ALPHABET_CHAR_LENGTH;
			}
			else {
				xil_printf("Not a message: Header %X\n\r", *(int *)msg);
			}
			//}
		}
		tcp_recved(pcb, p->tot_len);
		free(msg);
	}

	return ERR_OK;
}

// Encrypt or Decrypt message. Process is the same for both.
static void crypt(char* msg, char* key, char* out_msg)
{
	u32_t output[OTP_CRYPT_NUM_REGS / 3];

	// Clear registers
	for (int i = 0; i < OTP_CRYPT_NUM_REGS * 2 / 3; i++) {
		OTP_CRYPT_mWriteReg(XPAR_OTP_CRYPT_0_S00_AXI_BASEADDR, OTP_CRYPT_REG_SIZE*i, 0);
	}

	// Write key to input
	for (int i = 0; i < OTP_CRYPT_NUM_REGS / 3; i++) {
		OTP_CRYPT_mWriteReg(XPAR_OTP_CRYPT_0_S00_AXI_BASEADDR, OTP_CRYPT_REG_SIZE*i, *((int *)(key + OTP_CRYPT_REG_SIZE*i)));
	}

	// Write message to input registers
	for (int i = OTP_CRYPT_NUM_REGS / 3; i < OTP_CRYPT_NUM_REGS * 2 / 3; i++) {
		OTP_CRYPT_mWriteReg(XPAR_OTP_CRYPT_0_S00_AXI_BASEADDR, OTP_CRYPT_REG_SIZE*i, *((int *)(msg + OTP_CRYPT_REG_SIZE*(i - OTP_CRYPT_NUM_REGS / 3))));
	}

	// Read output message from output registers
	for (int i = OTP_CRYPT_NUM_REGS * 2 / 3; i < OTP_CRYPT_NUM_REGS; i++) {
		output[i - OTP_CRYPT_NUM_REGS * 2 / 3] = OTP_CRYPT_mReadReg(XPAR_OTP_CRYPT_0_S00_AXI_BASEADDR, OTP_CRYPT_REG_SIZE*i);
	}

	memcpy(out_msg, (char *)output, OTP_CRYPT_NUM_REGS / 3 * OTP_CRYPT_REG_SIZE);
	out_msg[strlen(msg)] = '\0';
}
