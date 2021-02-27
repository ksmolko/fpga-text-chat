#include <string.h>
#include "xil_io.h"
#include "vga.h"

#define IMG_BUF_PTR 0x00900000
#define NORMAL_COLOURS 0
#define ALTERNATE_COLOURS 1
#define HORIZONTAL_PIXEL_MAX 1920
#define VERTICAL_PIXEL_MAX 1080
#define NUM_BYTES_BUFFER HORIZONTAL_PIXEL_MAX*VERTICAL_PIXEL_MAX*4


static int * image_pointer[5] = {(int *)0x018D2008, (int *)0x020BB00C, (int *)0x028A4010, (int *)0x0308D014, (int *)0x03876018};
static int counter = 0;
void setColours(int colour, int* image_pointer[5])
{

	int red = 0xFF0000FF;
	int green = 0xFF00FF00;
	int blue = 0xFFFF0000;
	int yellow = 0xFF00FFFF;
	int purple = 0xFFFF00FF;

	if(colour == 1){
		red = 0xFFECFF00;
		green = 0xFFF000FF;
		blue = 0xFFFFFFFF;
		yellow = 0xFF0087FF;
		purple = 0xFF97FF00;
	}

	int addr_offset;

	// Assigning colors to each pixel
	for (int y = 0; y < 1024; y++) {
		for (int x = 0; x < 1280; x++) {
			addr_offset = 1280*y + x;

			// A simple chat UI: a yellow upper part for chat box
			// A green option bar below with red buttons
			// TODO: create a class that calculates the starting and ending point of a shape base on length, width, and center
			// TODO: also implement a way to render images with layers
			// E.g.: background layer, button layer, ...

			// Background layer
			if (y > VERTICAL_PIXEL_MAX*4/5) {
				// Option box
				memcpy(image_pointer[2] + addr_offset, &green, 4);
			} else if (y < VERTICAL_PIXEL_MAX/10) {
				// Current IP address you're chatting to
				memcpy(image_pointer[2] + addr_offset, &blue, 4);
			}
			else {
				// Chat history box
				memcpy(image_pointer[2] + addr_offset, &yellow, 4);
			}

			// Button layer
			int button1_xCenter = (HORIZONTAL_PIXEL_MAX/4 - 0)/2;
			int button2_xCenter = ((HORIZONTAL_PIXEL_MAX*2/4 - HORIZONTAL_PIXEL_MAX/4)/2) + HORIZONTAL_PIXEL_MAX/4;
			int button3_xCenter = ((HORIZONTAL_PIXEL_MAX*3/4 - HORIZONTAL_PIXEL_MAX*2/4)/2) + HORIZONTAL_PIXEL_MAX*2/4;
			int button4_xCenter = ((HORIZONTAL_PIXEL_MAX - HORIZONTAL_PIXEL_MAX*3/4)/2) + HORIZONTAL_PIXEL_MAX*3/4;

			int buttonLine_yCenter = ((VERTICAL_PIXEL_MAX - VERTICAL_PIXEL_MAX*4/5)/2) + VERTICAL_PIXEL_MAX*4/5;
			int button_xLength = 200;
			int button_yLength = 100;

			// Button 1
			if (x > (button1_xCenter-button_xLength/2) && x < (button1_xCenter+button_xLength/2) && y > (buttonLine_yCenter-button_yLength/2) && y < (buttonLine_yCenter+button_yLength/2)) {
				memcpy(image_pointer[2] + addr_offset, &red, 4);
			}

			// Button 2
			if (x > (button2_xCenter-button_xLength/2) && x < (button2_xCenter+button_xLength/2) && y > (buttonLine_yCenter-button_yLength/2) && y < (buttonLine_yCenter+button_yLength/2)) {
				memcpy(image_pointer[2] + addr_offset, &red, 4);
			}

			// Button 3
			if (x > (button3_xCenter-button_xLength/2) && x < (button3_xCenter+button_xLength/2) && y > (buttonLine_yCenter-button_yLength/2) && y < (buttonLine_yCenter+button_yLength/2)) {
				memcpy(image_pointer[2] + addr_offset, &red, 4);
			}

			// Button 4
			if (x > (button4_xCenter-button_xLength/2) && x < (button4_xCenter+button_xLength/2) && y > (buttonLine_yCenter-button_yLength/2) && y < (buttonLine_yCenter+button_yLength/2)) {
				memcpy(image_pointer[2] + addr_offset, &red, 4);
			}

			// Onscreen keyboard and composing message text box
			if (y > VERTICAL_PIXEL_MAX*2/5) {
				// Keyboard zone
				memcpy(image_pointer[1] + addr_offset, &green, 4);
			} else {
				// Your text message box
				memcpy(image_pointer[1] + addr_offset, &yellow, 4);
			}

			// Starting Menu
			// Background layer
			if (y > VERTICAL_PIXEL_MAX*4/5) {
				// Option box
				memcpy(image_pointer[0] + addr_offset, &green, 4);
			} else if (y < VERTICAL_PIXEL_MAX/10) {
				// Your IP address
				memcpy(image_pointer[0] + addr_offset, &blue, 4);
			}
			else {
				// Past IP addresses
				memcpy(image_pointer[0] + addr_offset, &yellow, 4);
			}

			// Past IP addresses list
			int list_top_boundary = VERTICAL_PIXEL_MAX/10;
			int list_bottom_boundary = VERTICAL_PIXEL_MAX*4/5;
			int num_of_elem = 4;
			int list_elem_height = (list_bottom_boundary - list_top_boundary)/num_of_elem;

			// Element 1
			if (y > list_top_boundary && y < list_top_boundary + list_elem_height) {
				memcpy(image_pointer[0] + addr_offset, &purple, 4);
			}
			// Element 2
			if (y > list_top_boundary + list_elem_height && y < list_top_boundary + 2*list_elem_height) {
				memcpy(image_pointer[0] + addr_offset, &yellow, 4);
			}
			// Element 3
			if (y > list_top_boundary + 2*list_elem_height && y < list_top_boundary + 3*list_elem_height) {
				memcpy(image_pointer[0] + addr_offset, &purple, 4);
			}

			// Element 4
			if (y > list_top_boundary + 3*list_elem_height && y < list_top_boundary + 4*list_elem_height) {
				memcpy(image_pointer[0] + addr_offset, &yellow, 4);
			}

			// Option button layer
			button1_xCenter = (HORIZONTAL_PIXEL_MAX/2 - 0)/2;
			button2_xCenter = ((HORIZONTAL_PIXEL_MAX - HORIZONTAL_PIXEL_MAX/2)/2) + HORIZONTAL_PIXEL_MAX/2;

			// Button 1
			if (x > (button1_xCenter-button_xLength/2) && x < (button1_xCenter+button_xLength/2) && y > (buttonLine_yCenter-button_yLength/2) && y < (buttonLine_yCenter+button_yLength/2)) {
				memcpy(image_pointer[0] + addr_offset, &red, 4);
			}

			// Button 2
			if (x > (button2_xCenter-button_xLength/2) && x < (button2_xCenter+button_xLength/2) && y > (buttonLine_yCenter-button_yLength/2) && y < (buttonLine_yCenter+button_yLength/2)) {
				memcpy(image_pointer[0] + addr_offset, &red, 4);
			}
		}
	}

}

void vga_init()
{
	memset(image_pointer[0], 0, NUM_BYTES_BUFFER);
	memset(image_pointer[1], 0, NUM_BYTES_BUFFER);
	memset(image_pointer[2], 0, NUM_BYTES_BUFFER);
	memset(image_pointer[3], 0, NUM_BYTES_BUFFER);
	memset(image_pointer[4], 0, NUM_BYTES_BUFFER);

	setColours(NORMAL_COLOURS, image_pointer);

	memcpy((void*)IMG_BUF_PTR, image_pointer[0], NUM_BYTES_BUFFER);
}

void vga_change() {
	counter += 1;
	if (counter % 3 == 0) {
		memcpy((void*)IMG_BUF_PTR, image_pointer[0], NUM_BYTES_BUFFER);
	} else if (counter % 3 == 1) {
		memcpy((void*)IMG_BUF_PTR, image_pointer[1], NUM_BYTES_BUFFER);
	} else {
		memcpy((void*)IMG_BUF_PTR, image_pointer[2], NUM_BYTES_BUFFER);
	}
}

void vga_loop()
{
	// Stub

}
