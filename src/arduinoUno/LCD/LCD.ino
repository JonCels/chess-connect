#include <SoftwareSerial.h>   //Bluetooth serial library
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_TFTLCD.h>  // Hardware-specific library
#include <TouchScreen.h>      //Touchscreen LCD library

#define LCD_CS A3  // Chip Select
#define LCD_CD A2  // Command/Data
#define LCD_WR A1  // LCD Write
#define LCD_RD A0  // LCD Read

#define YP A3  //Y+ to A10
#define XM A2  //X- to A11
#define YM 23  //Y- to D9
#define XP 22  //X+ to D8

// Coordinate system for touchscreen
#define TS_MINX 110
#define TS_MINY 80
#define TS_MAXX 900
#define TS_MAXY 940

// Pressure values for touchscreen
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// Assign names to common colors:
#define BLACK 0x0000
#define WHITE 0xFFFF

#define BLUE 0x001F
#define RED 0xF800
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define BROWN 0xA388

// Default theme colours:
#define TEAL 0x028C
#define LIGHTBROWN 0xDD0D
#define GREEN 0x4305
#define MUSTARD 0xED67

// Dark theme colours:
#define DARK_BLUEGRAY 0x2947
#define DARK_BROWN 0x3944
#define DARK_GREEN 0x01A0
#define DARK_TAN 0xC4A9
#define DARK_LIGHTBROWN 0xC448

// Number and size of buttons on the main screen
#define NUM_MAIN_SCREEN_BUTTONS 2 
#define MAIN_BUTTON_X 230
#define MAIN_BUTTON_Y 235

#define MODE_BUTTON_X 230
#define MODE_BUTTON_Y 75

// Number and size of buttons on the game screen
#define NUM_GAME_SCREEN_BUTTONS 3 
#define GAME_BUTTON_X 200
#define GAME_BUTTON_Y 75

#define OK_BUTTON_X 100
#define OK_BUTTON_Y 80
#define TERMINATION_TEXT_Y 100

#define SETTINGS_X 70
#define SETTINGS_Y 70

#define TOUCH_DELAY 50

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, A4);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 290);

int BACKGROUND_COLOUR = LIGHTBROWN;
int PRIMARY_COLOUR = TEAL;
int SECONDARY_COLOUR = GREEN;
int TERTIARY_COLOUR = BLACK;
int BORDER_COLOUR = BLACK;
int TEXT_COLOUR_1 = WHITE;
int TEXT_COLOUR_2 = BLACK;
int TEXT_COLOUR_3 = BLACK;
int HIGHLIGHT_COLOUR = MUSTARD;

//[topLeftX, topLeftY] for each button
int mainScreenBounds[NUM_MAIN_SCREEN_BUTTONS][2] = {};
int modeSelectScreenBounds[3][2] = {};
int gameScreenBounds[NUM_GAME_SCREEN_BUTTONS][2] = {};
int okButtonBounds[1][2] = {};
int settingsButtonBounds[1][2] = {};

char* currentTheme = "light";
int currentScreen = 0; //0 for main screen, 1 for mode select screen, 2 for game screen, 3 for promotion screen, 4 for termination screen, 5 for error screen
int currentUserMode = 1; //0 for beginner mode, 1 for normal mode, 2 for engine mode
double time = 0.0;

void setup() {
	Serial.begin(9600);
	tft.begin(tft.readID());
	changeTheme(currentTheme);
	tft.setRotation(1);
	tft.fillScreen(BACKGROUND_COLOUR);
	makeMainScreen();
}

void loop() {
	// Touchscreen points
	TSPoint p = ts.getPoint();
	pinMode(XM, OUTPUT);
	pinMode(YP, OUTPUT);
	if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
		p.x = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());
		p.y = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
		int x = p.y;
		int y = tft.height() - p.x;
		handleTouch(x, y);
	}
}

