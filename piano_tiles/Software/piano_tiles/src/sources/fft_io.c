#include "../includes/fft_io.h"

u32 readR0(){
	return PIANOTILES_IO_mReadReg(TILE_BASE, R0_OFFSET);
}

void writeR1(u32 in){
	PIANOTILES_IO_mWriteReg(TILE_BASE, R1_OFFSET, in);
}

void reset(){
	writeR1(1);
}
void unReset(){
	writeR1(0);
}
