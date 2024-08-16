#include "../includes/tiles.h"

void displayTileSummary(TileInfo *tiles, int tiles_count, int *count, int *avg, int *tiles_converted){
	xil_printf("Tiles generated, %d in total.\r\n", tiles_count);
	for(int i = 0; i < 4; i++){
		count[i] = 0;
		avg[i] = 0;
	}

	int threshold = 3000000;
	int threshold2 = 0;
	int thresholds[] = {threshold, threshold2, threshold2, threshold};
	int col;
	for (int i = 0; i < tiles_count; i++){
		int j = tiles[i].column;
		j = convertColumn(j);
		count[j]++;
		avg[j] = (avg[j]*2 + tiles[i].magnitude)/2;

//		xil_printf("Tile %d: %d", i, tiles[i].magnitude);

	}
	for(int i = 0; i < 4; i++){
		thresholds[i] = avg[i]/110; // 1/110 of the average
		xil_printf("Threshold %d is %d\r\n", i, thresholds[i]);
	}
	for (int i = 0; i < tiles_count; i++){
		int j = tiles[i].column;
		j = convertColumn(j);
		if (tiles[i].magnitude >= thresholds[j]){
			tiles_converted[i] = j;
		}
		else tiles_converted[i] = 4; // empty
	}
	for(int i = 0; i < 4; i++)
		xil_printf("C%d had %d samples with an average of %d magnitude\r\n", i+1, count[i], avg[i]);
}

void displayTile(int column){
	switch(column){
	case 0:
		xil_printf("--1--------\r\n");
		break;
	case 1:
		xil_printf("----2------\r\n");
		break;
	case 2:
		xil_printf("------3----\r\n");
		break;
	case 3:
		xil_printf("--------4--\r\n");
		break;
	default:
		xil_printf("-----------\r\n");
		break;
	}
}

int convertColumn(int column){
	if (column == 0) return 0;
	if (column == 1023) return 3;
	if (column < 512) return 1;
	return 2;
}
