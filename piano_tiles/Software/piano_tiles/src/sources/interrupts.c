#include "../includes/interrupts.h"

int initializeIntrSystem(INTC *IntcInstancePtr){
	int Status;
	XScuGic_Config *IntcConfig;
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig, IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return Status;
}

int Setup_IntrSystem(INTC *IntcInstancePtr, void *DeviceInstancePtr, u16 IntrId, u8 priority, void IntrHandler(void *)){
	int Status;

	XScuGic_SetPriorityTriggerType(IntcInstancePtr, IntrId, priority, 0x3);

	Status = XScuGic_Connect(IntcInstancePtr, IntrId, (Xil_InterruptHandler)IntrHandler, (void *)DeviceInstancePtr);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	XScuGic_Enable(IntcInstancePtr, IntrId);
	return Status;
}

int SetupDMAIntrSystem(INTC *IntcInstancePtr, XAxiDma *AxiDmaPtr, void (*func)(void *)){
	return Setup_IntrSystem(IntcInstancePtr, AxiDmaPtr, TX_INTR_ID, 0xA0, func);
}
int SetupTileIntrSystem(INTC *IntcInstancePtr, void TileIntrHandler(void *)){
	return Setup_IntrSystem(IntcInstancePtr, IntcInstancePtr, TILEDONE_INTR_ID, 0xA8, TileIntrHandler);
}
int SetupBtnIntrSystem(INTC *IntcInstancePtr, XGpio *GpioPtr, void BtnIntrHandler(void *)){
	int Status;

	//XScuGic_SetPriorityTriggerType(IntcInstancePtr, BUTTON_INTR_ID, 0xA8, 0x3);

	Status = XScuGic_Connect(IntcInstancePtr, BUTTON_INTR_ID, (Xil_InterruptHandler)BtnIntrHandler, (void *)GpioPtr);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	XGpio_InterruptEnable(GpioPtr, 1);
	XGpio_InterruptGlobalEnable(GpioPtr);

	XScuGic_Enable(IntcInstancePtr, BUTTON_INTR_ID);
	return Status;

}

void enableInterrupts(INTC *IntcInstancePtr, XGpio *GpioPtr){
	// Enable interrupt
	XGpio_InterruptEnable(GpioPtr, BTN_INT);
	XGpio_InterruptGlobalEnable(GpioPtr);
	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)INTC_HANDLER, (void *)IntcInstancePtr);

	Xil_ExceptionEnable();
}

int SetupIntrSystem(INTC *IntcInstancePtr, XAxiDma *AxiDmaPtr, XGpio *GpioPtr, void TileIntrHandler(void*)
																			 , void TxIntrHandler(void*)
																			 , void BtnIntrHandler(void *)){
	int Status;
	Status = initializeIntrSystem(IntcInstancePtr);
	if (Status == XST_FAILURE) xil_printf("Failed to initialize INTR system\r\n");

	enableInterrupts(IntcInstancePtr, GpioPtr);

	Status = SetupTileIntrSystem(IntcInstancePtr, TileIntrHandler);
	if (Status == XST_FAILURE) xil_printf("Failed to set up Tile Interrupt\r\n");

	Status = SetupDMAIntrSystem(IntcInstancePtr, AxiDmaPtr, TxIntrHandler);
	if (Status != XST_SUCCESS) xil_printf("Failed to set up DMA INTR system\r\n");

	Status = SetupBtnIntrSystem(IntcInstancePtr, GpioPtr, BtnIntrHandler);
	if (Status != XST_SUCCESS) xil_printf("Failed to set up Button INTR system\r\n");

	return Status;
}

void DisableIntrSystem(INTC *IntcInstancePtr){
	XScuGic_Disconnect(IntcInstancePtr, TX_INTR_ID);
	XScuGic_Disconnect(IntcInstancePtr, TILEDONE_INTR_ID);
	XScuGic_Disconnect(IntcInstancePtr, BUTTON_INTR_ID);

}
