#include <string.h>
#include "xil_io.h"
#include "vga.h"

#define IMG_BUF_PTR 0x00900000
#define NORMAL_COLOURS 0
#define ALTERNATE_COLOURS 1
#define HORIZONTAL_PIXEL_MAX 1920
#define HORIZONTAL_PIXEL_MIDDLE HORIZONTAL_PIXEL_MAX/2
#define VERTICAL_PIXEL_MAX 1080
#define VERTICAL_PIXEL_MIDDLE VERTICAL_PIXEL_MAX/2
#define NUM_BYTES_BUFFER HORIZONTAL_PIXEL_MAX*VERTICAL_PIXEL_MAX*4

#define KEYBOARD_HORIZONTAL_PIXEL 1410
#define KEYBOARD_VERTICAL_PIXEL 540
#define KEYBOARD_SQUARE_KEY_SIZE 100
#define KEYBOARD_RECT_KEY_SIZE_X 200
#define KEYBOARD_RECT_KEY_SIZE_Y 100
#define KEYBOARD_SPACE_SIZE_X 500
#define KEYBOARD_SPACE_SIZE_Y 100

#define KEYBOARD_START_PIXEL_X HORIZONTAL_PIXEL_MIDDLE-KEYBOARD_HORIZONTAL_PIXEL/2
#define KEYBOARD_START_PIXEL_Y 461

#define KEYBOARD_NUM_OF_KEY_X 12
#define KEYBOARD_NUM_OF_KEY_Y 4 // Not counting space key

#define RGB_RED 0xFF0000FF

#define IP_SCREEN 0
#define KB_SCREEN 1
#define CHAT_SCREEN 2


static int * image_pointer[5] = {(int *)0x018D2008, (int *)0x020BB00C, (int *)0x028A4010, (int *)0x0308D014, (int *)0x03876018};
static int kb_key_center_x[12] = {305, 415, 525, 635, 745, 855, 965, 1075, 1185, 1295, 1405, 1615};
static int kb_key_center_y[4] = {511, 621, 731, 841};	// Don't count space
static int kb_space_center_x = 855;
static int kb_space_center_y = 976;
static int counter = 0;

static int current_screen;

// Setting up a predefined address to access the sprites images
// The sprites are uploaded to the board through xsct dow -data
// TODO: Find a way to do this without using xsct shell
//static int* kb_normal_sprite = (int*)0x05000000;
//static int* kb_normal_sprite[2] = {(int*)0x05000000, (int*)0x0500C940};

static int* kb_normal_sprite[49] = {(int*)0x5000000, (int*)0x5009c40, (int*)0x5013880, (int*)0x501d4c0, (int*)0x5027100, (int*)0x5030d40, (int*)0x503a980, (int*)0x50445c0, (int*)0x504e200, (int*)0x5057e40, (int*)0x5061a80, (int*)0x506b6c0, \
		(int*)0x507ef40, (int*)0x5088b80, (int*)0x50927c0, (int*)0x509c400, (int*)0x50a6040, (int*)0x50afc80, (int*)0x50b98c0, (int*)0x50c3500, (int*)0x50cd140, (int*)0x50d6d80, (int*)0x50e09c0, (int*)0x50ea600, \
		(int*)0x50fde80, (int*)0x5107ac0, (int*)0x5111700, (int*)0x511b340, (int*)0x5124f80, (int*)0x512ebc0, (int*)0x5138800, (int*)0x5142440, (int*)0x514c080, (int*)0x5155cc0, (int*)0x515f900, (int*)0x5169540, \
		(int*)0x517cdc0, (int*)0x5186a00, (int*)0x5190640, (int*)0x519a280, (int*)0x51a3ec0, (int*)0x51adb00, (int*)0x51b7740, (int*)0x51c1380, (int*)0x51cafc0, (int*)0x51d4c00, (int*)0x51de840, (int*)0x51e8480, \
		(int*)0x51fbd00};

