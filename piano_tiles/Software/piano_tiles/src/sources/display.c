#include "../includes/display.h"

void shiftColourArrayLeftorDown(unsigned int *COLOR_ARRAY){
	unsigned int temp = COLOR_ARRAY[0];
	for (int i = 0; i < 4; ++i){
		COLOR_ARRAY[i] = COLOR_ARRAY[i+1];
	}
	COLOR_ARRAY[4] = temp;
}

void shiftColourArrayRightorUp(unsigned int *COLOR_ARRAY){
	unsigned int temp = COLOR_ARRAY[4];
	for (int i = 4; i > 0; --i){
		COLOR_ARRAY[i] = COLOR_ARRAY[i-1];
	}
	COLOR_ARRAY[0] = temp;
}
