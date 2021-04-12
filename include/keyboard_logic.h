#pragma once
#define BUFFER_BASE_ADDR 0x06000000
#define BUFFER_OFFSET_MAX 128 - 1

void Keyboard_init();
void Press_up();
void Press_down();
void Press_left();
void Press_right();
void Press_center();
void Keyboard_reset();


typedef struct keyboard_state_s keyboard_state;
struct keyboard_state_s {
	int current_x;
	int current_y;
	int is_caplock_on;
	char buffer_offset;	// Since we only want to increment the offset by 1 byte
	int chat_kb_enter_pressed;
};
