#include <SoftwareSerial.h>   //Bluetooth serial library
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_TFTLCD.h>  // Hardware-specific library
#include <TouchScreen.h>      //Touchscreen LCD library

#define LCD_CS A10  // Chip Select
#define LCD_CD A11  // Command/Data
#define LCD_WR A12  // LCD Write
#define LCD_RD A13  // LCD Read

#define YP A10  //Y+ to A10
#define XM A11  //X- to A11
#define YM 23   //Y- to D9
#define XP 22   //X+ to D8

// Touchscreen coordinate system
#define TS_LEFT 120
#define TS_TOP 80
#define TS_RIGHT 900
#define TS_BOT 940

// Pressure values for touchscreen
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// Assign names to common colors:
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x4CE7
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

//Size of buttons
#define BUTTONX 110
#define BUTTONY 70
#define MODEBUTTONY 60

SoftwareSerial BTserial(10, 11);  // TXD | RXD

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, A4);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 290);

Adafruit_GFX_Button startButton, drawGameButton, resignBlackButton, resignWhiteButton;
bool gameInProgress = false;

//State
char* currFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"; // Starting position
char currTerminationState[2] = {'s', '\0'}; // start, white resign, black resign, draw // char* instead of char to enable use with strcpy and strcat
char currMode[2] = {'n', '\0'}; // normal, engine, beginner // char* instead of char to enable use with strcpy and strcat
char* commData = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1@s@n";
char* code = "a0";

char inputStr[10] = "";

//char serialFENStr[64] = "";
//char serialFENStr2[64] = "";

void setup() {
	Serial.begin(9600);
	BTserial.begin(9600);
	//Serial1.begin(9600);

	Serial.println("The bluetooth gates are open.");

	tft.begin(tft.readID());
	tft.setRotation(1);  // Flips the screen to be landscape instead of portrait

	tft.fillScreen(WHITE);

	makeStartButton();
	makeEngineBox();
	makeWhiteResignButton();
	makeDrawButton();
	makeBlackResignButton();
	makeGameModeButtons();
}

void loop() {
	// Touchscreen points
	TSPoint p = ts.getPoint();
	pinMode(XM, OUTPUT);
	pinMode(YP, OUTPUT);

	btComm();  // Bluetooth communication

	// if (Serial1.available()) {
	// 	char c = Serial1.read();
		
	// 	if (c != '\r' && c != '\n') {
	// 		appendCharToStr(serialFENStr, c);
	// 	}
	// 	else {
	// 		if (strlen(serialFENStr)) {
	// 			if (strcmp("rnbqkbrnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0", serialFENStr)) {//Weird error, fix later
	// 				if(strcmp(currFEN, serialFENStr)) { //String empty sometimes, possible issue with memset usage or with appendCharToStr
	// 					//Serial.println(serialFENStr);
	// 					strcpy(currFEN, serialFENStr);
	// 					Serial.println(currFEN);
	// 					updateState();
	// 				}
	// 			}
	// 		}	
	// 		memset(serialFENStr, 0, 100);
	// 	}
	// }

	// if (Serial2.available()) {
	// 	char c = Serial2.read();

	// 	if (c != '\r' && c != '\n') {
	// 		appendCharToStr(serialFENStr2, c);
	// 	}
	// 	else {
	// 		if (strlen(serialFENStr2)) {
	// 			Serial.println("Here");
	// 			//strcpy(currFEN, serialFENStr2);
	// 			char* tmp = "";
	// 			strcpy(tmp, serialFENStr);
	// 			strcat(tmp, serialFENStr2);
	// 			strcpy(currFEN, tmp);
	// 			Serial.println(currFEN);
	// 			updateState();
	// 		}	
	// 		memset(serialFENStr, 0, 100);
	// 	}
	// }
	

	if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
		// Map resistance to flipped visual screen
		int16_t tmpX = p.x;
		p.x = map(p.y, TS_TOP, TS_BOT, 0, tft.width());
		p.y = map(tmpX, TS_LEFT, TS_RIGHT, tft.height(), 0);

		tft.drawCircle(p.x, p.y, 3, RED);
		// Start button pressed
		if (!gameInProgress && p.x < BUTTONX + 5 && p.x > 5 && p.y < tft.height() - 5 && p.y > tft.height() - (BUTTONY + 5)) {
			gameInProgress = true;
			startGame();
		}

		// White resign button pressed
		if (gameInProgress && p.x < 2*(BUTTONX + 5) && p.x > BUTTONX + 10 && p.y < tft.height() - 5 && p.y > tft.height() - (BUTTONY + 5)) {
			resignGame(true);
		}

		// Draw button pressed
		if (gameInProgress && p.x < tft.width() - (BUTTONX + 10) && p.x > 2*BUTTONX + 20 && p.y < tft.height() - 5 && p.y > tft.height() - (BUTTONY + 5)) {
			drawGame();
		}

		// Black resign button pressed
		if (gameInProgress && p.x < tft.width() - 5 && p.x > tft.width() - (BUTTONX + 5) && p.y < tft.height() - 5 && p.y > tft.height() - (BUTTONY + 5)) {
			resignGame(false);
		}

		// Normal mode button pressed
		if (!gameInProgress && currMode[0] != 'n' && p.x < BUTTONX + 5 && p.x > 5 && p.y < MODEBUTTONY + 55 && p.y > 55) {
			changeMode(0);
		}

		// Engine mode button pressed
		if (!gameInProgress && currMode[0] != 'e' && p.x < BUTTONX + 5 && p.x > 5 && p.y < 2*MODEBUTTONY + 55 && p.y > MODEBUTTONY + 55) {
			changeMode(1);
		}

		// Beginner mode button pressed
		if (!gameInProgress && currMode[0] != 'b' && !gameInProgress && p.x < BUTTONX + 5 && p.x > 5 && p.y < 3*MODEBUTTONY + 55 && p.y > 2*MODEBUTTONY + 55) {
			changeMode(2);
		}
	}
}

