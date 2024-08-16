#include "includes/main.h"

// DATA
int song_count[4] = {0};
int song_avg[4] = {0};
int piano_count[4] = {0};
int piano_avg[4] = {0};
int drum_count[4] = {0};
int drum_avg[4] = {0};
int highscores[3] = {0};
int cur_score;
int cur_song;

XAxiDma AxiDma;

XGpio BTNInst;
int btn_value;

static INTC Intc;

TileInfo song_tiles[MAX_TILE_ARRAY_LEN];
TileInfo piano_tiles[MAX_TILE_ARRAY_LEN];
TileInfo drum_tiles[MAX_TILE_ARRAY_LEN];
int song_tiles_converted[MAX_TILE_ARRAY_LEN];
int piano_tiles_converted[MAX_TILE_ARRAY_LEN];
int drum_tiles_converted[MAX_TILE_ARRAY_LEN];

Tile sprites[NUM_COLUMNS];
int song_tiles_count;
int piano_tiles_count;
int drum_tiles_count;

int isPlaying;
int isExit;
int done;
int btn_pressed;
int mode;
// 0: Nothing
// 1: Main Menu
// 2: Song Menu
// 3: Game
// 4: Score Page
// 5: Help Menu

int main()
{
	COMM_VAL = 0;

	Xil_SetTlbAttributes(0xFFFF0000,0x14de2);
	Xil_Out32(ARM1_STARTADR, ARM1_BASEADDR);
	dmb(); //waits until write has finished

	sev();

	print("Hello World - ARM0\n\r");
	sleep(1);
	COMM_VAL = 1;
	while(COMM_VAL == 1); // wait until core 1 replies

	sleep(1);

	int Status;

	resetMain();

	Status = XGpio_Initialize(&BTNInst, BTNS_DEVICE_ID);
	if(Status != XST_SUCCESS) return XST_FAILURE;
	// Set all buttons direction to inputs
	XGpio_SetDataDirection(&BTNInst, 1, 0xFF);

	Status = SetupIntrSystem(&Intc, &AxiDma, &BTNInst, TileIntrHandler
													, TxIntrHandler
													, BTN_Intr_Handler);
	if (Status == XST_FAILURE){
			xil_printf("Failed to set up interrupt system\r\n");
			return Status;
	}
	// Song processing

	Status = setUpHW(&Intc, &AxiDma);
	if (Status == XST_FAILURE){
		xil_printf("Failed to set up hardware\r\n");
		return Status;
	}

	cur_song = 0;
	Status = processSong(song, song_size);
	if (Status == XST_FAILURE){
		xil_printf("Failed to create tiles\r\n");
		return Status;
	}

	resetHW();

	cur_song = 1;
	Status = processSong(piano, piano_size);
	if (Status == XST_FAILURE){
		xil_printf("Failed to create tiles\r\n");
		return Status;
	}

	resetHW();

	cur_song = 2;
	Status = processSong(drum, drum_size);
	if (Status == XST_FAILURE){
		xil_printf("Failed to create tiles\r\n");
		return Status;
	}

	resetHW();
	sleep(1);


	displayTileSummary(song_tiles, song_tiles_count, song_count, song_avg, song_tiles_converted);
	displayTileSummary(piano_tiles, piano_tiles_count, piano_count, piano_avg, piano_tiles_converted);
	displayTileSummary(drum_tiles, drum_tiles_count, drum_count, drum_avg, drum_tiles_converted);

	//


	Xil_DCacheDisable();

	goToMainMenu(image_buffer_pointer, &mode);
	while(!done){
		if (COMM_VAL == 0) isPlaying = 0;
		if (!isExit && isPlaying){
			mode = 3;
			switch(cur_song){
			case 0:
				goToGameMenu(image_buffer_pointer, &mode, &isExit, &isPlaying, song_tiles_converted, song_tiles_count, sprites, &cur_score);
				break;
			case 1:
				goToGameMenu(image_buffer_pointer, &mode, &isExit, &isPlaying, piano_tiles_converted, piano_tiles_count, sprites, &cur_score);
				break;
			case 2:
				goToGameMenu(image_buffer_pointer, &mode, &isExit, &isPlaying, drum_tiles_converted, drum_tiles_count, sprites, &cur_score);
				break;
			default:

			}
			mode = 4;
			drawScoreMenu(image_buffer_pointer);
			if (cur_score > highscores[cur_song]) highscores[cur_song] = cur_score;
			if (cur_score < 0) cur_score = 0;
			drawFinalScore();
			sleep(7);
			goToMainMenu(image_buffer_pointer, &mode);
		}
	}
	xil_printf("Done\r\n");

	DisableIntrSystem(&Intc);

	return 0;
}

