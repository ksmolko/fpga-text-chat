#pragma once

#define HORIZONTAL_PIXEL_MAX 1920
#define VERTICAL_PIXEL_MAX 1080
#define ALPHABET_CHAR_LENGTH 32

void vga_init();
void vga_change();
void vga_loop();
void vga_print_character(int x_offset, int y_offset, char c);
void vga_print_string(int x_offset, int y_offset, char *str);
void vga_clear_chat_buf();

void vga_switch_to_IP();
void vga_switch_to_KB();
void vga_switch_to_CHAT();
void vga_switch_to_connect();
void vga_switch_to_listen();

void kb_move_cursor(int currentID, int nextID);