static int* kb_selected_sprite[49] = {(int*)0x5236680, (int*)0x52402c0, (int*)0x5249f00, (int*)0x5253b40, (int*)0x525d780, (int*)0x52673c0, (int*)0x5271000, (int*)0x527ac40, (int*)0x5284880, (int*)0x528e4c0, (int*)0x5298100, (int*)0x52a1d40, \
		(int*)0x52b55c0, (int*)0x52bf200, (int*)0x52c8e40, (int*)0x52d2a80, (int*)0x52dc6c0, (int*)0x52e6300, (int*)0x52eff40, (int*)0x52f9b80, (int*)0x53037c0, (int*)0x530d400, (int*)0x5317040, (int*)0x5320c80, \
		(int*)0x5334500, (int*)0x533e140, (int*)0x5347d80, (int*)0x53519c0, (int*)0x535b600, (int*)0x5365240, (int*)0x536ee80, (int*)0x5378ac0, (int*)0x5382700, (int*)0x538c340, (int*)0x5395f80, (int*)0x539fbc0, \
		(int*)0x53b3440, (int*)0x53bd080, (int*)0x53c6cc0, (int*)0x53d0900, (int*)0x53da540, (int*)0x53e4180, (int*)0x53eddc0, (int*)0x53f7a00, (int*)0x5401640, (int*)0x540b280, (int*)0x5414ec0, (int*)0x541eb00, \
		(int*)0x5432380};

static void update_screen();