void handleTouch(int x, int y) {
	//Start button (main screen or mode select screen)
	if ((currentScreen == 0 || currentScreen == 1) && (x > mainScreenBounds[0][0] && x < (mainScreenBounds[0][0] + MAIN_BUTTON_X) && y > mainScreenBounds[0][1] && y < (mainScreenBounds[0][1] + MAIN_BUTTON_Y))) {
		startGameButton();
	}
	
	//Mode button (main screen)
	if (currentScreen == 0 && (x > mainScreenBounds[1][0] && x < (mainScreenBounds[1][0] + MAIN_BUTTON_X) && y > mainScreenBounds[1][1] && y < (mainScreenBounds[1][1] + MAIN_BUTTON_Y))) {
		selectModeButton();
	}

	//Beginner mode button (mode select screen)
	if (currentScreen == 1 && (millis() > time + TOUCH_DELAY) && (x > modeSelectScreenBounds[0][0] && x < (modeSelectScreenBounds[0][0] + MODE_BUTTON_X) && y > modeSelectScreenBounds[0][1] && y < (modeSelectScreenBounds[0][1] + MODE_BUTTON_Y))) {
		selectMode(0);
	}

	//Normal mode button (mode select screen)
	if (currentScreen == 1 && (millis() > time + TOUCH_DELAY) && (x > modeSelectScreenBounds[1][0] && x < (modeSelectScreenBounds[1][0] + MODE_BUTTON_X) && y > modeSelectScreenBounds[1][1] && y < (modeSelectScreenBounds[1][1] + MODE_BUTTON_Y))) {
		selectMode(1);
	}

	//Engine mode button (mode select screen)
	if (currentScreen == 1 && (millis() > time + TOUCH_DELAY) && (x > modeSelectScreenBounds[2][0] && x < (modeSelectScreenBounds[2][0] + MODE_BUTTON_X) && y > modeSelectScreenBounds[2][1] && y < (modeSelectScreenBounds[2][1] + MODE_BUTTON_Y))) {
		selectMode(2);
	}

	//White resign button (game screen)
	if (currentScreen == 2 && (x > gameScreenBounds[0][0] && x < (gameScreenBounds[0][0] + GAME_BUTTON_X) && y > gameScreenBounds[0][1] && (y < gameScreenBounds[0][1] + GAME_BUTTON_Y))) {
		resignWhiteButton();
	}

	//Draw button (game screen)
	if (currentScreen == 2 && (x > gameScreenBounds[1][0] && x < (gameScreenBounds[1][0] + GAME_BUTTON_X) && y > gameScreenBounds[1][1] && (y < gameScreenBounds[1][1] + GAME_BUTTON_Y))) {
		drawButton();
	}

	//Black resign button (game screen)
	if (currentScreen == 2 && (x > gameScreenBounds[2][0] && x < (gameScreenBounds[2][0] + GAME_BUTTON_X) && y > gameScreenBounds[2][1] && (y < gameScreenBounds[2][1] + GAME_BUTTON_Y))) {
		resignBlackButton();
	}

	if (currentScreen == 4 && (x > okButtonBounds[0][0] && x < (okButtonBounds[0][0] + OK_BUTTON_X) && y > okButtonBounds[0][1] && (y < okButtonBounds[0][1] + OK_BUTTON_Y))) {
		okButton();
	}

	if ((currentScreen != 3 && currentScreen != 5) && (x > settingsButtonBounds[0][0] && x < (settingsButtonBounds[0][0] + SETTINGS_X) && y > settingsButtonBounds[0][1] && (y < settingsButtonBounds[0][1] + SETTINGS_Y))) {
		settingsButton();
	}
}