// Function accepts the tile location for column and determines the points earned
int ClickedPoints(int tile_y, Tile* sprite){ // this case is for when tile is clicked
	sprite->hit = true;
	if (tile_y<SCREEN_HEIGHT-TILE_HEIGHT){
		return -5;
	}
	float percent = (((SCREEN_HEIGHT-tile_y)*1.0)/(TILE_HEIGHT*1.0));
	if (percent < 0.2){
		return 1*10;
	}else if (percent < 0.4){
		return 2*10;
	}else if (percent < 0.6){
		return 3*10;
	}else if (percent < 0.8){
		return 4*10;
	}else{
		return 5*10;
	}
}

int processSong(unsigned char *song, int song_len){
//	resetHW();
//	sleep(1);

	int Status;
	Status = setUpHW(&Intc, &AxiDma);
	Status = transferData(song, song_len, &AxiDma);
	return Status;
}

void menu(){
	u8 inp = 0x00;
	u32 CntrlRegister;

	/* Turn off all LEDs */

	CntrlRegister = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_CR_OFFSET);

	XUartPs_WriteReg(UART_BASEADDR, XUARTPS_CR_OFFSET,
				  ((CntrlRegister & ~XUARTPS_CR_EN_DIS_MASK) |
				   XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN));

	xil_printf("\r\n\r\n");
	xil_printf("Tile Generation Demo\r\n");
	xil_printf("Enter '1' to go to song menu, '2' to go to game menu.\r\n");
	xil_printf("----------------------------------------\r\n");

	// Wait for input from UART via the terminal
	while (!XUartPs_IsReceiveData(UART_BASEADDR));
	inp = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
	// Select function based on UART input
	switch(inp){
	case '1':
		xil_printf("DISPLAYING SONG MENU\r\n");
		xil_printf("Press 'q' to return to the main menu\r\n");
		songMenu();
		break;
	case '2':
		xil_printf("DISPLAYING GAME MENU\r\n");
		xil_printf("Press 'q' to return to the main menu\r\n");
		gameMenu();
		break;
	default:
		break;
	} // switch

	sleep(1);
	menu();
}


void gameMenu(){
	u8 inp = 0x00;
	u32 CntrlRegister;

	/* Turn off all LEDs */

	CntrlRegister = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_CR_OFFSET);

	XUartPs_WriteReg(UART_BASEADDR, XUARTPS_CR_OFFSET,
				  ((CntrlRegister & ~XUARTPS_CR_EN_DIS_MASK) |
				   XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN));

	memcpy((int *)image_buffer_pointer, (int *)GAMEMENU_PTR, MENU_NUM_BYTES_BUFFER);

	score = 0;

	while(1){
		if (!XUartPs_IsReceiveData(UART_BASEADDR)) inp = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
		if (inp == 'q') break;
		drawScore(score);
		score++;
		if (score == 999999) score = 0;
	}
}

void songMenu(){
	memcpy((int *)image_buffer_pointer, (int *)SONGMENU_PTR, MENU_NUM_BYTES_BUFFER);

	int testScores[] = {90368, 35090, 1452};

	drawHighScores(testScores);

}

