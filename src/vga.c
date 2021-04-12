#include <string.h>
#include "xil_io.h"
#include "vga.h"

#define IMG_BUF_PTR 0x00900000
#define NORMAL_COLOURS 0
#define ALTERNATE_COLOURS 1
#define HORIZONTAL_PIXEL_MIDDLE HORIZONTAL_PIXEL_MAX/2
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

#define ALPHABET_SPRITE_LENGTH 512
#define ALPHABET_SPRITE_ADDR 0x54630C0
#define ALPHABET_SPACE_OFFSET 0
#define ALPHABET_DOT_OFFSET 0x116F0
#define ALPHABET_SLASH_OFFSET 0x11770
#define ALPHABET_0_OFFSET 0x23000
#define ALPHABET_1_OFFSET 0x23068
#define ALPHABET_2_OFFSET 0x230E8
#define ALPHABET_3_OFFSET 0x23168
#define ALPHABET_4_OFFSET 0x231E8
#define ALPHABET_5_OFFSET 0x23268
#define ALPHABET_6_OFFSET 0x232E8
#define ALPHABET_7_OFFSET 0x23368
#define ALPHABET_8_OFFSET 0x233E8
#define ALPHABET_9_OFFSET 0x23468
#define ALPHABET_COLON_OFFSET 0x234E8
#define ALPHABET_A_OFFSET 0x33068
#define ALPHABET_B_OFFSET 0x330E8
#define ALPHABET_C_OFFSET 0x33168
#define ALPHABET_D_OFFSET 0x331E8
#define ALPHABET_E_OFFSET 0x33268
#define ALPHABET_F_OFFSET 0x332E8
#define ALPHABET_G_OFFSET 0x33368
#define ALPHABET_H_OFFSET 0x333E8
#define ALPHABET_I_OFFSET 0x33468
#define ALPHABET_J_OFFSET 0x334E8
#define ALPHABET_K_OFFSET 0x33568
#define ALPHABET_L_OFFSET 0x335E8
#define ALPHABET_M_OFFSET 0x33668
#define ALPHABET_N_OFFSET 0x336E8
#define ALPHABET_O_OFFSET 0x33768
#define ALPHABET_P_OFFSET 0x43000
#define ALPHABET_Q_OFFSET 0x43068
#define ALPHABET_R_OFFSET 0x430E8
#define ALPHABET_S_OFFSET 0x43168
#define ALPHABET_T_OFFSET 0x431E8
#define ALPHABET_U_OFFSET 0x43268
#define ALPHABET_V_OFFSET 0x432E8
#define ALPHABET_W_OFFSET 0x43368
#define ALPHABET_X_OFFSET 0x433E8
#define ALPHABET_Y_OFFSET 0x43468
#define ALPHABET_Z_OFFSET 0x434E8
#define ALPHABET_a_OFFSET 0x52068
#define ALPHABET_b_OFFSET 0x520E8
#define ALPHABET_c_OFFSET 0x52168
#define ALPHABET_d_OFFSET 0x521E8
#define ALPHABET_e_OFFSET 0x52268
#define ALPHABET_f_OFFSET 0x522E8
#define ALPHABET_g_OFFSET 0x52368
#define ALPHABET_h_OFFSET 0x523E8
#define ALPHABET_i_OFFSET 0x52468
#define ALPHABET_j_OFFSET 0x524E8
#define ALPHABET_k_OFFSET 0x52568
#define ALPHABET_l_OFFSET 0x525E8
#define ALPHABET_m_OFFSET 0x52668
#define ALPHABET_n_OFFSET 0x526E8
#define ALPHABET_o_OFFSET 0x52768
#define ALPHABET_p_OFFSET 0x61800
#define ALPHABET_q_OFFSET 0x61868
#define ALPHABET_r_OFFSET 0x618E8
#define ALPHABET_s_OFFSET 0x61968
#define ALPHABET_t_OFFSET 0x619E8
#define ALPHABET_u_OFFSET 0x61A68
#define ALPHABET_v_OFFSET 0x61AE8
#define ALPHABET_w_OFFSET 0x61B68
#define ALPHABET_x_OFFSET 0x61BE8
#define ALPHABET_y_OFFSET 0x61C68
#define ALPHABET_z_OFFSET 0x61CE8
#define ALPHABET_QUESTION_OFFSET 0x2376C
#define ALPHABET_OPEN_BRACKET_OFFSET 0x41578
#define ALPHABET_CLOSE_BRACKET_OFFSET 0x41678
// TODO: Add the rest of the special characters


