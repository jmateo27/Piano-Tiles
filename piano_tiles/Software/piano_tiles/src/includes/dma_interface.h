#ifndef DMA_INTERFACE_H
#define DMA_INTERFACE_H

#include <unistd.h>
#include "xparameters.h"
#include "xil_io.h"

#include "xil_util.h"
#include "xiicps.h"

#include "fft_io.h"
#include "interrupts.h"

//------------------------------------------------------------------
// DMA SET UP
#include "xaxidma.h"
#include "xil_exception.h"
#include "xdebug.h"

#define DMA_DEV_ID					XPAR_AXI_DMA_0_DEVICE_ID
#define DDR_BASE_ADDR				XPAR_PS7_DDR_0_S_AXI_BASEADDR
#define MEM_BASE_ADDR				(DDR_BASE_ADDR + 0x1000000)

#define TX_BUFFER_BASE				(MEM_BASE_ADDR + 0x00100000)

#define RESET_TIMEOUT_COUNTER		10000
#define MAX_PKT_LEN					0x800

int setUpDMA(INTC *IntcInstancePtr, XAxiDma *AxiDmaPtr);

int transferData(unsigned char *data, int data_len, XAxiDma *AxiDmaPtr);

#endif // DMA_INTERFACE_H