void drawVerticalLines(){
	for (int bar = 0; bar < 5; ++bar) {
		// Calculate the start and end positions of the current bar
		int startX = bar * WIDTH / 5;
		int endX = (bar + 1) * WIDTH / 5;

		// Loop over the pixels in the current bar
		for (int y = 0; y < HEIGHT; ++y) {
			for (int x = startX - 1; x < endX; ++x) {
				*((int *)((int *)image_buffer_pointer + y * WIDTH + x)) = COLOR_ARRAY[bar];
			}
		}
	}
}

void DrawScoreSprite(Sprite sprite){
    int i = sprite.index % 32;
    int j = (sprite.index / 32) + 1;

    int posX = i*(WIDTH / 32 - 13) - (SQUARE_SIDE / 2) + WIDTH - 200;
    int posY = j*(HEIGHT / 24) - (SQUARE_SIDE / 2) + 64;

	// Draw a Sprite- pixel by pixel at given position
    for (int y = 0; y < SQUARE_SIDE; ++y) {
        for (int x = 0; x < SQUARE_SIDE; ++x) {
            int pixel_color = sprite.color[y*SQUARE_SIDE+x]; // Get sprite color
            if (pixel_color >= 0x7F7F7F) pixel_color = 0x676153; // change later for background
            else pixel_color = 0xFFFFFF;
            // Draw sprite pixel at its position

            *((int *)((int *)image_buffer_pointer + (posY + y) * WIDTH + (posX + x))) = pixel_color;
        }
    }
}

void drawScore(int num){
	int numDigits = 6;
	int placeValue[numDigits]; // 6 digits
	int temp = num;
	int place = 100000;

	for (int i = 0; i < numDigits; i++){
		placeValue[i] = temp/place;
		temp -= placeValue[i]*place;
		place /= 10;
		Sprite num_sprite = {i, (unsigned int *)digitBlock[placeValue[i]]};
		DrawScoreSprite(num_sprite);
	}
}

void DrawHighScoreSprite(Sprite sprite, int songNum){
    int i = sprite.index % 32;
    int j = (sprite.index / 32) + 1;

    int posX = i*(WIDTH / 32 - 13) - (SQUARE_SIDE / 2) + WIDTH - 525 ;
    int posY = j*(HEIGHT / 24) - (SQUARE_SIDE / 2) + 388 + 59*songNum;

	// Draw a Sprite- pixel by pixel at given position
    for (int y = 0; y < SQUARE_SIDE; ++y) {
        for (int x = 0; x < SQUARE_SIDE; ++x) {
            int pixel_color = sprite.color[y*SQUARE_SIDE+x]; // Get sprite color
            if (pixel_color >= 0x7F7F7F) pixel_color = 0x676153; // change later for background
            else pixel_color = 0xFFFFFF;
            // Draw sprite pixel at its position

            *((int *)((int *)image_buffer_pointer + (posY + y) * WIDTH + (posX + x))) = pixel_color;
        }
    }
}

void DrawFinalScoreSprite(Sprite sprite, int songNum){
    int i = sprite.index % 32;
    int j = (sprite.index / 32) + 1;

    int posX = i*(WIDTH / 32 - 13) - (SQUARE_SIDE / 2) + WIDTH - 525 ;
    int posY = j*(HEIGHT / 24) - (SQUARE_SIDE / 2) + 332 + 59*2*songNum;

	// Draw a Sprite- pixel by pixel at given position
    for (int y = 0; y < SQUARE_SIDE; ++y) {
        for (int x = 0; x < SQUARE_SIDE; ++x) {
            int pixel_color = sprite.color[y*SQUARE_SIDE+x]; // Get sprite color
            if (pixel_color >= 0x7F7F7F) pixel_color = 0x676153; // change later for background
            else pixel_color = 0xFFFFFF;
            // Draw sprite pixel at its position

            *((int *)((int *)image_buffer_pointer + (posY + y) * WIDTH + (posX + x))) = pixel_color;
        }
    }
}

