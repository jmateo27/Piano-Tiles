#ifndef MENU_H_
#define MENU_H_

/* ---------------------------------------------------------------------------- *
 * 								Header Files									*
 * ---------------------------------------------------------------------------- */
#include <stdio.h>
#include "xil_types.h"
#include "xparameters.h"
#include "xil_io.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "xuartps.h"
#include <unistd.h>  // Include for usleep function
#include "xil_cache.h"

#include "stdlib.h"
#include "stdbool.h"

#define UART_BASEADDR 			XPAR_PS7_UART_1_BASEADDR
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 1024
#define TILE_HEIGHT 320
#define TILE_WIDTH 160
#define NUM_COLUMNS 4
#define DELAY_BETWEEN_COLUMNS_US 5000000 // Delay between each column in microseconds (adjust as needed)
#define BACKGROUND_COLOUR 0x536167
#define TILE_COLOUR 0xFFFFFF
#define SPEED 8
#define MISSED_POINTS -5

#define MAINMENU_PTR		0x020BB00C
#define SONGMENU_PTR 		0x028A4010
#define TILE_PTR			0x0308D014
#define GAMEMENU_PTR		0x03876018
#define SCOREMENU_PTR		0x0405F01C
#define HELPMENU_PTR		0x04848020
#define MENU_NUM_BYTES_BUFFER	5542880
#define COMM_VAL (*(volatile unsigned long *)(0xFFFF0000))

/* ---------------------------------------------------------------------------- *
 * 							Initialize Structs									*
 * ---------------------------------------------------------------------------- */
typedef struct {
    int x; // X-coordinate of the sprite
    int y; // Y-coordinate of the sprite
    int speed; // Speed of the falling sprite
    bool active; // Flag to indicate if the sprite is active
    bool hit; // Flat to indicate if the sprite was hit while active
} Tile;
/* ---------------------------------------------------------------------------- *
 * 							Prototype Functions									*
 * ---------------------------------------------------------------------------- */
void uart_input(int* image_buffer_pointer);
//void DrawMenu(int *image_buffer_pointer);
void DrawSongSelect(int* image_buffer_pointer);
void DrawGamePage(int* image_buffer_pointer);
void FallingTiles(int* image_buffer_pointer);
void FallingTilesControlled(int* image_buffer_pointer);
void ClearTile(int* framebuffer, int x, int y);
void DrawTile(int* framebuffer, int x, int y);
bool IsTileAtBottom(int tile_y);

void goToMainMenu(int* image_buffer_pointer, int *mode);
void goToSongMenu(int* image_buffer_pointer, int *mode);
void goToGameMenu(int* image_buffer_pointer, int *mode, int *exit, int *isPlaying, int *tiles, int tiles_count, Tile *sprites, int *cur_score);

void drawMainMenu(int* image_buffer_pointer);
void drawSongMenu(int* image_buffer_pointer);
void drawGameMenu(int* image_buffer_pointer);
void drawScoreMenu(int* image_buffer_pointer);

/* ---------------------------------------------------------------------------- *
 * 							Screen Information									*
 * ---------------------------------------------------------------------------- */
// Screen width / height
#define WIDTH				1280
#define HEIGHT				1024




#endif /* MENU_H_*/
