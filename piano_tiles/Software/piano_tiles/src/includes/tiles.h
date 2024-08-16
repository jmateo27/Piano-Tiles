#ifndef TILES_H
#define TILES_H

#include "xil_exception.h"
#include "xdebug.h"
#include "xparameters.h"
#include "xil_io.h"
#include "xil_util.h"
#include "xiicps.h"

#include "interrupts.h"

typedef struct{
	u32 magnitude;
	int column;
} TileInfo;

#define MAX_TILE_ARRAY_LEN 			1000
#define MAGNITUDE_MASK 				0x3FFFFF
#define COLUMN_MASK 				0xFFC00000

void displayTileSummary(TileInfo *tiles, int tiles_count, int *count, int *avg, int *tiles_converted);

void displayTile(int column);

int convertColumn(int column);

void analyzeTiles(TileInfo *tiles, int *tilesOut, int th1, int th2, int th3, int th4);

#endif
