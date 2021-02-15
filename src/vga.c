#include <string.h>
#include "xil_io.h"
#include "vga.h"

#define IMG_BUF_PTR 0x00900000
#define NUM_BYTES_BUFFER 5242880


static int img = 0;
static char buf = 0;
static int * image_pointer[5] = {(int *)0x018D2008, (int *)0x020BB00C, (int *)0x028A4010, (int *)0x0308D014, (int *)0x03876018};

void vga_init()
{
	memset(image_pointer[0], 0, NUM_BYTES_BUFFER);
	memset(image_pointer[1], 0, NUM_BYTES_BUFFER);
	memset(image_pointer[2], 0, NUM_BYTES_BUFFER);
	memset(image_pointer[3], 0, NUM_BYTES_BUFFER);
	memset(image_pointer[4], 0, NUM_BYTES_BUFFER);

	setColours(0, image_pointer);
}

void vga_loop()
{
	if (buf == 0) {
		memcpy((void*)IMG_BUF_PTR, image_pointer[0], NUM_BYTES_BUFFER);
	}

	buf = inbyte();

	if(buf == '1'){
		setColours(0, image_pointer);
		memcpy((void*)IMG_BUF_PTR, image_pointer[img], NUM_BYTES_BUFFER);

	}
	else if(buf == '2'){
		setColours(1, image_pointer);
		memcpy((void*)IMG_BUF_PTR, image_pointer[img], NUM_BYTES_BUFFER);

	}
	else if(buf == 'w'){ // Shift up
		if(img == 0){
			memcpy((void*)IMG_BUF_PTR, image_pointer[4], NUM_BYTES_BUFFER);
			img = 4;
		}
		else {
			memcpy((void*)IMG_BUF_PTR, image_pointer[img-1], NUM_BYTES_BUFFER);
			img--;
		}
	}
	else if(buf == 's'){ // Shift down
		if(img == 4){
			memcpy((void*)IMG_BUF_PTR, image_pointer[0], NUM_BYTES_BUFFER);
			img = 0;
		}
		else {
			memcpy((void*)IMG_BUF_PTR, image_pointer[img+1], NUM_BYTES_BUFFER);
			img++;
		}
	}
}

void setColours(int colour, int* image_pointer[5])
{

	int red = 0xFF0000FF;
	int green = 0xFF00FF00;
	int blue = 0xFFFF0000;
	int yellow = 0x0000FFFF;
	int purple = 0x00FF00FF;

	if(colour == 1){
		red = 0xFFECFF00;
		green = 0xFFF000FF;
		blue = 0xFFFFFFFF;
		yellow = 0xFF0087FF;
		purple = 0xFF97FF00;
	}

	int i = 0;

	for(; i < NUM_BYTES_BUFFER / 20; i++){
		memcpy(image_pointer[0] + i, &red, 4);
		memcpy(image_pointer[1] + i, &yellow, 4);
		memcpy(image_pointer[2] + i, &purple, 4);
		memcpy(image_pointer[3] + i, &blue, 4);
		memcpy(image_pointer[4] + i, &green, 4);
	}

	for(; i < NUM_BYTES_BUFFER * 2 / 20; i++){
		memcpy(image_pointer[0] + i, &green, 4);
		memcpy(image_pointer[1] + i, &red, 4);
		memcpy(image_pointer[2] + i, &yellow, 4);
		memcpy(image_pointer[3] + i, &purple, 4);
		memcpy(image_pointer[4] + i, &blue, 4);
	}

	for(; i < NUM_BYTES_BUFFER * 3 / 20; i++){
		memcpy(image_pointer[0] + i, &blue, 4);
		memcpy(image_pointer[1] + i, &green, 4);
		memcpy(image_pointer[2] + i, &red, 4);
		memcpy(image_pointer[3] + i, &yellow, 4);
		memcpy(image_pointer[4] + i, &purple, 4);
	}

	for(; i < NUM_BYTES_BUFFER * 4 / 20; i++){
		memcpy(image_pointer[0] + i, &purple, 4);
		memcpy(image_pointer[1] + i, &blue, 4);
		memcpy(image_pointer[2] + i, &green, 4);
		memcpy(image_pointer[3] + i, &red, 4);
		memcpy(image_pointer[4] + i, &yellow, 4);
	}

	for(; i < NUM_BYTES_BUFFER * 5 / 20; i++){
		memcpy(image_pointer[0] + i, &yellow, 4);
		memcpy(image_pointer[1] + i, &purple, 4);
		memcpy(image_pointer[2] + i, &blue, 4);
		memcpy(image_pointer[3] + i, &green, 4);
		memcpy(image_pointer[4] + i, &red, 4);
	}
}