static void set_kb_square_key(int* image_pointer, int* kb_normal_sprite, int center_x, int center_y);
static void set_kb_rectangle_key(int* image_pointer, int* kb_normal_sprite, int center_x, int center_y);
static void set_kb_space_key(int* image_pointer, int* kb_normal_sprite, int center_x, int center_y);

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
	for (int y = 0; y < VERTICAL_PIXEL_MAX; y++) {
		for (int x = 0; x < HORIZONTAL_PIXEL_MAX; x++) {
			addr_offset = HORIZONTAL_PIXEL_MAX*y + x;

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

	// New loop for memcpy specific location
	// Onscreen keyboard layout
	// Each normal key is 100x100, 4 specials function keys have dimension 200x100
	// The spacebar is 300x150 (should be at the middle)
	// In total, we need an area of 1300x400 for the keyboards + 300x150 at the bottom for the space key
	// Plus maybe some gap between the keys (10 pixels?)

	for (int i = 0; i < KEYBOARD_NUM_OF_KEY_Y; ++i) {
		for (int j = 0; j < KEYBOARD_NUM_OF_KEY_X; ++j) {
			if (j != KEYBOARD_NUM_OF_KEY_X-1) {
				set_kb_square_key(image_pointer[1], kb_selected_sprite[i*KEYBOARD_NUM_OF_KEY_X + j], kb_key_center_x[j], kb_key_center_y[i]);
			} else {
				set_kb_rectangle_key(image_pointer[1], kb_selected_sprite[i*KEYBOARD_NUM_OF_KEY_X + j], kb_key_center_x[j], kb_key_center_y[i]);
			}
		}
	}

	set_kb_space_key(image_pointer[1], kb_selected_sprite[48], kb_space_center_x, kb_space_center_y);
}

static void set_kb_square_key(int* image_pointer, int* kb_sprite, int center_x, int center_y) {
	int start_x = center_x - KEYBOARD_SQUARE_KEY_SIZE/2;
	int start_y = center_y - KEYBOARD_SQUARE_KEY_SIZE/2;

	int end_x = center_x + KEYBOARD_SQUARE_KEY_SIZE/2;
	int end_y = center_y + KEYBOARD_SQUARE_KEY_SIZE/2;

	//int red = RGB_RED;
	// Set color for key here
	// Or copy spites info to here
	int sprite_offset = 0;
	for (int y = start_y; y < end_y; ++y) {
		for (int x = start_x; x < end_x; ++x) {
			int addr_offset = HORIZONTAL_PIXEL_MAX*y + x;
			//memcpy(image_pointer + addr_offset, &red, 4);
			memcpy(image_pointer + addr_offset, kb_sprite + sprite_offset, 4);
			++sprite_offset;
		}
	}
}

static void set_kb_rectangle_key(int* image_pointer, int* kb_normal_sprite, int center_x, int center_y) {
	int start_x = center_x - KEYBOARD_RECT_KEY_SIZE_X/2;
	int start_y = center_y - KEYBOARD_RECT_KEY_SIZE_Y/2;

	int end_x = center_x + KEYBOARD_RECT_KEY_SIZE_X/2;
	int end_y = center_y + KEYBOARD_RECT_KEY_SIZE_Y/2;

	//int red = RGB_RED;
	// Set color for key here
	// Or copy spites info to here
	int sprite_offset = 0;
	for (int y = start_y; y < end_y; ++y) {
		for (int x = start_x; x < end_x; ++x) {
			int addr_offset = HORIZONTAL_PIXEL_MAX*y + x;
			//memcpy(image_pointer + addr_offset, &red, 4);
			memcpy(image_pointer + addr_offset, kb_normal_sprite + sprite_offset, 4);
			++sprite_offset;
		}
	}
}

static void set_kb_space_key(int* image_pointer, int* kb_normal_sprite, int center_x, int center_y) {
	int start_x = center_x - KEYBOARD_SPACE_SIZE_X/2;
	int start_y = center_y - KEYBOARD_SPACE_SIZE_Y/2;

	int end_x = center_x + KEYBOARD_SPACE_SIZE_X/2;
	int end_y = center_y + KEYBOARD_SPACE_SIZE_Y/2;

	//int red = RGB_RED;
	// Set color for key here
	// Or copy spites info to here
	int sprite_offset = 0;
	for (int y = start_y; y < end_y; ++y) {
		for (int x = start_x; x < end_x; ++x) {
			int addr_offset = HORIZONTAL_PIXEL_MAX*y + x;
			//memcpy(image_pointer + addr_offset, &red, 4);
			memcpy(image_pointer + addr_offset, kb_normal_sprite + sprite_offset, 4);
			++sprite_offset;
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

	current_screen = IP_SCREEN;

	setColours(NORMAL_COLOURS, image_pointer);

	memcpy((void*)IMG_BUF_PTR, image_pointer[0], NUM_BYTES_BUFFER);
}

void vga_switch_to_IP() {
	current_screen = IP_SCREEN;
	update_screen();
}

void vga_switch_to_KB() {
	current_screen = KB_SCREEN;
	update_screen();
}

void vga_switch_to_CHAT() {
	current_screen = CHAT_SCREEN;
	update_screen();
}

// Internal function that change the base screen depending on what function is being used
static void update_screen() {
	switch (current_screen) {
	case IP_SCREEN:
		memcpy((void*)IMG_BUF_PTR, image_pointer[0], NUM_BYTES_BUFFER);
		break;
	case KB_SCREEN:
		// Have to worry about the cursor's current location
		memcpy((void*)IMG_BUF_PTR, image_pointer[1], NUM_BYTES_BUFFER);
		break;
	case CHAT_SCREEN:
		memcpy((void*)IMG_BUF_PTR, image_pointer[2], NUM_BYTES_BUFFER);
		break;
	default:
		xil_printf("Unknown screen ID");
		break;
	}
}

// This function lets keyboard_logic control the cursor's movement in the vga
// This function is only called when vga screen is KB_SCREEN
// Call this function before updating current location
// Both currentID and nextID have a range from 0 to 49
void kb_move_cursor(int currentID, int nextID) {
	int currentX, currentY, nextX, nextY;
	currentX = currentID % KEYBOARD_NUM_OF_KEY_X;
	currentY = currentID / KEYBOARD_NUM_OF_KEY_X;
	nextX = nextID % KEYBOARD_NUM_OF_KEY_X;
	nextY = nextID / KEYBOARD_NUM_OF_KEY_X;

	// Change the current sprite to normal
	if (currentID >= 49) {
		set_kb_space_key((void*)IMG_BUF_PTR, kb_normal_sprite[48], kb_space_center_x, kb_space_center_y);
	} else if (currentX == KEYBOARD_NUM_OF_KEY_X - 1) {
		set_kb_rectangle_key((void*)IMG_BUF_PTR, kb_normal_sprite[currentID], kb_key_center_x[currentX], kb_key_center_y[currentY]);
	} else {
		set_kb_square_key((void*)IMG_BUF_PTR, kb_normal_sprite[currentID], kb_key_center_x[currentX], kb_key_center_y[currentY]);
	}

	// Change the next sprite to selected
	if (nextID >= 49) {
		set_kb_space_key((void*)IMG_BUF_PTR, kb_selected_sprite[48], kb_space_center_x, kb_space_center_y);
	} else if (nextX == KEYBOARD_NUM_OF_KEY_X - 1) {
		set_kb_rectangle_key((void*)IMG_BUF_PTR, kb_selected_sprite[nextID], kb_key_center_x[nextX], kb_key_center_y[nextY]);
	} else {
		set_kb_square_key((void*)IMG_BUF_PTR, kb_selected_sprite[nextID], kb_key_center_x[nextX], kb_key_center_y[nextY]);
	}
}

// Deprecated
// Use switch commands now
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