void btComm() {
	// Receive data from external device
	if (BTserial.available()) {
		char c = BTserial.read();

		if (c != '\r' && c != '\n') {
			appendCharToStr(inputStr, c);
		}
		else {
			if (strlen(inputStr)) { //String empty sometimes, possible issue with memset usage or with appendCharToStr
				parseWebPayload(inputStr);
			}
			memset(inputStr, 0, 10);
		}
	}

	// Send data to external device
	if (Serial.available()) {
		BTserial.write(Serial.read());
	}
}

void appendCharToStr(char* str, char c) {
	for (; *str; str++);
	*str++ = c;
	*str++ = 0;
}

void makeEngineBox() {
	// No line thickness option, this draws 3 rectangles to look like one thick rectangle
	tft.drawRect(tft.width() - (3*BUTTONX + 30), 5, 3*BUTTONX + 25, tft.height() - (BUTTONY + 15), BLACK);
	tft.drawRect(tft.width() - (3*BUTTONX + 29), 6, 3*BUTTONX + 23, tft.height() - (BUTTONY + 17), BLACK);
	tft.drawRect(tft.width() - (3*BUTTONX + 28), 7, 3*BUTTONX + 21, tft.height() - (BUTTONY + 19), BLACK);
	tft.drawLine(10 + BUTTONX, 55, tft.width() - 6, 55, BLACK);
	tft.setCursor(40 + BUTTONX, 20);
	tft.setTextSize(3);
	tft.print("Best Engine Move");
}

void setEngineBoxText(char* str) {
	tft.fillRect(14 + BUTTONX, 56, 3*BUTTONX + 18, tft.height() - (BUTTONY + 87), WHITE);
	tft.setCursor(34 + BUTTONX, 116);
	tft.setTextColor(BLACK);
	tft.setTextSize(6);
	char tmp[strlen(str) + 3];
	strcpy(tmp, "1. ");
	strcat(tmp, str);
	tft.print(tmp);
}

/*
Drawn on top left of display, with a 5-pixel padding from the edges
*/
void makeStartButton() {
	startButton.initButton(&tft, BUTTONX / 2 + 5, tft.height() - (BUTTONY / 2 + 5), BUTTONX, BUTTONY, BLACK, WHITE, BLACK, "Start", 3);
	startButton.drawButton();
}

/*
Drawn on bottom left of display, with a 5-pixel padding from the edges
*/
void makeWhiteResignButton() {
	resignWhiteButton.initButton(&tft, BUTTONX / 2 + 10 + BUTTONX, tft.height() - (BUTTONY / 2 + 5), BUTTONX, BUTTONY, BLACK, WHITE, BLACK, "Resign", 2);  //tft, x, y, w, h, outline, fill, textcolor, text, textsize
	resignWhiteButton.drawButton();
}

/*
Drawn on bottom middle of display, with a 5-pixel padding from the edges
*/
void makeDrawButton() {
	drawGameButton.initButton(&tft, (BUTTONX + 15) / 2 + 15 + 2 * BUTTONX, tft.height() - (BUTTONY / 2 + 5), BUTTONX + 15, BUTTONY, BLACK, GREEN, WHITE, "Draw", 3);
	drawGameButton.drawButton();
}

/*
Drawn on bottom right of display, with a 5-pixel padding from the edges
*/
void makeBlackResignButton() {
	resignBlackButton.initButton(&tft, tft.width() - (BUTTONX / 2 + 5), tft.height() - (BUTTONY / 2 + 5), BUTTONX, BUTTONY, BLACK, BLACK, WHITE, "Resign", 2);
	resignBlackButton.drawButton();
}

