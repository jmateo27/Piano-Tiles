#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include "xil_types.h"
#include "xparameters.h"

#include "xil_io.h"
#include "xil_exception.h"
#include "xscugic.h"
#include <stdlib.h>
#include <stdbool.h>
#include "xil_cache.h"
#include "xuartps.h"
#include "xpseudo_asm.h"
#include "xil_mmu.h"

#include "xil_printf.h"
#include <unistd.h>

#include "fft_io.h"
#include "dma_interface.h"
#include "tiles.h"
#include "interrupts.h"
#include "display.h"
#include "numbers.h"
#include "menu.h"

#define sev() __asm__("sev")
#define ARM1_STARTADR 0xFFFFFFF0
#define ARM1_BASEADDR 0x10080000
//#define COMM_VAL (*(volatile unsigned long *)(0xFFFF0000))

#define BLUE    			0xFF0000
#define RED					0x0000FF
#define PURPLE				0xFF00FF
#define GREEN				0x00FF00
#define CYAN				0xFFFF00
#define BAR_WIDTH			80
#define SQUARE_SIDE			32
int *image_buffer_pointer = (int *)(0x00900000);


#define UART_BASEADDR XPAR_PS7_UART_1_BASEADDR



extern unsigned char song[];
extern int song_size;
extern unsigned char piano[];
extern int piano_size;
extern unsigned char chime[];
extern int chime_size;
extern unsigned char drum[];
extern int drum_size;
extern unsigned char boing[];
extern int boing_size;

// Sprite structure
typedef struct {
    int index; // Position
    unsigned int *color;// Color
} Sprite;

XScuGic INTCInst;
int btn_value;

int score;

unsigned int COLOR_ARRAY[] = {(unsigned int)BLUE, (unsigned int)RED, (unsigned int)PURPLE, (unsigned int)GREEN, (unsigned int)CYAN};

void drawVerticalLines();
void DrawSprite(Sprite sprite);
void drawScore(int num);
void DrawHighScoreSprite(Sprite sprite, int songNum);
void DrawFinalScoreSprite(Sprite sprite, int songNum);
void drawHighScores(int *scores);

void drawFinalScore();

void menu();
void gameMenu();
void songMenu();
int processSong(unsigned char *song, int song_len);
int ClickedPoints(int tile_y, Tile* sprite);

void resetMain();
void resetTiles();
int setUpHW(INTC *IntcInstancePtr, XAxiDma *AxiDmaPtr);
void resetHW();

void TxIntrHandler(void *Callback);
void TileIntrHandler(void *Callback);
void BTN_Intr_Handler(void *InstancePtr);

#endif