// Location of the buttons
#define BUTTON_Y_OFFSET (HORIZONTAL_PIXEL_MAX*VERTICAL_PIXEL_MAX*21/24)
#define BUTTON_CONNECT_OFFSET (BUTTON_Y_OFFSET + HORIZONTAL_PIXEL_MAX/4 - HORIZONTAL_PIXEL_MAX/18)
#define BUTTON_LISTEN_OFFSET (BUTTON_Y_OFFSET + HORIZONTAL_PIXEL_MAX*3/4 - HORIZONTAL_PIXEL_MAX/18)
#define BUTTON_HISTORY_OFFSET (BUTTON_Y_OFFSET + HORIZONTAL_PIXEL_MAX/16)
#define BUTTON_KEYBOARD_OFFSET (BUTTON_Y_OFFSET + HORIZONTAL_PIXEL_MAX/4 + HORIZONTAL_PIXEL_MAX/16)
#define BUTTON_RECORDING_OFFSET (BUTTON_Y_OFFSET + HORIZONTAL_PIXEL_MAX/2 + HORIZONTAL_PIXEL_MAX/16)
#define BUTTON_CLOSE_OFFSET (BUTTON_Y_OFFSET + HORIZONTAL_PIXEL_MAX*7/8 - HORIZONTAL_PIXEL_MAX/16)

#define CHAT_BUF_OFFSET HORIZONTAL_PIXEL_MAX*(VERTICAL_PIXEL_MAX*4/5 - ALPHABET_CHAR_LENGTH)*4
#define CHAT_BUF_SIZE ALPHABET_CHAR_LENGTH*HORIZONTAL_PIXEL_MAX

#define SCREEN_IP 0
#define SCREEN_KB 1
#define SCREEN_CHAT 2
#define SCREEN_CONNECT 3
#define SCREEN_LISTEN 4

#define print_SPACE_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_SPACE_OFFSET))
#define print_DOT_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_DOT_OFFSET))
#define print_slash_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_SLASH_OFFSET))
#define print_0_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_0_OFFSET))
#define print_1_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_1_OFFSET))
#define print_2_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_2_OFFSET))
#define print_3_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_3_OFFSET))
#define print_4_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_4_OFFSET))
#define print_5_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_5_OFFSET))
#define print_6_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_6_OFFSET))
#define print_7_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_7_OFFSET))
#define print_8_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_8_OFFSET))
#define print_9_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_9_OFFSET))
#define print_COLON_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_COLON_OFFSET))
#define print_A_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_A_OFFSET))
#define print_B_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_B_OFFSET))
#define print_C_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_C_OFFSET))
#define print_D_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_D_OFFSET))
#define print_E_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_E_OFFSET))
#define print_F_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_F_OFFSET))
#define print_G_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_G_OFFSET))
#define print_H_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_H_OFFSET))
#define print_I_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_I_OFFSET))
#define print_J_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_J_OFFSET))
#define print_K_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_K_OFFSET))
#define print_L_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_L_OFFSET))
#define print_M_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_M_OFFSET))
#define print_N_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_N_OFFSET))
#define print_O_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_O_OFFSET))
#define print_P_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_P_OFFSET))
#define print_Q_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_Q_OFFSET))
#define print_R_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_R_OFFSET))
#define print_S_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_S_OFFSET))
#define print_T_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_T_OFFSET))
#define print_U_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_U_OFFSET))
#define print_V_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_V_OFFSET))
#define print_W_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_W_OFFSET))
#define print_X_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_X_OFFSET))
#define print_Y_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_Y_OFFSET))
#define print_Z_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_Z_OFFSET))
#define print_a_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_a_OFFSET))
#define print_b_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_b_OFFSET))
#define print_c_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_c_OFFSET))
#define print_d_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_d_OFFSET))
#define print_e_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_e_OFFSET))
#define print_f_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_f_OFFSET))
#define print_g_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_g_OFFSET))
#define print_h_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_h_OFFSET))
#define print_i_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_i_OFFSET))
#define print_j_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_j_OFFSET))
#define print_k_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_k_OFFSET))
#define print_l_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_l_OFFSET))
#define print_m_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_m_OFFSET))
#define print_n_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_n_OFFSET))
#define print_o_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_o_OFFSET))
#define print_p_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_p_OFFSET))
#define print_q_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_q_OFFSET))
#define print_r_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_r_OFFSET))
#define print_s_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_s_OFFSET))
#define print_t_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_t_OFFSET))
#define print_u_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_u_OFFSET))
#define print_v_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_v_OFFSET))
#define print_w_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_w_OFFSET))
#define print_x_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_x_OFFSET))
#define print_y_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_y_OFFSET))
#define print_z_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_z_OFFSET))
#define print_question_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_QUESTION_OFFSET))
#define print_open_bracket_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_OPEN_BRACKET_OFFSET))
#define print_close_bracket_character(ptr) print_character(ptr, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_CLOSE_BRACKET_OFFSET))


