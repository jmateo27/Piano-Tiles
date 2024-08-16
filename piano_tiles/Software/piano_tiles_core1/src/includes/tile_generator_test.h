#ifndef TILE_GENERATOR_TEST_H
#define TILE_GENERATOR_TEST_H

//------------------------------------------------------------------
// INCLUDES
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <xil_printf.h>

#include "xparameters.h"
#include "xil_io.h"
#include "xscugic.h"
#include "xil_util.h"
#include "xiicps.h"
#include "xuartps.h"
#include "xpseudo_asm.h"
#include "xil_exception.h"
#include "xil_mmu.h"

#include "../includes/audio.h"

#define COMM_VAL (*(volatile unsigned long *)(0xFFFF0000))
extern u32 MMUTable;

//#define BUTTON_SWITCH_ID XPAR_GPIO_0_DEVICE_ID
#define BUTTON_CHANNEL 1
#define SWITCH_CHANNEL 2

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

void setUpAudio();
void audio_stream(unsigned char *data, int data_len);

#endif
