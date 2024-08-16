#include "../includes/menu.h"

const int SQUARE_SIDE = 32;
const int diff_EZ = 8; // 8 by 8 blocks
int song_selected = '1';

void uart_input(int* image_buffer_pointer) {
//	DrawMenu(image_buffer_pointer);
	xil_printf("Drew Menu\r\n");

	u8 inp = 0x00;
	u32 CntrlRegister;
	xil_printf("Reading UART register\r\n");
	CntrlRegister = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_CR_OFFSET);
	xil_printf("Writing to UART register\r\n");
	XUartPs_WriteReg(UART_BASEADDR, XUARTPS_CR_OFFSET,
			((CntrlRegister & ~XUARTPS_CR_EN_DIS_MASK) | XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN));
	xil_printf("\r\n\r\n");
	xil_printf("Piano Tiles Menu\r\n");
	xil_printf("Enter 1 for Quick Start\r\n");
	xil_printf("Enter 2 for Song Selection\r\n");
	xil_printf("Enter 3 for Falling Tiles\r\n");
	xil_printf("Enter 4 for Generating Falling Tiles\r\n");
	xil_printf("----------------------------------------\r\n");

	while (!XUartPs_IsReceiveData(UART_BASEADDR));

	inp = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
	// Select function based on UART input
	switch(inp) {
	case '1':
		song_selected='1';
		xil_printf("Starting Game\r\n");
		DrawGamePage(image_buffer_pointer);
		break;
	case '2':
		xil_printf("Song Selection\r\n");
		DrawSongSelect(image_buffer_pointer);
		break;
	case '3':
		xil_printf("Falling Tiles\r\n");
		FallingTiles(image_buffer_pointer);
		break;
	case '4':
		xil_printf("Falling Tiles Controlled\r\n");
		FallingTilesControlled(image_buffer_pointer);
		break;
	default:
		uart_input(image_buffer_pointer);
		break;
	} // switch
}

void InitTile(Tile *sprite, int x, int speed) {
    sprite->x = x;
    sprite->y = 0;
    sprite->speed = speed;
    sprite->active = true; // Start with the sprite inactive
    sprite->hit = false;
}

void ClearTile(int* framebuffer, int x, int y) {
    // Calculate the offset in the framebuffer for the sprite position
	int offset = (y * SCREEN_WIDTH) + x;

	for(int i=0; i<SPEED; i++){
		memset((void*)(framebuffer + offset + (i*SCREEN_WIDTH)), BACKGROUND_COLOUR, TILE_WIDTH * sizeof(int));
	}

}

void DrawTile(int* framebuffer, int x, int y) {
	int *tile_pointer = (int *)0x0308D014;
	// Calculate the offset in the framebuffer for the sprite position
	int offset = (y * SCREEN_WIDTH) + x;

	// Draw the pixels of the sprite
	for (int i = 0; i < TILE_HEIGHT; i++) {
		// Calculate the address in the framebuffer for the current row
        int row_address = offset + (i * SCREEN_WIDTH);
		// Draw the pixels for the current row
		memcpy((void*)(framebuffer + row_address), tile_pointer+i, TILE_WIDTH * sizeof(int));
	}
}

//// Function accepts the tile location for column and determines the points earned
//int ClickedPoints(int tile_y, Tile* sprite){ // this case is for when tile is clicked
//	sprite->hit = true;
//	if (tile_y<SCREEN_HEIGHT-TILE_HEIGHT){
//		return -2;
//	}
//	float percent = (((SCREEN_HEIGHT-tile_y)*1.0)/(TILE_HEIGHT*1.0));
//	if (percent < 0.2){
//		return 1;
//	}else if (percent < 0.4){
//		return 2;
//	}else if (percent < 0.6){
//		return 3;
//	}else if (percent < 0.8){
//		return 4;
//	}else{
//		return 5;
//	}
//}

// Function to check if a tile has hit the bottom of the screen
bool IsTileAtBottom(int tile_y) {
    // Adjust this condition according to your screen resolution
//    return tile_y + TILE_HEIGHT >= SCREEN_HEIGHT; // this will stop the tile once the bottom touches the end
    return tile_y >= SCREEN_HEIGHT; // stops once the top tile falls off the bottom
}