void makeGameModeButtons() {
	tft.drawRect(5, 5, BUTTONX, tft.height() - (BUTTONY + 15), BLACK);
	tft.drawLine(5, 55, BUTTONX + 4, 55, BLACK);
	tft.drawLine(5, MODEBUTTONY + 56, BUTTONX + 4, MODEBUTTONY + 56, BLACK);
	tft.drawLine(5, 2 * MODEBUTTONY + 57, BUTTONX + 4, 2 * MODEBUTTONY + 57, BLACK);

	tft.setCursor(25, 20);
	tft.setTextColor(BLACK);
	tft.setTextSize(3);
	tft.setTextWrap(true);
	tft.print("Mode");

	tft.fillRect(6, 56, BUTTONX - 2, MODEBUTTONY, YELLOW);
	tft.setCursor(23, MODEBUTTONY + 20);
	tft.setTextSize(2);
	tft.print("Normal");

	tft.setCursor(23, 2 * MODEBUTTONY + 20);
	tft.print("Engine");

	tft.setCursor(13, 3 * MODEBUTTONY + 20);
	tft.print("Beginner");
}

void selectNormalMode(bool active) {
	if (active) {
		tft.fillRect(6, 56, BUTTONX - 2, MODEBUTTONY, YELLOW);
		selectEngineMode(false);
		selectBeginnerMode(false);
		currMode[0] = 'n';
	}
	else {
		tft.fillRect(6, 56, BUTTONX - 2, MODEBUTTONY, WHITE);
	}
	tft.setCursor(23, MODEBUTTONY + 20);
	tft.setTextSize(2);
	tft.print("Normal");
}

void selectEngineMode(bool active) {
	if (active) {
		tft.fillRect(6, 57 + MODEBUTTONY, BUTTONX - 2, MODEBUTTONY, YELLOW);
		selectNormalMode(false);
		selectBeginnerMode(false);
		currMode[0] = 'e';
	}
	else {
		tft.fillRect(6, 57 + MODEBUTTONY, BUTTONX - 2, MODEBUTTONY, WHITE);
	}
	tft.setCursor(23, 2 * MODEBUTTONY + 20);
	tft.setTextSize(2);
	tft.print("Engine");
}

void selectBeginnerMode(bool active) {
	if (active) {
		tft.fillRect(6, 58 + 2*MODEBUTTONY, BUTTONX - 2, MODEBUTTONY + 1, YELLOW);
		selectNormalMode(false);
		selectEngineMode(false);
		currMode[0] = 'b';
	}
	else {
		tft.fillRect(6, 58 + 2*MODEBUTTONY, BUTTONX - 2, MODEBUTTONY + 1, WHITE);
	}
	tft.setCursor(13, 3 * MODEBUTTONY + 20);
	tft.setTextSize(2);
	tft.print("Beginner");
}

void resignGame(bool white) {
	gameInProgress = false;
	if (white) {
		currTerminationState[0] = 'w';
		Serial.println("White has resigned");
	} else {
		currTerminationState[0] = 'b';
		Serial.println("Black has resigned");
	}
	updateState();
	
	//Serial1.write(2);
}

void drawGame() {
	gameInProgress = false;
	currTerminationState[0] = 'd';
	updateState();
	Serial.println("Game drawn");
	//Serial1.write(2);
}

void startGame() {
	gameInProgress = true;
	currTerminationState[0] = 's';
	updateState();
	Serial.println("Game started");
	// Send game start signal to other arduino
	//Serial1.write(1);
}

void changeMode(int m) {
	if (m == 0) {
		selectNormalMode(true);
		Serial.println("Normal mode");
	}
	else if (m == 1) {
		selectEngineMode(true);
		Serial.println("Normal mode");
	}
	else if (m == 2) {
		selectBeginnerMode(true);
		Serial.println("Normal mode");
	}
	updateState();
}

void updateState() {
	char tmp[strlen(currFEN) + 5];
	Serial.println(currFEN);
	strcpy(tmp, currFEN);
	strcat(tmp, "@");
	strcat(tmp, currTerminationState);
	strcat(tmp, "@");
	strcat(tmp, currMode);
	strcat(tmp, "\r");
	strcpy(commData, tmp);
	Serial.println(commData);
	//BTserial.write(commData);
}

void parseWebPayload(char* inStr) {	
	char* engineMove = strtok(inStr, "@");
	char* incomingWebCode = strtok(NULL, "@");
	char* incomingGameState = strtok(NULL, "@");

	if (strcmp(engineMove, "a")) {
		if (strcmp(code, incomingWebCode)) {
			//Serial.println(engineMove);
			strcpy(code, incomingWebCode);
			setEngineBoxText(engineMove);
			//Send incomingGameState to other Arduino
			if (strcmp(incomingGameState, "n")) {
				//Serial1.write("e");
			}
		}
	}
}