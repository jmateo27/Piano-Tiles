#include "includes/tile_generator_test.h"

int main(void){
	/*Enable the interrupt for the device and then cause (simulate) an interrupt so the handlers will be called*/
	setUpAudio();

	Xil_SetTlbAttributes(0xFFFF0000,0x14de2);

	while(COMM_VAL == 0);
	print("Hello World - ARM1\n\r");
	sleep(1);
	COMM_VAL = 0;

	while(COMM_VAL == 0);

	while(1){
		switch(COMM_VAL){
		case 1:
			audio_stream(song, song_size);
			break;
		case 2:
			audio_stream(piano, piano_size);
			break;
		case 3:
			audio_stream(drum, drum_size); //, tiles, tiles_count
			break;
		default:
			print("Weird\n\r");
		}
		COMM_VAL = 0;
		while(COMM_VAL == 0);
//		audio_stream(piano, piano_size); //, tiles, tiles_count
	}

//	menu();

	return 1;
}


