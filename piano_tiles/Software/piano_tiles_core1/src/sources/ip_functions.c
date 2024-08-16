#include "../includes/tile_generator_test.h"

void setUpAudio(){
	//Configure the IIC data structure
	IicConfig(XPAR_XIICPS_0_DEVICE_ID);

	//Configure the Audio Codec's PLL
	AudioPllConfig();

	//Configure the Line in and Line out ports.
	//Call LineInLineOutConfig() for a configuration that
	//enables the HP jack too.
	AudioConfigureJacks();

	//xil_printf("ADAU1761 configured\n\r");
}

void audio_stream(unsigned char *data, int data_len){ //, TileInfo *tiles, int tiles_count

	u16 *ptr = (u16 *)data;
	int samples_len = data_len / sizeof(u16);
//	int inc = samples_len / tiles_count;
//	int j;
//	int threshold = 3000000;
//	int threshold2 = 0;
//	int flag = 0;

	for (int i = 0; i < samples_len; i++){
		usleep(23);
//		if (i % inc == 0) {
//			j = i/inc;
//			switch(tiles[j].column){
//				case 0:
//					if (tiles[j].magnitude > threshold) displayTile(0);
//					else displayTile(4);
//					break;
//				case 1023:
//					if (tiles[j].magnitude > threshold) displayTile(3);
//					else displayTile(4);
//					break;
//				default:
//					if (tiles[j].magnitude >= threshold2){
//						if (flag % 5 == 0){
//							if (tiles[j].column < 512) displayTile(1);
//							else displayTile(2);
//						}
//						else displayTile(4);
//						flag++;
//					}
//					else if ((flag-1) % 5 == 0) displayTile(4);
//			}
//		}

		int16_t sample_16b = *((int16_t *)ptr);
		int32_t sample_32b = ((int32_t)sample_16b) << 6;
		Xil_Out32(I2S_DATA_TX_L_REG, sample_32b);
		Xil_Out32(I2S_DATA_TX_R_REG, sample_32b);
		ptr++;
	}
}
