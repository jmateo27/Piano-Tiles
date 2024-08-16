#include "../includes/dma_interface.h"

int setUpDMA(INTC *IntcInstancePtr, XAxiDma *AxiDmaPtr){
	int Status;
	XAxiDma_Config *Config;

	Config = XAxiDma_LookupConfig(DMA_DEV_ID);
	if (!Config){
		xil_printf("No config found for DMA %d\r\n", DMA_DEV_ID);
		return XST_FAILURE;
	}

	Status = XAxiDma_CfgInitialize(AxiDmaPtr, Config);
	if (Status != XST_SUCCESS) {
		xil_printf("Initialization failed %d\r\n", Status);
		return XST_FAILURE;
	}
	if (XAxiDma_HasSg(AxiDmaPtr)) {
		xil_printf("Device configured as SG mode \r\n");
		return XST_FAILURE;
	}

	return Status;
}

int transferData(unsigned char *data, int data_len, XAxiDma *AxiDmaPtr){
	int Status = XST_SUCCESS;
	int i = 0;
	u32 *TxBufferPtr;
	TxBufferPtr = (u32 *)TX_BUFFER_BASE;
	int flag = 0;
	u16 *ptr = (u16 *)data;
	int samples_len = data_len / sizeof(u16);
	int16_t sample_16b;
	int32_t sample_32b;
	u32 scaled_sample;

	while (i < samples_len){
		for (int j = 0; j < MAX_PKT_LEN; j++, i++){
			if (i == samples_len-1){
				flag = 1;
				break;
			}
			sample_16b = *((int16_t *)ptr);
			sample_32b = ((int32_t)sample_16b) << 6;
			scaled_sample = (uint32_t)sample_32b + 65536;
			TxBufferPtr[j] = (u32)scaled_sample;
			ptr++;
		}
		if (flag) break;
		else{
			Xil_DCacheFlushRange((UINTPTR)TxBufferPtr, MAX_PKT_LEN);

			Status = XAxiDma_SimpleTransfer(AxiDmaPtr, (UINTPTR)TxBufferPtr, MAX_PKT_LEN*4, XAXIDMA_DMA_TO_DEVICE);

			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			usleep(10);
			reset();
			usleep(100);
			unReset();
		}
	}
	return Status;
}