void FallingTilesControlled(int* image_buffer_pointer){
	// Show Game Page
	int *image1_pointer = (int *)0x03876018;
	int NUM_BYTES_BUFFER = 5542880;
	memcpy(image_buffer_pointer, image1_pointer, NUM_BYTES_BUFFER);

	// Initialize Tiles
    Tile sprites[NUM_COLUMNS];
    for (int i = 0; i < NUM_COLUMNS; ++i) {
        InitTile(&sprites[i], (i * (TILE_WIDTH+32)) + 275,SPEED); // Adjust the speed as needed
    }

    int cur_score = 0;

    xil_printf("Enter the column to start\n");
	while (!XUartPs_IsReceiveData(UART_BASEADDR));
    bool allTilesInactive = false;
	while (!allTilesInactive) {
		char inp = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
		for (int i = 0; i < NUM_COLUMNS; ++i) {
			if (sprites[i].active) {
				// Clear previous position of the sprite
				ClearTile(image_buffer_pointer, sprites[i].x, sprites[i].y);

				// Update the position of the sprite (falling down)
				sprites[i].y += sprites[i].speed;

				// Draw the sprite at its new position
				DrawTile(image_buffer_pointer, sprites[i].x, sprites[i].y);

				// Check if the sprite has hit the bottom
				if (!IsTileAtBottom(sprites[i].y)) {
					allTilesInactive = false; // At least one sprite is still active
				} else {
					// Tile has hit the bottom, deactivate it
					ClearTile(image_buffer_pointer, sprites[i].x, sprites[i].y);
					sprites[i].active = false;
					sprites[i].y = 0; // Reset sprite position for next iteration
					if (sprites[i].hit == false){
						cur_score += MISSED_POINTS;
					}else{
						sprites[i].hit = false;
					}
				}
				if (inp=='c' && !sprites[i].hit){
					int ret = ClickedPoints(sprites[i].y, &sprites[i]);
					cur_score += ret;
					xil_printf("%d\r\n",cur_score);
				}
			}

		}
		if(inp <= '3' && inp >= '0'){
			sprites[inp-'0'].active = true;
		}
		if(inp == 'q'){
			uart_input(image_buffer_pointer);
		}
	}
	uart_input(image_buffer_pointer);
}

void FallingTiles(int* image_buffer_pointer){
	// Show Game Page
	int *image1_pointer = (int *)0x03876018;
	int NUM_BYTES_BUFFER = 5542880;
	memcpy(image_buffer_pointer, image1_pointer, NUM_BYTES_BUFFER);

	// Initialize Tiles
    Tile sprites[NUM_COLUMNS];
    for (int i = 0; i < NUM_COLUMNS; ++i) {
        InitTile(&sprites[i], (i * (TILE_WIDTH+32)) + 275, SPEED); // Adjust the speed as needed
    }

    bool allTilesInactive = false;
    while (!allTilesInactive) {
        for (int i = 0; i < NUM_COLUMNS; ++i) {
            if (!sprites[i].active) {
                // Activate the sprite if it's not active
                sprites[i].active = true;
            } else if (sprites[i].active) {
                // Clear previous position of the sprite
                ClearTile(image_buffer_pointer, sprites[i].x, sprites[i].y);

                // Update the position of the sprite (falling down)
                sprites[i].y += sprites[i].speed;

                // Draw the sprite at its new position
                DrawTile(image_buffer_pointer, sprites[i].x, sprites[i].y);

                // Check if the sprite has hit the bottom
                if (!IsTileAtBottom(sprites[i].y)) {
                    allTilesInactive = false; // At least one sprite is still active
                } else {
                    // Tile has hit the bottom, deactivate it
                	ClearTile(image_buffer_pointer, sprites[i].x, sprites[i].y);
                    sprites[i].active = false;
                    sprites[i].y = 0; // Reset sprite position for next iteration
                }
            }
        }
        if(XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET) == 'q'){
        		uart_input(image_buffer_pointer);
        	}
    }
}

void DrawSongSelect(int* image_buffer_pointer) {
	drawSongMenu(image_buffer_pointer);

	xil_printf("Song Options:\r\n");
	xil_printf("Type 1-3 for corresponding song\r\n");
	xil_printf("Type 'q' to go back to background\r\n");
	while (!XUartPs_IsReceiveData(UART_BASEADDR));
	char sel = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
	if(sel == 'q'){
		uart_input(image_buffer_pointer);
	}else if (sel == '1' || sel == '2' || sel == '3'){
		song_selected = sel;
		DrawGamePage(image_buffer_pointer);
	}else{
		DrawSongSelect(image_buffer_pointer);
	}
}