static int red = 0xFF0000FF;
static int green = 0xFF00FF00;
static int blue = 0xFFFF0000;
static int yellow = 0xFF00FFFF;
static int purple = 0xFFFF00FF;

static int * image_pointer[5] = {(int *)0x018D2008, (int *)0x020BB00C, (int *)0x028A4010, (int *)0x0308D014, (int *)0x03876018};
static int kb_key_center_x[12] = {305, 415, 525, 635, 745, 855, 965, 1075, 1185, 1295, 1405, 1615};
static int kb_key_center_y[4] = {511, 621, 731, 841};	// Don't count space
static int kb_space_center_x = 855;
static int kb_space_center_y = 976;
static int counter = 0;

static int current_screen;

static int *kb_screen_previous_ptr = (int *)0x05500000;
static int kb_screen_previous_id;

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

static void vga_update();
static void print_character(int* image_pointer, int* sprite);
static void print_string(int* image_pointer, char* str);
static void set_kb_square_key(int* image_pointer, int* kb_normal_sprite, int center_x, int center_y);
static void set_kb_rectangle_key(int* image_pointer, int* kb_normal_sprite, int center_x, int center_y);
static void set_kb_space_key(int* image_pointer, int* kb_normal_sprite, int center_x, int center_y);
static void backspace_character(int* address);

void setColours(int colour, int* image_pointer[5])
{

	red = 0xFF0000FF;
	green = 0xFF00FF00;
	blue = 0xFFFF0000;
	yellow = 0xFF00FFFF;
	purple = 0xFFFF00FF;

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
				set_kb_square_key(image_pointer[1], kb_normal_sprite[i*KEYBOARD_NUM_OF_KEY_X + j], kb_key_center_x[j], kb_key_center_y[i]);
			} else {
				set_kb_rectangle_key(image_pointer[1], kb_normal_sprite[i*KEYBOARD_NUM_OF_KEY_X + j], kb_key_center_x[j], kb_key_center_y[i]);
			}
		}
	}

	set_kb_square_key(image_pointer[1], kb_selected_sprite[0], kb_key_center_x[0], kb_key_center_y[0]);

	//print_character(image_pointer[0] + BUTTON_CONNECT_OFFSET, (int *)(ALPHABET_SPRITE_ADDR + ALPHABET_C_OFFSET));
	print_string(image_pointer[0] + BUTTON_CONNECT_OFFSET, "CONNECT");
	print_string(image_pointer[0] + BUTTON_LISTEN_OFFSET, "LISTEN");

	print_string(image_pointer[2] + BUTTON_HISTORY_OFFSET, "HISTORY(L)");
	print_string(image_pointer[2] + BUTTON_KEYBOARD_OFFSET, "KEYBOARD(D)");
	print_string(image_pointer[2] + BUTTON_RECORDING_OFFSET, "RECORDING(U)");
	print_string(image_pointer[2] + BUTTON_CLOSE_OFFSET, "CLOSE(R)");

	set_kb_space_key(image_pointer[1], kb_selected_sprite[48], kb_space_center_x, kb_space_center_y);
}

static void print_character(int* image_pointer, int* sprite)
{
	for (int y = 0; y < ALPHABET_CHAR_LENGTH; ++y) {
		for (int x = 0; x < ALPHABET_CHAR_LENGTH; ++x) {
			int addr_offset = HORIZONTAL_PIXEL_MAX*y + x;
			int offset = y*ALPHABET_SPRITE_LENGTH + x;
			memcpy(image_pointer + addr_offset, sprite + offset, 4);
		}
	}
}

void backspace_character(int* address) {
	for (int y = 0; y < ALPHABET_CHAR_LENGTH; ++y) {
		for (int x = 0; x < ALPHABET_CHAR_LENGTH; ++x) {
			int addr_offset = HORIZONTAL_PIXEL_MAX*y + x;
			memcpy(image_pointer + addr_offset, &yellow, 4);
		}
	}
}