void drawHighScores(int *scores){
	int numNums = 3;
	int numDigits = 6;
	int placeValue[numDigits]; // 6 digits
	int temp;
	int place;

	for (int j = 0; j < numNums; j++){
		temp = scores[j];
		place = 100000;
		for (int i = 0; i < numDigits; i++){
			placeValue[i] = temp/place;
			temp -= placeValue[i]*place;
			place /= 10;
			Sprite num_sprite = {i, (unsigned int *)digitBlock[placeValue[i]]};
			DrawHighScoreSprite(num_sprite, j);
		}
	}
}

void drawFinalScore(){
	int numNums = 2;
	int nums[] = {cur_score, highscores[cur_song]};
	int numDigits = 6;
	int placeValue[numDigits]; // 6 digits
	int temp;
	int place;

	for (int j = 0; j < numNums; j++){
		temp = nums[j];
		place = 100000;
		for (int i = 0; i < numDigits; i++){
			placeValue[i] = temp/place;
			temp -= placeValue[i]*place;
			place /= 10;
			Sprite num_sprite = {i, (unsigned int *)digitBlock[placeValue[i]]};
			DrawFinalScoreSprite(num_sprite, j);
		}
	}
}

void resetMain(){
	cur_song = 0;
	isPlaying = 0;
	isExit = 0;
	done = 0;
	btn_pressed = 0;
	mode = 1;
	resetTiles();
	resetHW();
}

void resetTiles(){
	song_tiles_count = 0;
	piano_tiles_count = 0;
	drum_tiles_count = 0;
}

int setUpHW(INTC *IntcInstancePtr, XAxiDma *AxiDmaPtr){
	int Status;

	Status = setUpDMA(IntcInstancePtr, AxiDmaPtr);
	if (Status == XST_FAILURE) xil_printf("Failed to set up DMA\r\n");


	return Status;
}

void resetHW(){
	reset();
	usleep(100);
	unReset();
}

void TxIntrHandler(void *Callback){
	u32 IrqStatus;
	int TimeOut;
	XAxiDma *AxiDmaInst = (XAxiDma *)Callback;
	IrqStatus = XAxiDma_IntrGetIrq(AxiDmaInst, XAXIDMA_DMA_TO_DEVICE);

	XAxiDma_IntrAckIrq(AxiDmaInst, IrqStatus, XAXIDMA_DMA_TO_DEVICE);

	if (!(IrqStatus & XAXIDMA_IRQ_ALL_MASK)) {
		return;
	}

	if ((IrqStatus & XAXIDMA_IRQ_ERROR_MASK)) {

//		Error = 1;
		xil_printf("Error TX\r\n");
		XAxiDma_Reset(AxiDmaInst);
		TimeOut = RESET_TIMEOUT_COUNTER;

		while (TimeOut) {
			if (XAxiDma_ResetIsDone(AxiDmaInst)) {
				break;
			}
			TimeOut -= 1;
		}
		return;
	}

	if ((IrqStatus & XAXIDMA_IRQ_IOC_MASK)) {
		//TxDone = 1;
		xil_printf("Transfer done for sure\r\n");
	}
}

void TileIntrHandler (void *Callback){
	int count;
	if (cur_song == 0) count = song_tiles_count;
	else if (cur_song == 1) count = piano_tiles_count;
	else if (cur_song == 2) count = drum_tiles_count;

	if (count == MAX_TILE_ARRAY_LEN){
		return;
	}

	u32 out = readR0();

	u32 magnitude = out & MAGNITUDE_MASK;
	int column = (out & COLUMN_MASK) >> 22;
	switch(cur_song){
	case 0:
		song_tiles[song_tiles_count].magnitude = magnitude;
		song_tiles[song_tiles_count].column = column;

		song_tiles_count++;
	case 1:
		piano_tiles[piano_tiles_count].magnitude = magnitude;
		piano_tiles[piano_tiles_count].column = column;

		piano_tiles_count++;
	case 2:
		drum_tiles[drum_tiles_count].magnitude = magnitude;
		drum_tiles[drum_tiles_count].column = column;

		drum_tiles_count++;
	default:
	}

}

