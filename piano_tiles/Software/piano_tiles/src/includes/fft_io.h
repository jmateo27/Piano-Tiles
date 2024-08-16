#ifndef FFT_IO_H
#define FFT_IO_H

#include "xparameters.h"
#include "pianotiles_io.h"
#include "xil_io.h"
#include <stdbool.h>

#define TILE_BASE XPAR_PIANOTILES_IO_0_S00_AXI_BASEADDR

#define R0_OFFSET PIANOTILES_IO_S00_AXI_SLV_REG0_OFFSET
#define R1_OFFSET PIANOTILES_IO_S00_AXI_SLV_REG1_OFFSET

#define BYTE 4

u32 readR0();
void writeR1(u32 in);

void reset();
void unReset();

#endif // FFT_IO_H