void DrawGamePage(int* image_buffer_pointer) {
	drawGameMenu(image_buffer_pointer);

	while (!XUartPs_IsReceiveData(UART_BASEADDR));
	xil_printf("Type 'q' to go back to background\r\n");
	if(XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET) == 'q'){
		uart_input(image_buffer_pointer);
	} else {
		DrawGamePage(image_buffer_pointer);
	}
}

void goToMainMenu(int* image_buffer_pointer, int *mode){
	drawMainMenu(image_buffer_pointer);
	*mode = 1;
}

void goToSongMenu(int* image_buffer_pointer, int *mode){
	drawSongMenu(image_buffer_pointer);
	*mode = 2;
}

void goToGameMenu(int* image_buffer_pointer, int *mode, int *exit, int *isPlaying, int *tiles, int tiles_count, Tile *sprites, int *cur_score){
	drawGameMenu(image_buffer_pointer);
	*mode = 3;

	// analyze tiles

	xil_printf("%d tiles to play\r\n", tiles_count);
	int num_tiles = tiles_count;
	// Initialize Tiles
//	Tile sprites[NUM_COLUMNS];
	for (int i = 0; i < NUM_COLUMNS; ++i) {
		InitTile(&sprites[i], (i * (TILE_WIDTH+32)) + 275,SPEED); // Adjust the speed as needed
	}

	*cur_score = 0;
	int timer = 0;

	xil_printf("Wait 2 seconds to start\n");
	sleep(2);
	// generate tiles per object
	while (!(*exit) && num_tiles>0) {
//		xil_printf("tile#%d\r\n", tiles_count-num_tiles);
//		char inp = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
		if (COMM_VAL == 0) break;
		for (int i = 0; i < NUM_COLUMNS; ++i) {
			if (sprites[i].active) {
				// Clear previous position of the sprite
				ClearTile(image_buffer_pointer, sprites[i].x, sprites[i].y);

				// Update the position of the sprite (falling down)
				sprites[i].y += sprites[i].speed;

				// Draw the sprite at its new position
				DrawTile(image_buffer_pointer, sprites[i].x, sprites[i].y);

				// Check if the sprite has hit the bottom
				if (IsTileAtBottom(sprites[i].y)) {
					if (sprites[i].hit == false){
						*cur_score += MISSED_POINTS;
					}else{
						sprites[i].hit = false;
					}
					// Tile has hit the bottom, deactivate it
					ClearTile(image_buffer_pointer, sprites[i].x, sprites[i].y);
					sprites[i].active = false;
					sprites[i].y = 0; // Reset sprite position for next iteration
				}
			}
		}
		if (timer >= SCREEN_HEIGHT/SPEED/4){ // create the next sprite
			if(tiles[tiles_count-num_tiles] <=3  && tiles[tiles_count-num_tiles] >= 0){
				sprites[tiles[tiles_count-num_tiles]].active = true;
			}
			num_tiles--;
			timer = 0;
		}
		timer++;
//
//		if(tiles[tiles_count-num_tiles] <=3  && tiles[tiles_count-num_tiles] >= 0){
//			sprites[tiles[tiles_count-num_tiles]].active = true;
//		}


	}
	xil_printf("Back to main menu\r\n");
	*isPlaying = 0;

//	uart_input(image_buffer_pointer);
}



void drawMainMenu(int* image_buffer_pointer){
	memcpy((int *)image_buffer_pointer, (int *)MAINMENU_PTR, MENU_NUM_BYTES_BUFFER);
}
void drawSongMenu(int* image_buffer_pointer){
	memcpy((int *)image_buffer_pointer, (int *)SONGMENU_PTR, MENU_NUM_BYTES_BUFFER);
}
void drawGameMenu(int* image_buffer_pointer){
	memcpy((int *)image_buffer_pointer, (int *)GAMEMENU_PTR, MENU_NUM_BYTES_BUFFER);
}
void drawScoreMenu(int* image_buffer_pointer){
	memcpy((int *)image_buffer_pointer, (int *)SCOREMENU_PTR, MENU_NUM_BYTES_BUFFER);
}
void drawHelpMenu(int* image_buffer_pointer){
	memcpy((int *)image_buffer_pointer, (int *)HELPMENU_PTR, MENU_NUM_BYTES_BUFFER);
}
