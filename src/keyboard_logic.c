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

#define BUFFER_BASE_ADDR 0x06000000
#define BUFFER_OFFSET_MAX 100

typedef struct keyboard_state_s keyboard_state;
struct keyboard_state_s {
	int current_x;
	int current_y;
	int is_caplock_on;
	char buffer_offset;	// Since we only want to increment the offset by 1 byte
};

static keyboard_state kb_state;


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
}

static int calculateAddress() {
	return kb_state.current_y*(KEYBOARD_X_MAX + 1) + kb_state.current_x;
}

// From the address get the character

void Press_up() {
	xil_printf("Pressing keyboard up\n\r");
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
	xil_printf("Current_x: %d Current_y: %d\n\r", kb_state.current_x, kb_state.current_y);
}

void Press_down() {
	xil_printf("Pressing keyboard down\n\r");
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
	xil_printf("Current_x: %d Current_y: %d\n\r", kb_state.current_x, kb_state.current_y);
}

void Press_left() {
	xil_printf("Pressing keyboard left\n\r");
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
	xil_printf("Current_x: %d Current_y: %d\n\r", kb_state.current_x, kb_state.current_y);
}

void Press_right() {
	xil_printf("Pressing keyboard right\n\r");
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
	xil_printf("Current_x: %d Current_y: %d\n\r", kb_state.current_x, kb_state.current_y);
}

void Press_center() {
	xil_printf("Pressing keyboard center\n\r");
	// Get current address
	int currentAddress = calculateAddress();
	xil_printf("Current address: %d\n\r", currentAddress);
	char character;
	switch (currentAddress) {
	case FUNCTION_BACKSPACE:
		// Backspace implementation
		// Decrease the buffer offset by 1
		xil_printf("Backspace stub\n\r");
		kb_state.buffer_offset = kb_state.buffer_offset - 1;
		break;
	case FUNCTION_CAPLOCK:
		// Caplock implementation
		xil_printf("Caplock stub\n\r");
		// TODO: think of a better way with XOR to toggle
		if (kb_state.is_caplock_on == 0) {
			xil_printf("Toggle caplock on\n\r");
			kb_state.is_caplock_on = 1;
		} else {
			xil_printf("Toggle caplock off\n\r");
			kb_state.is_caplock_on = 0;
		}

		break;
	case FUNCTION_ENTER:
		// Enter implementation
		// Read the characters in the buffer from offset 0 to the current buffer offset
		// Then reset the buffer offset

		// Currently, we're just outputting the buffer through UART
		xil_printf("Enter stub\n\r");
		for (char i = 0; i < kb_state.buffer_offset; ++i) {
			memcpy(&character, (void*)BUFFER_BASE_ADDR + i, 1);
			xil_printf("%c", character);
		}
		xil_printf("\n\r");
		kb_state.buffer_offset = 0;
		break;
	case FUNCTION_RETURN:
		// Return implementation
		// Reset the buffer offset
		xil_printf("Return stub\n\r");
		kb_state.buffer_offset = 0;
		break;
	default:
		if (kb_state.is_caplock_on == 0) {
			character = character_map[currentAddress];
		} else {
			character = character_map_cap[currentAddress];
		}

		memcpy((void*)BUFFER_BASE_ADDR + kb_state.buffer_offset, &character, 1);
		kb_state.buffer_offset = kb_state.buffer_offset + 1;
		xil_printf("%c\n\r", character);
		break;
	}

}