static void print_string(int* image_pointer, char* str)
{
	for (int i = 0; i < strlen(str); i++) {
		switch(str[i]) {
			case ' ':
				print_SPACE_character(image_pointer + 32*i);
				break;
			case '.':
				print_DOT_character(image_pointer + 32*i);
				break;
			case '/':
				print_slash_character(image_pointer + 32*i);
				break;
			case '0':
				print_0_character(image_pointer + 32*i);
				break;
			case '1':
				print_1_character(image_pointer + 32*i);
				break;
			case '2':
				print_2_character(image_pointer + 32*i);
				break;
			case '3':
				print_3_character(image_pointer + 32*i);
				break;
			case '4':
				print_4_character(image_pointer + 32*i);
				break;
			case '5':
				print_5_character(image_pointer + 32*i);
				break;
			case '6':
				print_6_character(image_pointer + 32*i);
				break;
			case '7':
				print_7_character(image_pointer + 32*i);
				break;
			case '8':
				print_8_character(image_pointer + 32*i);
				break;
			case '9':
				print_9_character(image_pointer + 32*i);
				break;
			case ':':
				print_COLON_character(image_pointer + 32*i);
				break;
			case 'A':
				print_A_character(image_pointer + 32*i);
				break;
			case 'B':
				print_B_character(image_pointer + 32*i);
				break;
			case 'C':
				print_C_character(image_pointer + 32*i);
				break;
			case 'D':
				print_D_character(image_pointer + 32*i);
				break;
			case 'E':
				print_E_character(image_pointer + 32*i);
				break;
			case 'F':
				print_F_character(image_pointer + 32*i);
				break;
			case 'G':
				print_G_character(image_pointer + 32*i);
				break;
			case 'H':
				print_H_character(image_pointer + 32*i);
				break;
			case 'I':
				print_I_character(image_pointer + 32*i);
				break;
			case 'J':
				print_J_character(image_pointer + 32*i);
				break;
			case 'K':
				print_K_character(image_pointer + 32*i);
				break;
			case 'L':
				print_L_character(image_pointer + 32*i);
				break;
			case 'M':
				print_M_character(image_pointer + 32*i);
				break;
			case 'N':
				print_N_character(image_pointer + 32*i);
				break;
			case 'O':
				print_O_character(image_pointer + 32*i);
				break;
			case 'P':
				print_P_character(image_pointer + 32*i);
				break;
			case 'Q':
				print_Q_character(image_pointer + 32*i);
				break;
			case 'R':
				print_R_character(image_pointer + 32*i);
				break;
			case 'S':
				print_S_character(image_pointer + 32*i);
				break;
			case 'T':
				print_T_character(image_pointer + 32*i);
				break;
			case 'U':
				print_U_character(image_pointer + 32*i);
				break;
			case 'V':
				print_V_character(image_pointer + 32*i);
				break;
			case 'W':
				print_W_character(image_pointer + 32*i);
				break;
			case 'X':
				print_X_character(image_pointer + 32*i);
				break;
			case 'Y':
				print_Y_character(image_pointer + 32*i);
				break;
			case 'Z':
				print_Z_character(image_pointer + 32*i);
				break;
			case 'a':
				print_a_character(image_pointer + 32*i);
				break;
			case 'b':
				print_b_character(image_pointer + 32*i);
				break;
			case 'c':
				print_c_character(image_pointer + 32*i);
				break;
			case 'd':
				print_d_character(image_pointer + 32*i);
				break;
			case 'e':
				print_e_character(image_pointer + 32*i);
				break;
			case 'f':
				print_f_character(image_pointer + 32*i);
				break;
			case 'g':
				print_g_character(image_pointer + 32*i);
				break;
			case 'h':
				print_h_character(image_pointer + 32*i);
				break;
			case 'i':
				print_i_character(image_pointer + 32*i);
				break;
			case 'j':
				print_j_character(image_pointer + 32*i);
				break;
			case 'k':
				print_k_character(image_pointer + 32*i);
				break;
			case 'l':
				print_l_character(image_pointer + 32*i);
				break;
			case 'm':
				print_m_character(image_pointer + 32*i);
				break;
			case 'n':
				print_n_character(image_pointer + 32*i);
				break;
			case 'o':
				print_o_character(image_pointer + 32*i);
				break;
			case 'p':
				print_p_character(image_pointer + 32*i);
				break;
			case 'q':
				print_q_character(image_pointer + 32*i);
				break;
			case 'r':
				print_r_character(image_pointer + 32*i);
				break;
			case 's':
				print_s_character(image_pointer + 32*i);
				break;
			case 't':
				print_t_character(image_pointer + 32*i);
				break;
			case 'u':
				print_u_character(image_pointer + 32*i);
				break;
			case 'v':
				print_v_character(image_pointer + 32*i);
				break;
			case 'w':
				print_w_character(image_pointer + 32*i);
				break;
			case 'x':
				print_x_character(image_pointer + 32*i);
				break;
			case 'y':
				print_y_character(image_pointer + 32*i);
				break;
			case 'z':
				print_z_character(image_pointer + 32*i);
				break;
			case '?':
				print_question_character(image_pointer + 32*i);
				break;
			case '[':
				print_open_bracket_character(image_pointer + 32*i);
				break;
			case ']':
				print_close_bracket_character(image_pointer + 32*i);
				break;
			case '#':
				// Special char for backspace
				backspace_character(image_pointer + 32*i);
				break;
		}
	}
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

	current_screen = SCREEN_IP;

	setColours(NORMAL_COLOURS, image_pointer);

	memcpy((void*)IMG_BUF_PTR, image_pointer[0], NUM_BYTES_BUFFER);
}

