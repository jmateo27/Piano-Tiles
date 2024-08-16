#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "xscugic.h"
#include "xparameters.h"
#include "xaxidma.h"
#include "xgpio.h"

#define TX_INTR_ID					XPAR_FABRIC_AXI_DMA_0_MM2S_INTROUT_INTR
#define TILEDONE_INTR_ID			62U
#define BUTTON_INTR_ID				63U

#define INTC_DEVICE_ID				XPAR_SCUGIC_0_DEVICE_ID
#define INTC						XScuGic
#define INTC_HANDLER				XScuGic_InterruptHandler

#define	BTNS_DEVICE_ID				XPAR_PS7_GPIO_0_DEVICE_ID
#define BTN_INT 					XGPIO_IR_CH1_MASK

int initializeIntrSystem(INTC *IntcInstancePtr);

int Setup_IntrSystem(INTC *IntcInstancePtr, void *DeviceInstancePtr, u16 IntrId, u8 priority, void IntrHandler(void *));

int SetupDMAIntrSystem(INTC *IntcInstancePtr, XAxiDma *AxiDmaPtr, void (*func)(void *));
int SetupTileIntrSystem(INTC *IntcInstancePtrvoid, void TileIntrHandler(void *));
int SetupBtnIntrSystem(INTC *IntcInstancePtr, XGpio *GpioPtr, void BtnIntrHandler(void *));

void enableInterrupts(INTC *IntcInstancePtr, XGpio *GpioPtr);

int SetupIntrSystem(INTC *IntcInstancePtr, XAxiDma *AxiDmaPtr, XGpio *GpioPtr, void TileIntrHandler(void*)
																			 , void TxIntrHandler(void*)
																			 , void BtnIntrHandler(void *));

void DisableIntrSystem(INTC *IntcInstancePtr);

#endif
