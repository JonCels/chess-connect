#include <SoftwareSerial.h>	//Bluetooth serial library
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h> //Touchscreen LCD library

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define YP A3 //Y+ to A3
#define XM A2 //X- to A2
#define YM 9 //Y- to D9
#define XP 8 //X+ to D8

// Touchscreen coordinate system
#define TS_LEFT 120
#define TS_TOP 80
#define TS_RIGHT 900
#define TS_BOT 940

// Pressure values for touchscreen
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// Assign names to common colors:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

//Size of buttons
#define BUTTONX 130
#define BUTTONY 70

SoftwareSerial BTserial(10, 11); // TXD | RXD

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, A4);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 290);

Adafruit_GFX_Button startButton, drawGameButton, resignBlackButton, resignWhiteButton;
bool gameInProgress = true;

void setup() {
	Serial.begin(9600);
	BTserial.begin(9600); 
	Serial.println("The bluetooth gates are open.\n Connect to HC-05 from any other bluetooth device with 1234 as pairing key!.");

	tft.begin(tft.readID());
	tft.setRotation(1); //Flips the screen to be landscape instead of portrait

	tft.fillScreen(WHITE);

	makeStartButton();
	makeEngineBox();
	makeWhiteResignButton();
	makeDrawButton();
	makeBlackResignButton();
}

void loop() {
	// Touchscreen points
	TSPoint p = ts.getPoint();
	pinMode(XM, OUTPUT);
	pinMode(YP, OUTPUT);

	btComm(); //Bluetooth communication

	if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
		//Map resistance to flipped visual screen
		int16_t tmpX = p.x;
		p.x = map(p.y, TS_TOP, TS_BOT, 0, tft.width());
		p.y = map(tmpX, TS_LEFT, TS_RIGHT, tft.height(), 0);
		
		//Start button pressed
		if (!gameInProgress && p.x < 135 && p.x > 5 && p.y < 75 && p.y > 5) {
			gameInProgress = true;
			tft.fillCircle(p.x, p.y, 2, GREEN);
			startGame();
		}

		//White resign button pressed
		if (gameInProgress && p.x < 135 && p.x > 5 && p.y < 315 && p.y > 245) {
			resignGame(true);
			tft.fillCircle(p.x, p.y, 2, RED);
		}

		//Draw button pressed
		if (gameInProgress && p.x < 305 && p.x > 175 && p.y < 315 && p.y > 245) {
			drawGame();
			tft.fillCircle(p.x, p.y, 2, BLUE);
		}

		//Black resign button pressed
		if (gameInProgress && p.x < 475 && p.x > 345 && p.y < 315 && p.y > 245) {
			resignGame(false);
			tft.fillCircle(p.x, p.y, 2, YELLOW);
		}
	}
}

void btComm() {
	// Receive data from external device
	if (BTserial.available()) {
		Serial.write(BTserial.read());
	}

	// Send data to external device
	if (Serial.available()){
		BTserial.write(Serial.read());
	}
}

/*
Drawn on top left of display, with a 5-pixel padding from the edges
*/
void makeStartButton() {
	startButton.initButton(&tft, BUTTONX/2 + 5, BUTTONY/2 + 5, BUTTONX, BUTTONY, BLACK, WHITE, BLACK, "Start", 3);
	startButton.drawButton();
}

void makeEngineBox() {
	//No line thickness option, this draws 3 rectangles to look like one thick rectangle
	tft.drawRect(tft.width() - 340, 5, 335, 235, BLACK);
	tft.drawRect(tft.width() - 339, 6, 333, 233, BLACK);
	tft.drawRect(tft.width() - 338, 7, 331, 231, BLACK);

}

void setEngineBoxText(char* str) {
	tft.fillRect(tft.width() - 337, 8, 329, 229, WHITE);
	tft.setCursor(tft.width() - 325, 15);
	tft.setTextColor(BLACK);
	tft.setTextSize(4);
	tft.print(strcat("1. ", str));
}

/*
Drawn on bottom left of display, with a 5-pixel padding from the edges
*/
void makeWhiteResignButton() {
	resignWhiteButton.initButton(&tft, BUTTONX/2 + 5, tft.height() - (BUTTONY/2 + 5), BUTTONX, BUTTONY, BLACK, WHITE, BLACK, "Resign", 3); //tft, x, y, w, h, outline, fill, textcolor, text, textsize
	resignWhiteButton.drawButton();
}

/*
Drawn on bottom middle of display, with a 5-pixel padding from the edges
*/
void makeDrawButton() {
	drawGameButton.initButton(&tft, tft.width()/2, tft.height() - (BUTTONY/2 + 5), BUTTONX, BUTTONY, RED, GREEN, BLACK, "Draw", 3); 
	drawGameButton.drawButton();
}

/*
Drawn on bottom right of display, with a 5-pixel padding from the edges
*/
void makeBlackResignButton() {
	resignBlackButton.initButton(&tft, tft.width() - (BUTTONX/2 + 5), tft.height() - (BUTTONY/2 + 5), BUTTONX, BUTTONY, BLACK, BLACK, WHITE, "Resign", 3);
	resignBlackButton.drawButton();
}

void resignGame(bool white) {
	gameInProgress = false;
	if (white) {
		Serial.println("White has resigned. Black wins by resignaton.");
	}
	else {
		Serial.println("Black has resigned. White wins by resignaton.");
	}
}

void drawGame() {
	gameInProgress = false;
	Serial.println("Game drawn.");
	setEngineBoxText("Bxe6");
}

void startGame() {
	gameInProgress = true;
	Serial.println("Game started.");
	setEngineBoxText("Rdf6");
}