void vga_clear_chat_buf()
{
	for (int i = 0; i < CHAT_BUF_SIZE; i++) {
		memcpy((void *)(IMG_BUF_PTR + CHAT_BUF_OFFSET + 4*i), &yellow, 4);
	}
}

void vga_switch_to_IP() {
	current_screen = SCREEN_IP;
	vga_update();
}

void vga_switch_to_KB() {
	// Save the current screen before switching
	memcpy(kb_screen_previous_ptr, (void*)IMG_BUF_PTR, NUM_BYTES_BUFFER);
	kb_screen_previous_id = current_screen;
	// Update the screen to on screen keyboard
	current_screen = SCREEN_KB;
	vga_update();
}

void vga_switch_to_KB_previous() {
	// Load the previous screen before changing to KB
	// This way, keyboard will just be a substate that belongs to a state controlled by main.c
	// Don't need to add another state in state.h
	memcpy((void*)IMG_BUF_PTR, kb_screen_previous_ptr, NUM_BYTES_BUFFER);
	current_screen = kb_screen_previous_id;
}

void vga_switch_to_CHAT() {
	current_screen = SCREEN_CHAT;
	vga_update();
}

void vga_switch_to_connect() {
	current_screen = SCREEN_CONNECT;
	vga_update();
}

void vga_switch_to_listen() {
	current_screen = SCREEN_IP;
	vga_update();
}

// Internal function that change the base screen depending on what function is being used
static void vga_update() {
	switch (current_screen) {
		case SCREEN_IP:
			memcpy((void*)IMG_BUF_PTR, image_pointer[0], NUM_BYTES_BUFFER);
			break;
		case SCREEN_KB:
			// Have to worry about the cursor's current location
			memcpy((void*)IMG_BUF_PTR, image_pointer[1], NUM_BYTES_BUFFER);
			break;
		case SCREEN_CHAT:
			memcpy((void*)IMG_BUF_PTR, image_pointer[2], NUM_BYTES_BUFFER);
			break;
		case SCREEN_CONNECT:
			memcpy((void*)IMG_BUF_PTR, image_pointer[3], NUM_BYTES_BUFFER);
			break;
		case SCREEN_LISTEN:
			memcpy((void*)IMG_BUF_PTR, image_pointer[4], NUM_BYTES_BUFFER);
			break;
		default:
			xil_printf("Unknown screen ID");
			break;
	}
}

// This function lets keyboard_logic control the cursor's movement in the vga
// This function is only called when vga screen is SCREEN_KB
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

void ip_move_cursor() {
	// Clear the cursor of the current button

	// Enable the cursor for the selected button
}

void chat_move_cursor() {
	// Clear the cursor of the current button

	// Enable the cursor for the selected button
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

void vga_print_character(int x_offset, int y_offset, char c)
{
	char str[1] = "";
	strncat(str, &c, 1);
	print_string((void *)(IMG_BUF_PTR + y_offset*HORIZONTAL_PIXEL_MAX*4 + x_offset*4), str);
}

void vga_print_string(int x_offset, int y_offset, char *str)
{
	print_string((void *)(IMG_BUF_PTR + y_offset*HORIZONTAL_PIXEL_MAX*4 + x_offset*4), str);
}
