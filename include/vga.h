#ifndef VGA_H
#define VGA_H

void vga_init();
void vga_change();
void vga_loop();

void vga_switch_to_IP();
void vga_switch_to_KB();
void vga_switch_to_CHAT();

void kb_move_cursor(int currentID, int nextID);

#endif