//Makes the main screen, defined with the appropriate number and size of buttons as defined globally. 
//Stores the x and y coordinates of the buttons in the array mainScreenBounds
void makeMainScreen() {
	tft.fillScreen(BACKGROUND_COLOUR);
	int paddingX = 5;
	int paddingY = 5;
	int x = paddingX;
	int y = 2*paddingY + SETTINGS_Y;

	tft.fillRect(x, y, MAIN_BUTTON_X + paddingX, MAIN_BUTTON_Y, SECONDARY_COLOUR);
	tft.drawRect(x, y, MAIN_BUTTON_X + paddingX, MAIN_BUTTON_Y, BORDER_COLOUR);
	mainScreenBounds[0][0] = x;
	mainScreenBounds[0][1] = y;

	tft.fillRect(3*paddingX + MAIN_BUTTON_X, y, MAIN_BUTTON_X, MAIN_BUTTON_Y, PRIMARY_COLOUR);
	tft.drawRect(3*paddingX + MAIN_BUTTON_X, y, MAIN_BUTTON_X, MAIN_BUTTON_Y, BORDER_COLOUR);
	mainScreenBounds[1][0] = 3*paddingX + MAIN_BUTTON_X;
	mainScreenBounds[1][1] = y;

	tft.setTextColor(TEXT_COLOUR_1);
	tft.setTextSize(6);
	int textInset = (MAIN_BUTTON_X - getPixelWidth("Start", 6)) / 2;
	tft.setCursor(mainScreenBounds[0][0] + textInset, mainScreenBounds[0][1] + 40);
	tft.println("Start");

	textInset = (MAIN_BUTTON_X - getPixelWidth("Game", 6)) / 2;
	tft.setCursor(mainScreenBounds[0][0] + textInset, tft.getCursorY() + paddingY);
	tft.println("Game");

	drawStartMode();
	
	tft.setTextColor(TEXT_COLOUR_1);
	tft.setTextSize(6);
	textInset = (MAIN_BUTTON_X - getPixelWidth("Select", 6)) / 2;
	tft.setCursor(mainScreenBounds[1][0] + textInset + 3, mainScreenBounds[1][1] + 40);
	tft.println("Select");

	textInset = (MAIN_BUTTON_X - getPixelWidth("User", 6)) / 2;
	tft.setCursor(mainScreenBounds[1][0] + textInset, tft.getCursorY() + paddingY);
	tft.println("User");
	
	textInset = (MAIN_BUTTON_X - getPixelWidth("Mode", 6)) / 2;
	tft.setCursor(mainScreenBounds[1][0] + textInset, tft.getCursorY() + paddingY);
	tft.println("Mode");

	makeSettingsButton();
}