void BTN_Intr_Handler(void *InstancePtr)
{
	XGpio *BTNInstPtr = &BTNInst;
	usleep(20000); //20 ms
	// Disable GPIO interrupts
	XGpio_InterruptDisable(BTNInstPtr, BTN_INT);
	// Ignore additional button presses
	if ((XGpio_InterruptGetStatus(BTNInstPtr) & BTN_INT) != BTN_INT) {
		return;
	}
	btn_pressed = 1;
	btn_value = XGpio_DiscreteRead(BTNInstPtr, 1); // centre=1, down=2, left=3, right=4, up=5

	switch(mode){
	case 1: // Main Menu
		switch(btn_value){
			case 0x1: // Song Selection
				xil_printf("Centre button pressed\r\n");
				goToSongMenu(image_buffer_pointer, &mode);
				drawHighScores(highscores);
				break;
			case 0x10: // Quick Start
				xil_printf("Up button pressed\r\n");
				isPlaying = 1;
				COMM_VAL = 1; // tell core 1 to play music
				cur_song = 0;
				break;
			case 0x8: // Nothing
				xil_printf("Right button pressed\r\n");
				break;
			case 0x2: // Help Menu
				xil_printf("Down button pressed\r\n");
				drawHelpMenu(image_buffer_pointer);
				mode = 5;
				break;
			case 0x4: // Exit
				done = 1;
				xil_printf("Left button pressed\r\n");
				break;
		}
		break;
	case 2: // Song Select
//		COMM_VAL = 1; // tell core 1 to play music
		isPlaying = 1;

		switch(btn_value){
			case 0x1: // Song 2
				xil_printf("Centre button pressed\r\n");
				COMM_VAL = 2; // tell core 1 to play music
				cur_song = 1;
				break;
			case 0x10: // Song 1
				xil_printf("Up button pressed\r\n");
				COMM_VAL = 1; // tell core 1 to play music
				cur_song = 0;
				break;
			case 0x8: // Nothing
				xil_printf("Right button pressed\r\n");
				break;
			case 0x2: // Song 3
				xil_printf("Down button pressed\r\n");
				COMM_VAL = 3; // tell core 1 to play music
				cur_song = 2;
				break;
			case 0x4: // Back
				xil_printf("Left button pressed\r\n");
				goToMainMenu(image_buffer_pointer, &mode);
				break;
		}
		break;
	case 3: // Game Menu
//		if (inp=='c' && !sprites[i].hit){
//				int ret = ClickedPoints(sprites[i].y,&sprites[i]);
//				cur_score += ret;
//				xil_printf("%d\r\n",cur_score);
//			}
//		}
		switch(btn_value){
			case 0x1: // Song 2
				xil_printf("Centre button pressed\r\n");
//				isExit = 1;
//				goToMainMenu(image_buffer_pointer, &mode);
//				isExit = 0;
				break;
			case 0x10: // Column 2
				xil_printf("Up button pressed\r\n");
				cur_score += ClickedPoints(sprites[1].y, &sprites[1]);
				break;
			case 0x8: // Column 4
				xil_printf("Right button pressed\r\n");
				cur_score += ClickedPoints(sprites[3].y, &sprites[3]);
				break;
			case 0x2: // Column 3
				xil_printf("Down button pressed\r\n");
				cur_score += ClickedPoints(sprites[2].y, &sprites[2]);
				break;
			case 0x4: // Column 1
				xil_printf("Left button pressed\r\n");
				cur_score += ClickedPoints(sprites[0].y, &sprites[0]);
				break;
		}
		if (cur_score < 0) cur_score = 0;
		drawScore(cur_score);
		xil_printf("%d\r\n",cur_score);
		break;
	case 5:
		if (btn_value == 0x1)
			goToMainMenu(image_buffer_pointer, &mode);
		break;
	default: // Other
		break;
	}


    (void)XGpio_InterruptClear(BTNInstPtr, BTN_INT);
    // Enable GPIO interrupts
    XGpio_InterruptEnable(BTNInstPtr, BTN_INT);
}
