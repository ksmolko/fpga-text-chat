#include <stdlib.h>
#include "keyboard_logic.h"
#include "xil_io.h"
#include "vga.h"

#define KEYBOARD_X_MIN 0
#define KEYBOARD_Y_MIN 0
#define KEYBOARD_X_MAX 11
#define KEYBOARD_Y_MAX 4

#define FUNCTION_BACKSPACE 11
#define FUNCTION_CAPLOCK 11+12
#define FUNCTION_ENTER 11+12*2
#define FUNCTION_RETURN 11+12*3


extern int state;	// From main.c
extern int is_in_kb;	// From platform.c

keyboard_state kb_state;
// The text box will start at the top left corner of the screen
static int chat_buf_x_offset = 0;
static int chat_buf_y_offset = 0;


/*
 * Keyboard character map
 * The '*' character is a special character stubbing for backspace, caplock, enter, and return
 * The keyboard logic will use the address to get the corresponding function when encounter
 * this special character
 *
 * The bottom of the keyboard will all be spacebar (space character)
 */
char character_map[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '*'\
						,'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '/', '*'\
						,'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '\'', '*'\
						,'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '?', '!', '*'\
						,' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};


char character_map_cap[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '*'\
						,'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '/', '*'\
						,'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\'', '*'\
						,'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '?', '!', '*'\
						,' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};

void Keyboard_init() {
	kb_state.current_x = 0;
	kb_state.current_y = 0;
	kb_state.is_caplock_on = 0;
	kb_state.buffer_offset = 0;
	kb_state.chat_kb_enter_pressed = 0;
	kb_state.ip_kb_enter_pressed = 0;
	vga_switch_to_KB();
}

void Keyboard_reset() {
	kb_state.current_x = 0;
	kb_state.current_y = 0;
	kb_state.is_caplock_on = 0;
	kb_state.buffer_offset = 0;
	chat_buf_x_offset = 0;
	chat_buf_y_offset = 0;
}

static int calculateAddress() {
	return kb_state.current_y*(KEYBOARD_X_MAX + 1) + kb_state.current_x;
}

// From the address get the character

void Press_up() {
	// If cursor is at the top, don't do anything
	if (kb_state.current_y != KEYBOARD_Y_MIN) {
		// Cursor is not at the top
		// Update the current state
		int currentAddr = calculateAddress();
		kb_state.current_y = kb_state.current_y - 1;
		int nextAddr = calculateAddress();

		// TODO: notify the vga.c to update the cursor
		kb_move_cursor(currentAddr, nextAddr);
	}
}

void Press_down() {
	// If cursor is at the top, don't do anything
	if (kb_state.current_y != KEYBOARD_Y_MAX) {
		// Cursor is not at the bottom
		// Update the current state
		int currentAddr = calculateAddress();
		kb_state.current_y = kb_state.current_y + 1;
		int nextAddr = calculateAddress();

		// TODO: notify the vga.c to update the cursor
		kb_move_cursor(currentAddr, nextAddr);
	}
}

void Press_left() {
	// If cursor is at the top, don't do anything
	if (kb_state.current_x != KEYBOARD_X_MIN) {
		// Cursor is not at the top
		// Update the current state
		int currentAddr = calculateAddress();
		kb_state.current_x = kb_state.current_x - 1;
		int nextAddr = calculateAddress();

		// TODO: notify the vga.c to update the cursor
		kb_move_cursor(currentAddr, nextAddr);
	}
}

void Press_right() {
	// If cursor is at the top, don't do anything
	if (kb_state.current_x != KEYBOARD_X_MAX) {
		// Cursor is not at the top
		// Update the current state
		int currentAddr = calculateAddress();
		kb_state.current_x = kb_state.current_x + 1;
		int nextAddr = calculateAddress();

		// TODO: notify the vga.c to update the cursor
		kb_move_cursor(currentAddr, nextAddr);
	}
}

void Press_center() {
	// Get current address
	int currentAddress = calculateAddress();
	xil_printf("Current address: %d\n\r", currentAddress);
	char character;
	switch (currentAddress) {
	case FUNCTION_BACKSPACE:
		// Backspace implementation
		// Decrease the buffer offset by 1
		kb_state.buffer_offset = kb_state.buffer_offset - 1;
		// Update the vga
		// Move the offset back by 1 char
		if (chat_buf_x_offset == 0 && chat_buf_y_offset > 0) {
			// First character on next line
			chat_buf_x_offset = HORIZONTAL_PIXEL_MAX - ALPHABET_CHAR_LENGTH;
			chat_buf_y_offset -= ALPHABET_CHAR_LENGTH;
		} else {
			// A character in a line
			chat_buf_x_offset -= ALPHABET_CHAR_LENGTH;
		}

		// Clear the last character on vga
		vga_print_character(chat_buf_x_offset, chat_buf_y_offset, '#');

		break;
	case FUNCTION_CAPLOCK:
		// Caplock implementation
		if (kb_state.is_caplock_on == 0) {
			kb_state.is_caplock_on = 1;
		} else {
			kb_state.is_caplock_on = 0;
		}

		break;
	case FUNCTION_ENTER:
		// Enter implementation
		// Read the characters in the buffer from offset 0 to the current buffer offset
		// Then reset the buffer offset

		// Reload the screen before the onscreen keyboard
		vga_switch_to_KB_previous();

		// Notify chat window to get the message in memory
		kb_state.chat_kb_enter_pressed = 1;		// set flag enter pressed so that chat.c can retrieve message from onscreen kb
		kb_state.ip_kb_enter_pressed = 1;
		is_in_kb = 0;
		break;
	case FUNCTION_RETURN:
		// Return implementation
		// Reset the buffer offset

		// If state is STATE_MENU, STATE_MENU_CONNECT, STATE_MENU_LISTEN
		// Go back to STATE_MENU

		// If state is STATE_REQUEST, STATE_CALL_CLIENT
		// Go back to STATE_CALL_CLIENT, STATE_CALL_SERVER
		vga_switch_to_KB_previous();

		Keyboard_reset();
		is_in_kb = 0;
		break;
	default:
		if (kb_state.is_caplock_on == 0) {
			character = character_map[currentAddress];
		} else {
			character = character_map_cap[currentAddress];
		}

		memcpy((void*)BUFFER_BASE_ADDR + kb_state.buffer_offset, &character, 1);
		// Display the character on vga here
		vga_print_character(chat_buf_x_offset, chat_buf_y_offset, character);

		// Move the offset to next location
		kb_state.buffer_offset = kb_state.buffer_offset + 1;
		chat_buf_x_offset += ALPHABET_CHAR_LENGTH;


		// Go to next line
		if (chat_buf_x_offset - ALPHABET_CHAR_LENGTH >= HORIZONTAL_PIXEL_MAX) {
			chat_buf_x_offset = 0;
			chat_buf_y_offset += ALPHABET_CHAR_LENGTH;
		}

		xil_printf("%c\n\r", character);
		break;
	}

}