void makeModeSelectScreen() {
	int paddingX = 5;
	int paddingY = 5;
	int x = 3*paddingX + MAIN_BUTTON_X;
	int y = 3*paddingY + SETTINGS_Y + MODE_BUTTON_Y;
	tft.fillRect(x, 2*paddingY + SETTINGS_Y, MODE_BUTTON_X, 3*MODE_BUTTON_Y + 2*paddingY, BACKGROUND_COLOUR);

	tft.fillRect(x, y - MODE_BUTTON_Y - 5, MODE_BUTTON_X, MODE_BUTTON_Y, PRIMARY_COLOUR);
	tft.drawRect(x, y - MODE_BUTTON_Y - 5, MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
	modeSelectScreenBounds[0][0] = x;
	modeSelectScreenBounds[0][1] = y - MODE_BUTTON_Y - 5;

	tft.fillRect(x, y, MODE_BUTTON_X, MODE_BUTTON_Y, HIGHLIGHT_COLOUR);
	tft.drawRect(x, y, MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
	modeSelectScreenBounds[1][0] = x;
	modeSelectScreenBounds[1][1] = y;
	
	tft.fillRect(x, y + MODE_BUTTON_Y + 5, MODE_BUTTON_X, MODE_BUTTON_Y, PRIMARY_COLOUR);
	tft.drawRect(x, y + MODE_BUTTON_Y + 5, MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
	modeSelectScreenBounds[2][0] = x;
	modeSelectScreenBounds[2][1] = y + MODE_BUTTON_Y + 5;
	
	int textInset = (MAIN_BUTTON_X - getPixelWidth("Beginner", 4)) / 2;
	tft.setCursor(modeSelectScreenBounds[0][0] + textInset, modeSelectScreenBounds[0][1] + 24);
	tft.setTextColor(TEXT_COLOUR_1);
	tft.setTextSize(4);
	tft.print("Beginner");

	textInset = (MAIN_BUTTON_X - getPixelWidth("Normal", 4)) / 2;
	tft.setCursor(modeSelectScreenBounds[1][0] + textInset, modeSelectScreenBounds[1][1] + 24);
	tft.setTextColor(TEXT_COLOUR_3);
	tft.setTextSize(4);
	tft.print("Normal");

	textInset = (MAIN_BUTTON_X - getPixelWidth("Engine", 4)) / 2;
	tft.setCursor(modeSelectScreenBounds[2][0] + textInset, modeSelectScreenBounds[2][1] + 24);
	tft.setTextColor(TEXT_COLOUR_1);
	tft.setTextSize(4);
	tft.print("Engine");

	time = millis();
}

void makeGameScreen() {
	tft.fillScreen(BACKGROUND_COLOUR);
	int padding = 5;
	int startY = 80;
	int x = tft.width() - GAME_BUTTON_X - padding;
	int y = startY;

	// Create buttons
	tft.fillRect(x, y, GAME_BUTTON_X, GAME_BUTTON_Y, PRIMARY_COLOUR);
	tft.drawRect(x, y, GAME_BUTTON_X, GAME_BUTTON_Y, BORDER_COLOUR);
	gameScreenBounds[0][0] = x;
	gameScreenBounds[0][1] = y;

	tft.fillRect(x, y + padding + GAME_BUTTON_Y, GAME_BUTTON_X, GAME_BUTTON_Y, SECONDARY_COLOUR);
	tft.drawRect(x, y + padding + GAME_BUTTON_Y, GAME_BUTTON_X, GAME_BUTTON_Y, BORDER_COLOUR);
	gameScreenBounds[1][0] = x;
	gameScreenBounds[1][1] = y + padding + GAME_BUTTON_Y;

	tft.fillRect(x, y + 2*padding + 2*GAME_BUTTON_Y, GAME_BUTTON_X, GAME_BUTTON_Y, TERTIARY_COLOUR);
	tft.drawRect(x, y + 2*padding + 2*GAME_BUTTON_Y, GAME_BUTTON_X, GAME_BUTTON_Y, BORDER_COLOUR);
	gameScreenBounds[2][0] = x;
	gameScreenBounds[2][1] = y + 2*padding + 2*GAME_BUTTON_Y;

	tft.drawRect(padding, startY, tft.width() - 3*padding - GAME_BUTTON_X, 3*GAME_BUTTON_Y + 2*padding, BORDER_COLOUR);
	
	// Add text to buttons
	int textInset = (GAME_BUTTON_X - getPixelWidth("Resign", 4)) / 2;
	tft.setCursor(gameScreenBounds[0][0] + textInset, gameScreenBounds[0][1] + 10);
	tft.setTextColor(TEXT_COLOUR_1);
	tft.setTextSize(4);
	tft.println("Resign");
	textInset = (GAME_BUTTON_X - getPixelWidth("(White)", 3)) / 2;
	tft.setTextSize(3);
	tft.setCursor(gameScreenBounds[0][0] + textInset, tft.getCursorY() + 3);
	tft.print("(White)");

	textInset = (GAME_BUTTON_X - getPixelWidth("Draw", 4)) / 2;
	tft.setCursor(gameScreenBounds[1][0] + textInset, gameScreenBounds[1][1] + 25);
	tft.setTextColor(TEXT_COLOUR_1);
	tft.setTextSize(4);
	tft.print("Draw");

	textInset = (GAME_BUTTON_X - getPixelWidth("Resign", 4)) / 2;
	tft.setCursor(gameScreenBounds[2][0] + textInset, gameScreenBounds[2][1] + 10);
	tft.setTextColor(TEXT_COLOUR_1);
	tft.setTextSize(4);
	tft.println("Resign");
	textInset = (GAME_BUTTON_X - getPixelWidth("(Black)", 3)) / 2;
	tft.setTextSize(3);
	tft.setCursor(gameScreenBounds[0][0] + textInset, tft.getCursorY() + 3);
	tft.print("(Black)");

	tft.setCursor(5, 24);
	tft.setTextColor(TEXT_COLOUR_2);
	tft.setTextSize(4);
	char* currMode = getUserMode(currentUserMode);
	tft.print(currMode);
	tft.print(" Mode");

	makeSettingsButton();
}

void makeEndScreen(char* msg) {
	tft.fillScreen(BACKGROUND_COLOUR);
	resetState();
	int x = (tft.width() - OK_BUTTON_X) / 2;
	int y = 2 * tft.height() / 3;
	tft.fillRect(x, y, OK_BUTTON_X, OK_BUTTON_Y, SECONDARY_COLOUR);
	tft.drawRect(x, y, OK_BUTTON_X, OK_BUTTON_Y, BORDER_COLOUR);
	okButtonBounds[0][0] = x;
	okButtonBounds[0][1] = y;

	tft.setCursor(x + 15, y + 20);
	tft.setTextColor(TEXT_COLOUR_1);
	tft.setTextSize(6);
	tft.print("OK");

	// Split message into 2 lines for readability
	char* line2 = strstr(msg, "by ") + sizeof(char)*3;
	int ind = line2 - msg;
	char line1[ind];
	getSubstring(msg, line1, 0, ind);

	tft.setTextColor(TEXT_COLOUR_2);
	tft.setTextSize(5);

	x = (tft.width() - getPixelWidth(line1, 5)) / 2;
	tft.setCursor(x, TERMINATION_TEXT_Y);
	tft.println(line1);

	x = (tft.width() - getPixelWidth(line2, 5)) / 2;
	tft.setCursor(x, tft.getCursorY() + 5);
	tft.print(line2);
}

void makePromotionScreen() {
	tft.fillScreen(BACKGROUND_COLOUR);
}

char* getSubstring(char* src, char* dest, int start, int end) {
	memcpy(dest, &src[start], end - start);
	dest[end - start - 1] = '\0';
	return dest;
}

void startGameButton() {
	currentScreen = 2;
	makeGameScreen();
}

void selectModeButton() {
	makeModeSelectScreen();
	currentScreen = 1;
}

void selectMode(int mode) {
	if (currentUserMode != mode) {
		if (mode == 0) {
			beginnerModeButton(true);
		}
		else if (mode == 1) {
			normalModeButton(true);
		}
		else if (mode == 2) {
			engineModeButton(true);
		}	
	}
}

void beginnerModeButton(bool active) {
	if (active) {
		tft.fillRect(modeSelectScreenBounds[0][0], modeSelectScreenBounds[0][1], MODE_BUTTON_X, MODE_BUTTON_Y, HIGHLIGHT_COLOUR);
		tft.drawRect(modeSelectScreenBounds[0][0], modeSelectScreenBounds[0][1], MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
		if (currentUserMode == 1) {
			normalModeButton(false);
		}
		else if (currentUserMode == 2) {
			engineModeButton(false);
		}
		currentUserMode = 0;
		drawStartMode();
		tft.setTextColor(TEXT_COLOUR_3);
	}
	else {
		tft.fillRect(modeSelectScreenBounds[0][0], modeSelectScreenBounds[0][1], MODE_BUTTON_X, MODE_BUTTON_Y, PRIMARY_COLOUR);
		tft.drawRect(modeSelectScreenBounds[0][0], modeSelectScreenBounds[0][1], MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
		tft.setTextColor(TEXT_COLOUR_1);
	}
	int textInset = (MODE_BUTTON_X - getPixelWidth("Beginner", 4)) / 2;
	tft.setCursor(modeSelectScreenBounds[0][0] + textInset, modeSelectScreenBounds[0][1] + 24);
	tft.setTextSize(4);
	tft.print("Beginner");
}

void normalModeButton(bool active) {
	if (active) {
		tft.fillRect(modeSelectScreenBounds[1][0], modeSelectScreenBounds[1][1], MODE_BUTTON_X, MODE_BUTTON_Y, HIGHLIGHT_COLOUR);
		tft.drawRect(modeSelectScreenBounds[1][0], modeSelectScreenBounds[1][1], MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
		if (currentUserMode == 0) {
			beginnerModeButton(false);
		}
		else if (currentUserMode == 2) {
			engineModeButton(false);
		}
		currentUserMode = 1;
		drawStartMode();
		tft.setTextColor(TEXT_COLOUR_3);
	}
	else {
		tft.fillRect(modeSelectScreenBounds[1][0], modeSelectScreenBounds[1][1], MODE_BUTTON_X, MODE_BUTTON_Y, PRIMARY_COLOUR);
		tft.drawRect(modeSelectScreenBounds[1][0], modeSelectScreenBounds[1][1], MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
		tft.setTextColor(TEXT_COLOUR_1);
	}
	int textInset = (MODE_BUTTON_X - getPixelWidth("Normal", 4)) / 2;
	tft.setCursor(modeSelectScreenBounds[1][0] + textInset, modeSelectScreenBounds[1][1] + 24);
	tft.setTextSize(4);
	tft.print("Normal");
}

void engineModeButton(bool active) {
	if (active) {
		tft.fillRect(modeSelectScreenBounds[2][0], modeSelectScreenBounds[2][1], MODE_BUTTON_X, MODE_BUTTON_Y, HIGHLIGHT_COLOUR);
		tft.drawRect(modeSelectScreenBounds[2][0], modeSelectScreenBounds[2][1], MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
		if (currentUserMode == 0) {
			beginnerModeButton(false);
		}
		else if (currentUserMode == 1) {
			normalModeButton(false);
		}
		currentUserMode = 2;
		drawStartMode();
		tft.setTextColor(TEXT_COLOUR_3);
	}
	else {
		tft.fillRect(modeSelectScreenBounds[2][0], modeSelectScreenBounds[2][1], MODE_BUTTON_X, MODE_BUTTON_Y, PRIMARY_COLOUR);
		tft.drawRect(modeSelectScreenBounds[2][0], modeSelectScreenBounds[2][1], MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
		tft.setTextColor(TEXT_COLOUR_1);
	}
	int textInset = (MODE_BUTTON_X - getPixelWidth("Engine", 4)) / 2;
	tft.setCursor(modeSelectScreenBounds[2][0] + textInset, modeSelectScreenBounds[2][1] + 24);
	tft.setTextSize(4);
	tft.print("Engine");
}

void resignWhiteButton() {
	currentScreen = 4;
	makeEndScreen("Black Wins by Resignation");
}

void resignBlackButton() {
	currentScreen = 4;
	makeEndScreen("White Wins by Resignation");
}

void drawButton() {
	currentScreen = 4;
	makeEndScreen("Game Drawn by Agreement");
}

void checkmateWhite() {
	currentScreen = 4;
	makeEndScreen("White Wins by Checkmate");
}

void checkmateBlack() {
	currentScreen = 4;
	makeEndScreen("Black Wins by Checkmate");
}

void stalemate() {
	currentScreen = 4;
	makeEndScreen("Game Drawn by Stalemate");
}

void okButton() {
	currentScreen = 0;
	makeMainScreen();
}

void settingsButton() {
	if (strcmp(currentTheme, "dark") == 0) {
		changeTheme("light");
	}
	else if (strcmp(currentTheme, "light") == 0) {
		changeTheme("dark");
	}
}

int getPixelWidth(char* str, int textSize) {
	int strLen = strlen(str);
	int pixelSize = textSize * 6;
	int wordPixelWidth = strLen * pixelSize;
	return wordPixelWidth;
}

char* getUserMode(int mode) {
	if (mode == 0) {
		return "Beginner";
	}
	else if (mode == 1) {
		return "Normal";
	}
	else if (mode == 2) {
		return "Engine";
	}
}

void drawStartMode() {	
	char* tmp = getUserMode(currentUserMode);
	char mode[11] = {};
	if (strcmp(tmp, "Beginner") == 0) {
		strcat(mode, "(");
		strcat(mode, tmp);
		strcat(mode, ")");		
	}
	else {
		strcat(mode, " (");
		strcat(mode, tmp);
		strcat(mode, ") ");			
	}
	
	tft.setTextSize(3);
	tft.setTextColor(TEXT_COLOUR_1, SECONDARY_COLOUR);
	int textInset = (MAIN_BUTTON_X - getPixelWidth(mode, 3)) / 2 + 2;
	tft.setCursor(mainScreenBounds[0][0] + textInset, mainScreenBounds[0][1] + 160);
	tft.print(mode);
}

void makeSettingsButton() {
	int padding = 5;
	tft.drawRect(tft.width() - padding - SETTINGS_X, padding, SETTINGS_X, SETTINGS_Y, BORDER_COLOUR);
	settingsButtonBounds[0][0] = tft.width() - padding - SETTINGS_X;
	settingsButtonBounds[0][1] = padding;
}

void resetState() {
	currentUserMode = 1;
}

void changeTheme(char *theme) {
	if (strcmp(theme, "light") == 0) {
		BACKGROUND_COLOUR = LIGHTBROWN;
		PRIMARY_COLOUR = TEAL;
		SECONDARY_COLOUR = GREEN;
		TERTIARY_COLOUR = BLACK;
		BORDER_COLOUR = BLACK;
		TEXT_COLOUR_1 = WHITE;
		TEXT_COLOUR_2 = BLACK;
		TEXT_COLOUR_3 = BLACK;
		HIGHLIGHT_COLOUR = MUSTARD;
	}
	else if (strcmp(theme, "dark") == 0) {
		BACKGROUND_COLOUR = BLACK;
		PRIMARY_COLOUR = DARK_BLUEGRAY;
		SECONDARY_COLOUR = DARK_BROWN;
		TERTIARY_COLOUR = DARK_GREEN;
		BORDER_COLOUR = DARK_TAN;
		TEXT_COLOUR_1 = BLACK;
		TEXT_COLOUR_2 = DARK_TAN;
		TEXT_COLOUR_3 = BLACK;
		HIGHLIGHT_COLOUR = 0xC448;
	}
	currentTheme = theme;
}