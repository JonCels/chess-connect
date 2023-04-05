#include <SoftwareSerial.h>	 //Bluetooth serial library
#include <Adafruit_GFX.h>	 //Core graphics library
#include <Adafruit_TFTLCD.h> //Hardware-specific library
#include <TouchScreen.h>	 //Touchscreen LCD library

#define LCD_CS A3 //Chip Select
#define LCD_CD A2 //Command/Data
#define LCD_WR A1 //LCD Write
#define LCD_RD A0 //LCD Read

#define CLK A5
#define CS A8

#define YP A3 //Y+ to A10
#define XM A2 //X- to A11
#define YM 23 //Y- to D9
#define XP 22 //X+ to D8

//Coordinate system for touchscreen
#define TS_MINX 110
#define TS_MINY 80
#define TS_MAXX 900
#define TS_MAXY 940

//Pressure values for touchscreen
#define MINPRESSURE 50
#define MAXPRESSURE 1000

//Delay before lights come on after piece is picked up in beginner mode
#define SUSPENSION_TIME 1000

#define AVG_SAMPLE_SIZE 20

//Common colours:
#define BLACK_COLOUR 0x0000
#define WHITE_COLOUR 0xFFFF

//Default theme colours:
#define TEAL 0x028C
#define LIGHTBROWN 0xDD0D
#define GREEN 0x4305
#define MUSTARD 0xED67
#define DARKGRAY 0x41E8
#define PINK 0xEAAA
#define PURPLE 0x398A

//Dark theme colours:
#define DARK_BLUEGRAY 0x2947
#define DARK_BROWN 0x3944
#define DARK_GREEN 0x01A0
#define DARK_TAN 0xC4A9
#define DARK_LIGHTBROWN 0xC448
#define DARK_BROWNWHITE 0xE612
#define DARK_PINK 0x4806
#define DARK_PURPLE 0x1885

//Main screen buttons
#define MAIN_BUTTON_X 230
#define MAIN_BUTTON_Y 235

//Mode screen buttons
#define MODE_BUTTON_X 230
#define MODE_BUTTON_Y 75

//Game screen buttons
#define GAME_BUTTON_X 200
#define GAME_BUTTON_Y 75

//Promotion screen buttons
#define PROMOTION_COMMON_X 360
#define PROMOTION_RARE_X 90
#define PROMOTION_Y 100
#define ICON_SIZE 90

//Confirmation screen buttons
#define YES_NO_X 120
#define YES_NO_Y 80

//End/error screen buttons
#define OK_BUTTON_X 100
#define OK_BUTTON_Y 80
#define TERMINATION_TEXT_Y 100

//Theme button
#define THEME_BUTTON_X 70
#define THEME_BUTTON_Y 70

#define TOUCH_DELAY 50

//Hall sensors
#define BLACK_HALL 120
#define WHITE_HALL 420

#define BOARD_X 8
#define BOARD_Y 8

//Maximum possible number of squares that each type of piece can move to
#define PAWN_SIGHT 4
#define KNIGHT_SIGHT 8
#define BISHOP_SIGHT 13
#define ROOK_SIGHT 14
#define QUEEN_SIGHT 27
#define KING_SIGHT 8

enum UserMode : char
{
	BEGINNER = 'B',
	NORMAL = 'N',
	ENGINE = 'E',
	NO_MODE = 'M'
};

enum GameState : char
{
	GAME_ACTIVE,
	GAME_INACTIVE,
	GAME_TERMINATED,
	PIECE_LIFTED,
	PIECE_SUSPENDED,
	PROMOTING,
	ERROR
};

//Follows FEN notation assuming white pieces. For black pieces, add 32
enum ChessPiece : char
{
	PAWN = 'P',
	KNIGHT = 'N',
	BISHOP = 'B',
	ROOK = 'R',
	QUEEN = 'Q',
	KING = 'K',
	NO_PIECE = '0'
};

enum Colour : char
{
	WHITE = 'w',
	BLACK = 'b',
	NO_COLOUR = 'n'
};

enum Theme
{
	LIGHT,
	DARK
};

enum Screen
{ 
	MAIN_SCREEN,
	MODE_SELECT_SCREEN,
	GAME_SCREEN,
	PROMOTION_SCREEN,
	TERMINATION_SCREEN,
	ERROR_SCREEN,
	CONFIRMATION_SCREEN
};

//Represents each square on the physical chess board
struct Square
{
	ChessPiece piece;
	Colour colour;
	int row;
	int col;

	//Define == and != for Square objects
	bool operator==(const Square &s) const
	{
		return (piece == s.piece && colour == s.colour && row == s.row && col == s.col);
	}

	bool operator!=(const Square &s) const
	{
		return !(piece == s.piece && colour == s.colour && row == s.row && col == s.col);
	}

    //Constructors
	Square()
	{
		piece = NO_PIECE;
		colour = NO_COLOUR;
		row = -1;
		col = -1;
	}

	Square(int r, int c)
	{
		piece = NO_PIECE;
		colour = NO_COLOUR;
		row = r;
		col = c;
	}

	Square(ChessPiece piece_, Colour pieceColour, int r, int c)
	{
		piece = piece_;
		colour = pieceColour;
		row = r;
		col = c;
	}
};

//Global variables

//Bluetooth
SoftwareSerial BTserial(10, 11); //(TX, RX)

//LCD
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, A4);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 290);

//Colours for theme
int BACKGROUND_COLOUR, PRIMARY_COLOUR, SECONDARY_COLOUR, TERTIARY_COLOUR, QUATERNARY_COLOUR, BORDER_COLOUR, TEXT_COLOUR_1, TEXT_COLOUR_2, TEXT_COLOUR_3, HIGHLIGHT_COLOUR, THEME_BUTTON_COLOUR;

//Arduino pin configurations
int anodes[8] = {46, 47, 48, 49, 50, 51, 52, 53};
int cathodes[8] = {38, 39, 40, 41, 42, 43, 44, 45};
int hallRx[8] = {A13, A11, A9, A6, 2, 4, 6, 8};
int hallTx[8] = {A14, A12, A10, A7, 3, 5, 7, 9};

//Pixel bounds in format [topLeftX, topLeftY] for each button
int mainScreenBounds[2][2] = {};
int modeSelectScreenBounds[3][2] = {};
int gameScreenBounds[3][2] = {};
int okButtonBounds[1][2] = {};
int themeButtonBounds[1][2] = {};
int promotionScreenBounds[4][2] = {};
int confirmationScreenBounds[2][2] = {};

//Screen variables
Theme currentTheme = LIGHT;
Screen currentScreen = MAIN_SCREEN;

//Game state variables
UserMode currentUserMode = NORMAL;	//0 for beginner mode, 1 for normal mode, 2 for engine mode
GameState afterError = GAME_ACTIVE;
char currentBluetoothGameState = 'n';											   //n for not active, s for started (game active), b for black resign, w for white resign, d for draw by agreement
char currentFen[100] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"; //Starting position
double time = 0.0;
char inputStr[100] = "";
char *castlingStatus = "KQkq";
char whoseTurn = WHITE;
int numTurns = 1;
char *currentEngineMove = "N/A";

//Promotion variables
ChessPiece selectedPromotion = QUEEN;
bool promoting = false;
bool pawnExchangedForPromotion = false;
Square *promotionSquare = NULL;

//Variables to track piece movement
Square *liftedSquare = NULL;
Square *toSquare = NULL;
Square *fromSquare = NULL;
Colour liftedPieceColour = NO_COLOUR;
GameState gameState = GAME_INACTIVE;
bool liftedFlag = false;
double timeLifted = 0;
Colour topColour = BLACK;
Colour botColour = WHITE;

//Hall sensor board state
int avgData[BOARD_X][BOARD_Y][AVG_SAMPLE_SIZE];
int avgSums[BOARD_X][BOARD_Y];
int avgIndex = 0;
int rawStates[BOARD_X][BOARD_Y] = {{0, 0, 0, 0, 0, 0, 0, 0},
								   {0, 0, 0, 0, 0, 0, 0, 0},
								   {0, 0, 0, 0, 0, 0, 0, 0},
								   {0, 0, 0, 0, 0, 0, 0, 0},
								   {0, 0, 0, 0, 0, 0, 0, 0},
								   {0, 0, 0, 0, 0, 0, 0, 0},
								   {0, 0, 0, 0, 0, 0, 0, 0},
								   {0, 0, 0, 0, 0, 0, 0, 0}};

Square currentBoard[BOARD_X][BOARD_Y]; //Current board based on sensor data
Square oldBoard[BOARD_X][BOARD_Y];	   //Old board for change comparison

//Bitmap icons
//brush icon, 60x60px
const unsigned char epdBitmapBrush[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xc0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xe0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xf0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xf0,
	0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xe0,
	0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xe0,
	0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xfe, 0x00,
	0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xf8, 0x00,
	0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xe0, 0x00,
	0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xc0, 0x00,
	0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x83, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc1, 0xff, 0xf8, 0x00, 0x00,
	0x00, 0x00, 0x03, 0xe0, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x7f, 0xe0, 0x00, 0x00,
	0x00, 0x00, 0x07, 0xf8, 0x3f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfc, 0x1f, 0x80, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x7e, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x06, 0x00, 0x00, 0x00,
	0x00, 0x0f, 0xf8, 0x1f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, 0x0f, 0xc0, 0x00, 0x00, 0x00,
	0x00, 0xff, 0xff, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0x87, 0xc0, 0x00, 0x00, 0x00,
	0x03, 0xff, 0xff, 0xc3, 0x80, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xc3, 0x00, 0x00, 0x00, 0x00,
	0x07, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00,
	0x0f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00,
	0x0f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00,
	0x1f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00,
	0x1f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00,
	0x1f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00,
	0x3f, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00,
	0x7f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x3f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//knight icon, 90x90px
const unsigned char epdBitmapKnight[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff,
	0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff,
	0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xfe, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff,
	0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xc0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff,
	0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xf0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff,
	0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xf8,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff,
	0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xfc,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff,
	0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xfc,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xfe, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x0f, 0xff, 0xfc, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff,
	0xf8, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf8, 0xff, 0xff, 0xf8,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x0f, 0xff, 0xf0, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff,
	0xe0, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xc0, 0xff, 0xff, 0xf0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xc0, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0xff, 0x81, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f,
	0x81, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x01, 0xff, 0xff, 0xe0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x07, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xe0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x1f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xf0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xf8,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f,
	0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//bishop icon, 90x90px
const unsigned char epdBitmapBishop[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x7f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xc0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xe0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x19, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x39, 0xff, 0xff, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x79, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xf9, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf9, 0xff, 0xff, 0xc0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf9, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0xf9, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0xfd, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xc0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x7f, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x07, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf8, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xfc, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x1f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xfe, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x7f, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
	0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xf8,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f,
	0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//queen icon, 90x90px
const unsigned char epdBitmapQueen[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x80, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xc0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x40, 0x7f, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x3f, 0x01, 0xe0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xc0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x7f, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x07, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf8, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x07, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xfc, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xfc, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x1f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xfe, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x1f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xc0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
	0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xf8,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f,
	0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//rook icon, 90x90px
const unsigned char epdBitmapRook[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x07, 0xc1, 0xff, 0xe0, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f,
	0xc1, 0xff, 0xe0, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc1, 0xff, 0xe0, 0xfc,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc1, 0xff, 0xe0, 0xfc, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0f, 0xc1, 0xff, 0xe0, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f,
	0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xfc,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f,
	0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xfc,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f,
	0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xfc,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
	0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x3f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x7f, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xc0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
	0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xf8,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f,
	0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void setup()
{
	//Initialize communication
	Serial.begin(9600);
	BTserial.begin(9600);

	//Initialize LCD
	tft.begin(tft.readID());
	changeTheme(currentTheme);
	tft.setRotation(1);
	makeMainScreen();

	//Initialize chess board hardware
	setupHallSensors();
	setupLEDs();
	setupAvg();
	lightsOff();
	for (int i = 0; i < AVG_SAMPLE_SIZE; i++)
	{
		getHallSensorData();
	}
	initBoard();
}

//Main Arduino control loop
void loop()
{
	//Touchscreen points
	TSPoint p = ts.getPoint();
	pinMode(XM, OUTPUT);
	pinMode(YP, OUTPUT);
	if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
	{
        //Map touch to rotated screen
		p.x = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());
		p.y = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
		int x = p.y;
		int y = tft.height() - p.x;
		handleTouch(x, y);
	}
	btComm();
	stateMachine();
}

void stateMachine()
{
	getHallSensorData();
	identifyColours();

	switch (gameState)
	{ 
        case GAME_INACTIVE:
            //Handled in startGameButton() function
            break;

        case GAME_ACTIVE:
            checkPromotingPawns();
            if (promotionSquare != NULL)
            {
                gameState = PROMOTING;
                currentScreen = PROMOTION_SCREEN;
                makePromotionScreen();
            }

            if (checkPickup())
            { //Change detected on board -> PIECE_LIFTED
                if (liftedPieceColour != NO_COLOUR && liftedPieceColour == whoseTurn)
                {
                    gameState = PIECE_LIFTED;
                    whoseTurn = (whoseTurn == WHITE) ? BLACK : WHITE;
                }
                else
                {
                    gameState = ERROR;
                    currentScreen = ERROR_SCREEN;
                    makeErrorScreen("Not your turn!");
                    afterError = GAME_ACTIVE;
                }
                break;
            }
            //Handled in respective termination functions
            break;

        case PIECE_LIFTED:
            if (currentUserMode == BEGINNER)
            {
                if (liftedFlag)
                {
                    timeLifted = millis();
                    liftedFlag = false;
                }
                else if ((millis() - timeLifted) > SUSPENSION_TIME)
                { //Beginner mode and no change detected on board within timeframe -> PIECE_SUSPENDED
                    gameState = PIECE_SUSPENDED;
                    liftedFlag = true;
                    break;
                }
            }

            if (checkPlaceDown() && moveValid())
            { //Change detected on board and valid move -> GAME_ACTIVE
                gameState = GAME_ACTIVE;
                char currentFenStr[100] = "";
                boardToFen(currentFenStr);
                updateState(currentFenStr, 's', getUserModeChar(currentUserMode)); //Send position over bluetooth
            }
            else if (checkPlaceDown() && !moveValid())
            { //Change detected on board and invalid move -> ERROR
                gameState = ERROR;
                currentScreen = ERROR_SCREEN;
                makeErrorScreen("Invalid move!");
                afterError = GAME_ACTIVE;
            }
            break;

        case PIECE_SUSPENDED:
            flash();
            if (checkPlaceDown() && moveValid())
            { //Change detected on board and valid move -> GAME_ACTIVE
                gameState = GAME_ACTIVE;
                lightsOff();
            }
            else if (checkPlaceDown() && !moveValid())
            { //Change detected on board and invalid move -> ERROR
                gameState = ERROR;
                currentScreen = ERROR_SCREEN;
                makeErrorScreen("Invalid move!");
                afterError = GAME_ACTIVE;
                lightsOff();
            }
            break;

        case PROMOTING:
            checkPlaceDown();
            //Handled in promotionButton()
            //Piece selected on LCD && Piece has been switched out -> GAME_ACTIVE
            if (promoting && pawnExchangedForPromotion)
            {
                pawnExchangedForPromotion = false;
                promoting = false;
                gameState = GAME_ACTIVE;
                break;
            }
            break;

        case GAME_TERMINATED:
            //Handled in terminationOkButton()
            break;

        case ERROR:
            //Handled in errorOkButton()
            break;
	}
	updateBoard();
}

void setupHallSensors()
{
	pinMode(CS, OUTPUT);
	pinMode(CLK, OUTPUT);

	for (int i = 0; i < BOARD_X; i++)
	{
		pinMode(hallRx[i], INPUT);
		pinMode(hallTx[i], OUTPUT);
	}
}

//Since our Arduino has too many ADC's and not enough SPI pins, we need to read the chips manually.
//The code below is taken from Adafruit's MCP3008.h library and modified for our specific usage.
int readHallSensorData(int adcnum, int rx, int tx)
{
	digitalWrite(CS, HIGH);
	digitalWrite(CLK, LOW);
	digitalWrite(CS, LOW);

	int commandout = adcnum;
	commandout |= 0x18;
	commandout <<= 3;

	for (int i = 0; i < 5; i++)
	{
		if (commandout & 0x80)
		{
			digitalWrite(tx, HIGH);
		}
		else
		{
			digitalWrite(tx, LOW);
		}

		commandout <<= 1;
		digitalWrite(CLK, HIGH);
		digitalWrite(CLK, LOW);
	}

	int adcout = 0;

	for (int i = 0; i < 12; i++)
	{
		digitalWrite(CLK, HIGH);
		digitalWrite(CLK, LOW);

		adcout <<= 1;

		if (digitalRead(rx))
		{
			adcout |= 0x1;
		}
	}

	digitalWrite(CS, HIGH);

	adcout >>= 1;
	return adcout;
}

//Collecting average hall sensor data to smooth out noise
void getHallSensorData()
{
	for (int i = 0; i < BOARD_Y; i++)
	{
		for (int j = 0; j < BOARD_X; j++)
		{
			rawStates[i][j] = runningAverage(readHallSensorData(j, hallRx[i], hallTx[i]), i, j);
		}
	}

	//Some hall sensors are broken. Values can be hard coded as follows
	// rawStates[1][0] = 20; //Hardcoded black piece
	// rawStates[5][0] = 200; //Hardcoded empty square
	// rawStates[7][0] = 450; //Hardcoded white piece
}

//Setup 3D array of 0's
void setupAvg()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			for (int k = 0; k < AVG_SAMPLE_SIZE; k++)
			{
				avgData[i][j][k] = 0;
			}
			avgSums[i][j] = 0;
		}
	}
}

//Compute running average of hall sensor
int runningAverage(int value, int row, int col)
{
	int temp = avgData[row][col][avgIndex];
	avgSums[row][col] += value - temp;
	avgData[row][col][avgIndex] = value;

	if (row == 7 && col == 7)
	{
		avgIndex = (avgIndex + 1) % AVG_SAMPLE_SIZE;
	}
	return avgSums[row][col] / AVG_SAMPLE_SIZE;
}

//Initializes currentBoard with the starting position
void initBoard()
{
	ChessPiece backRow[BOARD_X] = {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK};
	for (int i = 0; i < BOARD_Y; i++)
	{ //Rows
		for (int j = 0; j < BOARD_X; j++)
		{ //Columns
			switch (i)
			{
			case 0:
				currentBoard[i][j].piece = backRow[j];
				currentBoard[i][j].colour = topColour;
				break;
			case 7:
				currentBoard[i][j].piece = backRow[j];
				currentBoard[i][j].colour = botColour;
				break;
			case 1:
				currentBoard[i][j].piece = PAWN;
				currentBoard[i][j].colour = topColour;
				break;
			case 6:
				currentBoard[i][j].piece = PAWN;
				currentBoard[i][j].colour = botColour;
				break;
			default:
				currentBoard[i][j].piece = NO_PIECE;
				currentBoard[i][j].colour = NO_COLOUR;
				break;
			}
			currentBoard[i][j].row = i;
			currentBoard[i][j].col = j;
		}
	}
	getColourOrientation();
	updateBoard();
	whoseTurn = WHITE;
}

//Sets which colour pieces are on which side of the board
//Should only be called in the starting position
void getColourOrientation()
{
	identifyColours();
	topColour = currentBoard[0][0].colour;
	botColour = currentBoard[7][0].colour;
}

//User action was accepted, update saved board state with current state.
void updateBoard()
{
	for (int i = 0; i < BOARD_Y; i++)
	{
		for (int j = 0; j < BOARD_X; j++)
		{
			oldBoard[i][j] = currentBoard[i][j];
		}
	}
}

//Read a row of the chess board and translate it into a FEN string
void rowToFen(Square row[8], char *rowFen)
{
	int emptySquares = 0;
	for (int i = 0; i < BOARD_X; i++)
	{
		ChessPiece piece = row[i].piece;
		Colour colour = row[i].colour;
		if (piece == NO_PIECE)
		{
			emptySquares++;
		}
		else
		{ //Piece
			if (emptySquares > 0)
			{
				char numChar = emptySquares + '0';
				strncat(rowFen, &numChar, 1);
				emptySquares = 0;
			}
			char pieceChar = pieceToChar(piece, colour);
			strncat(rowFen, &pieceChar, 1);
		}
	}
	if (emptySquares > 0)
	{
		char numChar = emptySquares + '0';
		strncat(rowFen, &numChar, 1);
	}
}

//Generate the FEN string of the board
void boardToFen(char *boardFen)
{
	for (int i = 0; i < BOARD_Y - 1; i++)
	{
		char row[9] = "";
		rowToFen(currentBoard[i], row);
		strcat(boardFen, row);
		strncat(boardFen, "/", 1);
	}
	char row[9] = "";
	rowToFen(currentBoard[BOARD_Y - 1], row);
	strcat(boardFen, row);

	//Active colour
	char space = ' ';
	strncat(boardFen, &space, 1);
	strncat(boardFen, &whoseTurn, 1);

	//Castling availability
	strncat(boardFen, &space, 1);
	strcat(boardFen, castlingStatus);

	//En passant target square
	strncat(boardFen, &space, 1);
	strncat(boardFen, "-", 1);

	//Halfmove clock
	strncat(boardFen, &space, 1);
	strncat(boardFen, "0", 1);

	//Fullmove number
	strncat(boardFen, &space, 1);
	char fullmoves = numTurns + '0';
	strncat(boardFen, &fullmoves, 1);
}

//Translate a piece to its FEN character representation
char pieceToChar(ChessPiece piece, Colour colour)
{
	char pieceChar;
	switch (piece)
	{
	case NO_PIECE:
		pieceChar = '0';
		break;
	case PAWN:
		pieceChar = 'P';
		break;
	case KNIGHT:
		pieceChar = 'N';
		break;
	case BISHOP:
		pieceChar = 'B';
		break;
	case ROOK:
		pieceChar = 'R';
		break;
	case QUEEN:
		pieceChar = 'Q';
		break;
	case KING:
		pieceChar = 'K';
		break;
	}
	if (colour == BLACK)
	{
		pieceChar += 32; //Uppercase to lowercase
	}
	return pieceChar;
}

//Check if the software board representation is in the starting chess position 
bool validStartingPosition()
{
	char *currentFenStr = "";
	char *startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"; //Starting position
	boardToFen(currentFenStr);
	return (strcmp(currentFenStr, startingFen) == 0);
}

//Check if the hall sensors read the board to be in the starting chess position
bool validStartingBoard()
{
	for (int i = 0; i < BOARD_Y; i++)
	{
		for (int j = 0; j < BOARD_X; j++)
		{
			switch (i)
			{
			case 0:
			case 1:
				if (currentBoard[i][j].colour != topColour)
				{
					return false;
				}
				break;
			case 2:
			case 3:
			case 4:
			case 5:
				if (currentBoard[i][j].colour != NO_COLOUR)
				{
					return false;
				}
				break;
			case 6:
			case 7:
				if (currentBoard[i][j].colour != botColour)
				{
					return false;
				}
				break;
			}
		}
	}
	return true;
}

//Ignoring the moved piece, the board is in the starting position
//Must be called after checkPlacedown()
bool otherwiseStartingBoard()
{
	int fromRow = (*fromSquare).row;
	int fromCol = (*fromSquare).col;
	int toRow = (*toSquare).row;
	int toCol = (*toSquare).col;
	for (int i = 0; i < BOARD_Y; i++)
	{
		for (int j = 0; j < BOARD_X; j++)
		{
			if ((fromRow != i || fromCol != j) && (toRow != i || toCol != j))
			{ //Not square that piece moved to or from
				switch (i)
				{
				case 0:
				case 1:
					if (currentBoard[i][j].colour != topColour)
					{
						return false;
					}
					break;
				case 2:
				case 3:
				case 4:
				case 5:
					if (currentBoard[i][j].colour != NO_COLOUR)
					{
						return false;
					}
					break;
				case 6:
				case 7:
					if (currentBoard[i][j].colour != botColour)
					{
						return false;
					}
					break;
				}
			}
		}
	}
	return true;
}

//Which colour pieces are on which squares according to the hall sensors
void identifyColours()
{
	for (int i = 0; i < BOARD_Y; i++)
	{
		for (int j = 0; j < BOARD_X; j++)
		{
			if (rawStates[i][j] > WHITE_HALL)
			{ //White piece according to hall sensor
				currentBoard[i][j].colour = WHITE;
			}
			else if (rawStates[i][j] < BLACK_HALL)
			{ //Black piece according to hall sensor
				currentBoard[i][j].colour = BLACK;
			}
			else
			{ //No piece according to hall sensor
				currentBoard[i][j].colour = NO_COLOUR;
			}
		}
	}
}

//Check if a piece has been picked up
bool checkPickup()
{
	for (int i = 0; i < BOARD_Y; i++)
	{
		for (int j = 0; j < BOARD_X; j++)
		{
			if (currentBoard[i][j].colour != oldBoard[i][j].colour)
			{
				if (liftedSquare == NULL)
				{
					printHall();
					liftedSquare = &currentBoard[i][j];
					liftedPieceColour = oldBoard[i][j].colour;
					fromSquare = new Square(currentBoard[i][j].piece, liftedPieceColour, i, j);
					Serial.print("Pickup on square: ");
					Serial.print(i);
					Serial.print(", ");
					Serial.println(j);
					updateBoard();
					return true;
				}
			}
		}
	}
	return false;
}

//Check if the lifted piece has been placed down
bool checkPlaceDown()
{
	checkPickup();
	for (int i = 0; i < BOARD_Y; i++)
	{
		for (int j = 0; j < BOARD_X; j++)
		{
			if (gameState == PROMOTING && liftedSquare != NULL && (i == 0 || i == 7) && liftedSquare->col == promotionSquare->col)
			{
				//Successfully exchanged same piece as promoting square
				promotionSquare = NULL;
				pawnExchangedForPromotion = true;
				return true;
			}
			else if (gameState == PROMOTING && liftedSquare != NULL && (i == 0 || i == 7) && liftedSquare->colour != promotionSquare->colour)
			{
				//Another piece was moved other than the promoting square
				gameState = ERROR;
				currentScreen = ERROR_SCREEN;
				makeErrorScreen("Please select a piece for promotion!");
				afterError = PROMOTING;
				Serial.println("Moving to ERROR");
				return false;
			}
			else if (currentBoard[i][j].colour != oldBoard[i][j].colour)
			{
				//Normal game play, no promotion
				if (liftedSquare != NULL && liftedPieceColour == currentBoard[i][j].colour)
				{
					//Record lifted square in the current board configuration
					currentBoard[i][j] = Square((*liftedSquare).piece, liftedPieceColour, i, j);
					toSquare = new Square(currentBoard[i][j].piece, currentBoard[i][j].colour, i, j);
					int origRow = (*liftedSquare).row;
					int origCol = (*liftedSquare).col;
					currentBoard[origRow][origCol] = Square(origRow, origCol);

					//Reset lifted piece variables to nothing
					liftedSquare = NULL;
					liftedPieceColour = NO_COLOUR;
					Serial.print("Placedown on square: ");
					Serial.print(i);
					Serial.print(", ");
					Serial.println(j);

					//Save board state
					updateBoard();
					return true;
				}
			}
		}
	}
	return false;
}

//Validate move against the standard rules of chess
bool moveValid()
{
	Square from = *fromSquare;
	Square to = *toSquare;

	Square possibleMoves[QUEEN_SIGHT] = {};
	getValidMoves(from, possibleMoves);

	int i = 0;
	while (possibleMoves[i].row != -1 && possibleMoves[i].col != -1)
	{
		if (possibleMoves[i++] == *toSquare)
		{
			return true;
		}
	}
	return false;
}

//Check to see if a pawn made it to the other side for promotion
void checkPromotingPawns()
{
	for (int i = 0; i < BOARD_X; i++)
	{
		if (currentBoard[0][i].piece == PAWN)
		{
			promotionSquare = new Square(currentBoard[0][i].piece, currentBoard[0][i].colour, 0, i);
			return;
		}
		else if (currentBoard[7][i].piece == PAWN)
		{
			promotionSquare = new Square(currentBoard[7][i].piece, currentBoard[7][i].colour, 7, i);
			return;
		}
	}
	promotionSquare = NULL;
}

//Exchange pawn piece for another piece
void promotePawn(ChessPiece promotionPiece)
{
	int row = (*promotionSquare).row;
	int col = (*promotionSquare).col;
	currentBoard[row][col].piece = promotionPiece;
}

//Light valid moves
void highlightMoves(Square squares[])
{
	int i = 0;
	Square nextSquare = squares[i++];
	while (nextSquare.row >= 0 && nextSquare.col >= 0)
	{
		lightUp(nextSquare.row, nextSquare.col);
		nextSquare = squares[i++];
	}
}

//Algorithm to determine valid moves for each piece. Used for Beginner mode lights
void getValidMoves(Square startingSquare, Square possibleMoves[])
{
	switch (startingSquare.piece)
	{
	case PAWN:
		Square possiblePawnMoves[PAWN_SIGHT] = {};
		getPawnMoves(startingSquare, possiblePawnMoves);
		memcpy(possibleMoves, possiblePawnMoves, sizeof(possiblePawnMoves));
		break;
	case KNIGHT:
		Square possibleKnightMoves[KNIGHT_SIGHT] = {};
		getKnightMoves(startingSquare, possibleKnightMoves);
		memcpy(possibleMoves, possibleKnightMoves, sizeof(possibleKnightMoves));
		break;
	case BISHOP:
		Square possibleBishopMoves[BISHOP_SIGHT] = {};
		getBishopMoves(startingSquare, possibleBishopMoves);
		memcpy(possibleMoves, possibleBishopMoves, sizeof(possibleBishopMoves));
		break;
	case ROOK:
		Square possibleRookMoves[ROOK_SIGHT] = {};
		getRookMoves(startingSquare, possibleRookMoves);
		memcpy(possibleMoves, possibleRookMoves, sizeof(possibleRookMoves));
		break;
	case QUEEN:
		Square possibleQueenMoves[QUEEN_SIGHT] = {};
		getQueenMoves(startingSquare, possibleQueenMoves);
		memcpy(possibleMoves, possibleQueenMoves, sizeof(possibleQueenMoves));
		break;
	case KING:
		Square possibleKingMoves[KING_SIGHT] = {};
		getKingMoves(startingSquare, possibleKingMoves);
		memcpy(possibleMoves, possibleKingMoves, sizeof(possibleKingMoves));
		break;
	}
}

//Get valid moves for a pawn on the starting square
void getPawnMoves(Square startingSquare, Square possibleMoves[PAWN_SIGHT])
{
	int row = startingSquare.row;
	int col = startingSquare.col;
	Colour activeColour = (*fromSquare).colour;
	int numMoves = 0; //Index and counter for possible moves

	int direction = (activeColour == WHITE) ? -1 : 1; //-1 for up, 1 for down
	int newRow = row + direction;
	if (newRow >= 0 && newRow < BOARD_X)
	{ //Target row on board
		if (currentBoard[newRow][col].piece == NO_PIECE)
		{ //Move forward 1 square if unoccupied
			possibleMoves[numMoves++] = currentBoard[newRow][col];
		}

		if (col >= 1 && currentBoard[newRow][col - 1].colour != activeColour && currentBoard[newRow][col - 1].piece != NO_PIECE)
		{ //Capture diagonally to the left
			possibleMoves[numMoves++] = currentBoard[newRow][col - 1];
		}

		if (col <= BOARD_X && currentBoard[newRow][col + 1].colour != activeColour && currentBoard[newRow][col + 1].piece != NO_PIECE)
		{ //Capture diagonally to the right
			possibleMoves[numMoves++] = currentBoard[newRow][col + 1];
		}
	}

	if ((row == 1 && direction == 1) || (row == 6 && direction == -1))
	{ //On starting row
		if (currentBoard[newRow][col].piece == NO_PIECE && currentBoard[newRow + direction][col].piece == NO_PIECE)
		{ //Move forward 2 squares if on starting row and both squares unoccupied
			possibleMoves[numMoves++] = currentBoard[newRow + direction][col];
		}
	}
}

//Get valid moves for a knight on the starting square
void getKnightMoves(Square startingSquare, Square possibleMoves[KNIGHT_SIGHT])
{
	int row = startingSquare.row;
	int col = startingSquare.col;
	Colour activeColour = (*fromSquare).colour;
	int numMoves = 0; //Index and counter for possible moves

	int moves[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
	for (int i = 0; i < 8; i++)
	{
		//For every directional move
		int newRow = row + moves[i][0];
		int newCol = col + moves[i][1];
		if (newRow >= 0 && newRow < BOARD_Y && newCol >= 0 && newCol < BOARD_X)
		{ //Target row/col on board
			Square targetSquare = currentBoard[newRow][newCol];
			if (targetSquare.colour != activeColour)
			{
				possibleMoves[numMoves++] = currentBoard[newRow][newCol];
			}
		}
	}
}

//Get valid moves for a bishop on the starting square
void getBishopMoves(Square startingSquare, Square possibleMoves[BISHOP_SIGHT])
{
	int row = startingSquare.row;
	int col = startingSquare.col;
	Colour activeColour = (*fromSquare).colour;
	int numMoves = 0; //Index and counter for possible moves

	//Up and left
	int newRow = row - 1;
	int newCol = col - 1;
	while (newRow >= 0 && newRow < BOARD_Y && newCol >= 0 && newCol < BOARD_X)
	{ //Target row/col on board
		Square targetSquare = currentBoard[newRow][newCol];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[newRow][newCol];
		}
		if (targetSquare.piece != NO_PIECE)
		{ //Target square has opposing piece
			break;
		}
		newRow -= 1;
		newCol -= 1;
	}

	//Up and right
	newRow = row - 1;
	newCol = col + 1;
	while (newRow >= 0 && newRow < BOARD_Y && newCol >= 0 && newCol < BOARD_X)
	{ //Target row/col on board
		Square targetSquare = currentBoard[newRow][newCol];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[newRow][newCol];
		}
		if (targetSquare.piece != NO_PIECE)
		{ //Target square has opposing piece
			break;
		}
		newRow -= 1;
		newCol += 1;
	}

	//Down and left
	newRow = row + 1;
	newCol = col - 1;
	while (newRow >= 0 && newRow < BOARD_Y && newCol >= 0 && newCol < BOARD_X)
	{ //Target row/col on board
		Square targetSquare = currentBoard[newRow][newCol];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[newRow][newCol];
		}
		if (targetSquare.piece != NO_PIECE)
		{ //Target square has opposing piece
			break;
		}
		newRow += 1;
		newCol -= 1;
	}

	//Down and right
	newRow = row + 1;
	newCol = col + 1;
	while (newRow >= 0 && newRow < BOARD_Y && newCol >= 0 && newCol < BOARD_X)
	{ //Target row/col on board
		Square targetSquare = currentBoard[newRow][newCol];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[newRow][newCol];
		}
		if (targetSquare.piece != NO_PIECE)
		{ //Target square has opposing piece
			break;
		}
		newRow += 1;
		newCol += 1;
	}
}

//Get valid moves for a rook on the starting square
void getRookMoves(Square startingSquare, Square possibleMoves[ROOK_SIGHT])
{
	int row = startingSquare.row;
	int col = startingSquare.col;
	Colour activeColour = (*fromSquare).colour;
	int numMoves = 0; //Index and counter for possible moves

	//Up
	int newRow = row - 1;
	while (newRow >= 0 && newRow < BOARD_Y)
	{ //Target row on board
		Square targetSquare = currentBoard[newRow][col];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[newRow][col];
		}
		if (targetSquare.piece != NO_PIECE)
		{ //Target square has opposing piece
			break;
		}
		newRow -= 1;
	}

	//Down
	newRow = row + 1;
	while (newRow >= 0 && newRow < BOARD_Y)
	{ //Target row on board
		Square targetSquare = currentBoard[newRow][col];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[newRow][col];
		}
		if (targetSquare.piece != NO_PIECE)
		{ //Target square has opposing piece
			break;
		}
		newRow += 1;
	}

	//Left
	int newCol = col - 1;
	while (newCol >= 0 && newCol < BOARD_X)
	{ //Target col on board
		Square targetSquare = currentBoard[row][newCol];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[row][newCol];
		}
		if (targetSquare.piece != NO_PIECE)
		{ //Target square has opposing piece
			break;
		}
		newCol -= 1;
	}

	//Right
	newCol = col + 1;
	while (newCol >= 0 && newCol < BOARD_X)
	{ //Target col on board
		Square targetSquare = currentBoard[row][newCol];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[row][newCol];
		}
		if (targetSquare.piece != NO_PIECE)
		{ //Target square has opposing piece
			break;
		}
		newCol += 1;
	}
}

//Get valid moves for a queen on the starting square
void getQueenMoves(Square startingSquare, Square possibleMoves[QUEEN_SIGHT])
{
	int row = startingSquare.row;
	int col = startingSquare.col;
	Colour activeColour = (*fromSquare).colour;
	int numMoves = 0; //Index and counter for possible moves

	//Up
	int newRow = row - 1;
	while (newRow >= 0 && newRow < BOARD_Y)
	{ //Target row on board
		Square targetSquare = currentBoard[newRow][col];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[newRow][col];
		}
		if (targetSquare.piece != NO_PIECE)
		{ //Target square has opposing piece
			break;
		}
		newRow -= 1;
	}

	//Down
	newRow = row + 1;
	while (newRow >= 0 && newRow < BOARD_Y)
	{ //Target row on board
		Square targetSquare = currentBoard[newRow][col];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[newRow][col];
		}
		if (targetSquare.piece != NO_PIECE)
		{ //Target square has opposing piece
			break;
		}
		newRow += 1;
	}

	//Left
	int newCol = col - 1;
	while (newCol >= 0 && newCol < BOARD_X)
	{ //Target col on board
		Square targetSquare = currentBoard[row][newCol];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[row][newCol];
		}
		if (targetSquare.piece != NO_PIECE)
		{ //Target square has opposing piece
			break;
		}
		newCol -= 1;
	}

	//Right
	newCol = col + 1;
	while (newCol >= 0 && newCol < BOARD_X)
	{ //Target col on board
		Square targetSquare = currentBoard[row][newCol];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[row][newCol];
		}
		if (targetSquare.piece != NO_PIECE)
		{ //Target square has opposing piece
			break;
		}
		newCol += 1;
	}

	//Up and left
	newRow = row - 1;
	newCol = col - 1;
	while (newRow >= 0 && newRow < BOARD_Y && newCol >= 0 && newCol < BOARD_X)
	{ //Target row/col on board
		Square targetSquare = currentBoard[newRow][newCol];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[newRow][newCol];
		}
		if (targetSquare.piece != NO_PIECE)
		{ //Target square has opposing piece
			break;
		}
		newRow -= 1;
		newCol -= 1;
	}

	//Up and right
	newRow = row - 1;
	newCol = col + 1;
	while (newRow >= 0 && newRow < BOARD_Y && newCol >= 0 && newCol < BOARD_X)
	{ //Target row/col on board
		Square targetSquare = currentBoard[newRow][newCol];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[newRow][newCol];
		}
		if (targetSquare.piece != NO_PIECE)
		{ //Target square has opposing piece
			break;
		}
		newRow -= 1;
		newCol += 1;
	}

	//Down and left
	newRow = row + 1;
	newCol = col - 1;
	while (newRow >= 0 && newRow < BOARD_Y && newCol >= 0 && newCol < BOARD_X)
	{ //Target row/col on board
		Square targetSquare = currentBoard[newRow][newCol];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[newRow][newCol];
		}
		if (targetSquare.piece != NO_PIECE)
		{ //Target square has opposing piece
			break;
		}
		newRow += 1;
		newCol -= 1;
	}

	//Down and right
	newRow = row + 1;
	newCol = col + 1;
	while (newRow >= 0 && newRow < BOARD_Y && newCol >= 0 && newCol < BOARD_X)
	{ //Target row/col on board
		Square targetSquare = currentBoard[newRow][newCol];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[newRow][newCol];
		}
		if (targetSquare.piece != NO_PIECE)
		{ //Target square has opposing piece
			break;
		}
		newRow += 1;
		newCol += 1;
	}
}

//Get valid moves for a king on the starting square
void getKingMoves(Square startingSquare, Square possibleMoves[KING_SIGHT])
{
	int row = startingSquare.row;
	int col = startingSquare.col;
	Colour activeColour = (*fromSquare).colour;
	int numMoves = 0; //Index and counter for possible moves

	//Up
	int newRow = row - 1;
	if (newRow >= 0 && newRow < BOARD_Y)
	{ //Target row on board
		Square targetSquare = currentBoard[newRow][col];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[newRow][col];
		}
	}

	//Down
	newRow = row + 1;
	if (newRow >= 0 && newRow < BOARD_Y)
	{ //Target row on board
		Square targetSquare = currentBoard[newRow][col];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[newRow][col];
		}
	}

	//Left
	int newCol = col - 1;
	if (newCol >= 0 && newCol < BOARD_X)
	{ //Target col on board
		Square targetSquare = currentBoard[row][newCol];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[row][newCol];
		}
	}

	//Right
	newCol = col + 1;
	if (newCol >= 0 && newCol < BOARD_X)
	{ //Target col on board
		Square targetSquare = currentBoard[row][newCol];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[row][newCol];
		}
	}

	//Up and left
	newRow = row - 1;
	newCol = col - 1;
	if (newRow >= 0 && newRow < BOARD_Y && newCol >= 0 && newCol < BOARD_X)
	{ //Target row/col on board
		Square targetSquare = currentBoard[newRow][newCol];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[newRow][newCol];
		}
	}

	//Up and right
	newRow = row - 1;
	newCol = col + 1;
	if (newRow >= 0 && newRow < BOARD_Y && newCol >= 0 && newCol < BOARD_X)
	{ //Target row/col on board
		Square targetSquare = currentBoard[newRow][newCol];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[newRow][newCol];
		}
	}

	//Down and left
	newRow = row + 1;
	newCol = col - 1;
	if (newRow >= 0 && newRow < BOARD_Y && newCol >= 0 && newCol < BOARD_X)
	{ //Target row/col on board
		Square targetSquare = currentBoard[newRow][newCol];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[newRow][newCol];
		}
	}

	//Down and right
	newRow = row + 1;
	newCol = col + 1;
	if (newRow >= 0 && newRow < BOARD_Y && newCol >= 0 && newCol < BOARD_X)
	{ //Target row/col on board
		Square targetSquare = currentBoard[newRow][newCol];
		if (targetSquare.colour != activeColour)
		{ //Target square does not have same-coloured piece
			possibleMoves[numMoves++] = currentBoard[newRow][newCol];
		}
	}
}

//Light up LED based on what piece was lifted
void flash()
{
	Square square = *fromSquare;
	ChessPiece squarePiece = square.piece;
	switch (squarePiece)
	{
	case PAWN:
		Square possiblePawnMoves[PAWN_SIGHT] = {};
		getPawnMoves(square, possiblePawnMoves);
		highlightMoves(possiblePawnMoves);
		break;
	case KNIGHT:
		Square possibleKnightMoves[KNIGHT_SIGHT] = {};
		getKnightMoves(square, possibleKnightMoves);
		highlightMoves(possibleKnightMoves);
		break;
	case BISHOP:
		Square possibleBishopMoves[BISHOP_SIGHT] = {};
		getBishopMoves(square, possibleBishopMoves);
		highlightMoves(possibleBishopMoves);
		break;
	case ROOK:
		Square possibleRookMoves[ROOK_SIGHT] = {};
		getRookMoves(square, possibleRookMoves);
		highlightMoves(possibleRookMoves);
		break;
	case QUEEN:
		Square possibleQueenMoves[QUEEN_SIGHT] = {};
		getQueenMoves(square, possibleQueenMoves);
		highlightMoves(possibleQueenMoves);
		break;
	case KING:
		Square possibleKingMoves[KING_SIGHT] = {};
		getKingMoves(square, possibleKingMoves);
		highlightMoves(possibleKingMoves);
		break;
	}
}

//Setup LED control
void setupLEDs()
{
	for (int i = 0; i < BOARD_X; i++)
	{
		pinMode(anodes[i], OUTPUT);
		pinMode(cathodes[i], OUTPUT);
		digitalWrite(anodes[i], HIGH);
		digitalWrite(cathodes[i], LOW);
	}
}

//Turn on specific LED
void lightUp(int row, int col)
{
	int adjustedRow = BOARD_Y - row - 1;
	int adjustedCol = BOARD_X - col - 1;
	digitalWrite(anodes[adjustedCol], LOW);
	digitalWrite(cathodes[adjustedRow], HIGH);
}

//All LEDs off
void lightsOff()
{
	for (int i = 0; i < BOARD_X; i++)
	{
		digitalWrite(anodes[i], HIGH);
		digitalWrite(cathodes[i], LOW);
	}
}

//All LEDs on
void lightsOn()
{
	for (int i = 0; i < BOARD_X; i++)
	{
		digitalWrite(anodes[i], LOW);
		digitalWrite(cathodes[i], HIGH);
	}
}

//Debug Hall sensor data
void printHall()
{
	int i, j;

	Serial.println("\ta\tb\tc\td\te\tf\tg\th");
	for (i = 0; i < 8; i++)
	{
		Serial.print(8 - i);
		Serial.print("\t");
		for (j = 0; j < 8; j++)
		{
			Serial.print(rawStates[i][j]);
			Serial.print("\t");
		}
		Serial.print("\n");
		//delay(1);
	}
}

//Touchscreen input logic
void handleTouch(int x, int y)
{
	//Start button (main screen or mode select screen)
	if ((currentScreen == MAIN_SCREEN || currentScreen == MODE_SELECT_SCREEN) && (x > mainScreenBounds[0][0] && x < (mainScreenBounds[0][0] + MAIN_BUTTON_X) && y > mainScreenBounds[0][1] && y < (mainScreenBounds[0][1] + MAIN_BUTTON_Y)))
	{
		startGameButton();
	}

	//Mode button (main screen)
	if (currentScreen == MAIN_SCREEN && (x > mainScreenBounds[1][0] && x < (mainScreenBounds[1][0] + MAIN_BUTTON_X) && y > mainScreenBounds[1][1] && y < (mainScreenBounds[1][1] + MAIN_BUTTON_Y)))
	{
		selectModeButton();
	}

	//Beginner mode button (mode select screen)
	if (currentScreen == MODE_SELECT_SCREEN && ((millis() - time) > TOUCH_DELAY) && (x > modeSelectScreenBounds[0][0] && x < (modeSelectScreenBounds[0][0] + MODE_BUTTON_X) && y > modeSelectScreenBounds[0][1] && y < (modeSelectScreenBounds[0][1] + MODE_BUTTON_Y)))
	{
		selectMode(BEGINNER);
	}

	//Normal mode button (mode select screen)
	if (currentScreen == MODE_SELECT_SCREEN && ((millis() - time) > TOUCH_DELAY) && (x > modeSelectScreenBounds[1][0] && x < (modeSelectScreenBounds[1][0] + MODE_BUTTON_X) && y > modeSelectScreenBounds[1][1] && y < (modeSelectScreenBounds[1][1] + MODE_BUTTON_Y)))
	{
		selectMode(NORMAL);
	}

	//Engine mode button (mode select screen)
	if (currentScreen == MODE_SELECT_SCREEN && ((millis() - time) > TOUCH_DELAY) && (x > modeSelectScreenBounds[2][0] && x < (modeSelectScreenBounds[2][0] + MODE_BUTTON_X) && y > modeSelectScreenBounds[2][1] && y < (modeSelectScreenBounds[2][1] + MODE_BUTTON_Y)))
	{
		selectMode(ENGINE);
	}

	//White resign button (game screen)
	if (currentScreen == GAME_SCREEN && (x > gameScreenBounds[0][0] && x < (gameScreenBounds[0][0] + GAME_BUTTON_X) && y > gameScreenBounds[0][1] && (y < gameScreenBounds[0][1] + GAME_BUTTON_Y)))
	{
		resignWhiteButton();
	}

	//Draw button (game screen)
	if (currentScreen == GAME_SCREEN && (x > gameScreenBounds[1][0] && x < (gameScreenBounds[1][0] + GAME_BUTTON_X) && y > gameScreenBounds[1][1] && (y < gameScreenBounds[1][1] + GAME_BUTTON_Y)))
	{
		drawButton();
	}

	//Black resign button (game screen)
	if (currentScreen == GAME_SCREEN && (x > gameScreenBounds[2][0] && x < (gameScreenBounds[2][0] + GAME_BUTTON_X) && y > gameScreenBounds[2][1] && (y < gameScreenBounds[2][1] + GAME_BUTTON_Y)))
	{
		resignBlackButton();
	}

	//Queen button (promotion screen)
	if (currentScreen == PROMOTION_SCREEN && (x > promotionScreenBounds[0][0] && x < (promotionScreenBounds[0][0] + PROMOTION_COMMON_X) && y > promotionScreenBounds[0][1] && (y < promotionScreenBounds[0][1] + PROMOTION_Y)))
	{
		selectedPromotion = QUEEN;
		promotionButton(selectedPromotion);
	}

	//Rook button (promotion screen)
	if (currentScreen == PROMOTION_SCREEN && (x > promotionScreenBounds[1][0] && x < (promotionScreenBounds[1][0] + PROMOTION_RARE_X) && y > promotionScreenBounds[1][1] && (y < promotionScreenBounds[1][1] + PROMOTION_Y)))
	{
		currentScreen = CONFIRMATION_SCREEN;
		makeConfirmationScreen(ROOK);
	}

	//Bishop button (promotion screen)
	if (currentScreen == PROMOTION_SCREEN && (x > promotionScreenBounds[2][0] && x < (promotionScreenBounds[2][0] + PROMOTION_RARE_X) && y > promotionScreenBounds[2][1] && (y < promotionScreenBounds[2][1] + PROMOTION_Y)))
	{
		currentScreen = CONFIRMATION_SCREEN;
		makeConfirmationScreen(BISHOP);
	}

	//Knight button (promotion screen)
	if (currentScreen == PROMOTION_SCREEN && (x > promotionScreenBounds[3][0] && x < (promotionScreenBounds[3][0] + PROMOTION_COMMON_X) && y > promotionScreenBounds[3][1] && (y < promotionScreenBounds[3][1] + PROMOTION_Y)))
	{
		currentScreen = CONFIRMATION_SCREEN;
		makeConfirmationScreen(KNIGHT);
	}

	//OK button (end screen)
	if (currentScreen == TERMINATION_SCREEN && (x > okButtonBounds[0][0] && x < (okButtonBounds[0][0] + OK_BUTTON_X) && y > okButtonBounds[0][1] && (y < okButtonBounds[0][1] + OK_BUTTON_Y)))
	{
		terminationOkButton();
	}

	//Theme button
	if ((currentScreen != PROMOTION_SCREEN && currentScreen != TERMINATION_SCREEN && currentScreen != ERROR_SCREEN && currentScreen != CONFIRMATION_SCREEN) && (x > themeButtonBounds[0][0] && x < (themeButtonBounds[0][0] + THEME_BUTTON_X) && y > themeButtonBounds[0][1] && (y < themeButtonBounds[0][1] + THEME_BUTTON_Y)))
	{
		themeButton();
	}

	//Yes confirmation
	if (currentScreen == CONFIRMATION_SCREEN && ((millis() - time) > TOUCH_DELAY) && (x > confirmationScreenBounds[0][0] && x < (confirmationScreenBounds[0][0] + YES_NO_X) && y > confirmationScreenBounds[0][1] && (y < confirmationScreenBounds[0][1] + YES_NO_Y)))
	{
		promotionButton(selectedPromotion);
	}

	//No confirmation
	if (currentScreen == CONFIRMATION_SCREEN && ((millis() - time) > TOUCH_DELAY) && (x > confirmationScreenBounds[1][0] && x < (confirmationScreenBounds[1][0] + YES_NO_X) && y > confirmationScreenBounds[1][1] && (y < confirmationScreenBounds[1][1] + YES_NO_Y)))
	{
		makePromotionScreen();
	}

	if (currentScreen == ERROR_SCREEN && (x > okButtonBounds[0][0] && x < (okButtonBounds[0][0] + OK_BUTTON_X) && y > okButtonBounds[0][1] && (y < okButtonBounds[0][1] + OK_BUTTON_Y)))
	{
		errorOkButton();
	}
}

void btComm()
{
	//Receive data from external device
	if (BTserial.available())
	{
		char c = BTserial.read(); //Comes in form "FEN@code@code\r\n"
		if (c == -1)
		{
			return;
		}
		if (c == '\n')
		{
			parseWebPayload(inputStr);
			memset(inputStr, 0, 10);
		}
		else if (c == '\r')
		{
			;
		}
		else
		{
			strncat(inputStr, &c, 1);
		}
	}

	//Send data to external device
	if (Serial.available())
	{
		BTserial.write(Serial.read());
	}
}

//Draw main screen on the LCD
void makeMainScreen()
{
	tft.fillScreen(BACKGROUND_COLOUR);
	int paddingX = 5;
	int paddingY = 5;
	int x = paddingX;
	int y = 2 * paddingY + THEME_BUTTON_Y;

	//Border and background
	tft.fillRect(x, y, MAIN_BUTTON_X + paddingX, MAIN_BUTTON_Y, SECONDARY_COLOUR);
	tft.drawRect(x, y, MAIN_BUTTON_X + paddingX, MAIN_BUTTON_Y, BORDER_COLOUR);
	mainScreenBounds[0][0] = x;
	mainScreenBounds[0][1] = y;

	tft.fillRect(3 * paddingX + MAIN_BUTTON_X, y, MAIN_BUTTON_X, MAIN_BUTTON_Y, PRIMARY_COLOUR);
	tft.drawRect(3 * paddingX + MAIN_BUTTON_X, y, MAIN_BUTTON_X, MAIN_BUTTON_Y, BORDER_COLOUR);
	mainScreenBounds[1][0] = 3 * paddingX + MAIN_BUTTON_X;
	mainScreenBounds[1][1] = y;

	//Game initialization buttons
	tft.setTextColor(TEXT_COLOUR_1);
	tft.setTextSize(6);
	int textInset = (MAIN_BUTTON_X - getPixelWidth("Start", 6)) / 2;
	tft.setCursor(mainScreenBounds[0][0] + textInset, mainScreenBounds[0][1] + 40);
	tft.println("Start");

	textInset = (MAIN_BUTTON_X - getPixelWidth("Game", 6)) / 2;
	tft.setCursor(mainScreenBounds[0][0] + textInset, tft.getCursorY() + paddingY);
	tft.println("Game");

	//Game mode select buttons
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

	//Logo
	tft.setCursor(5, 18);
	tft.setTextColor(TEXT_COLOUR_2);
	tft.setTextSize(5);
	tft.print("Chess Connect");

	makeThemeButton();
}

//Draw mode select screen on the LCD
void makeModeSelectScreen()
{
	int paddingX = 5;
	int paddingY = 5;
	int x = 3 * paddingX + MAIN_BUTTON_X;
	int y = 3 * paddingY + THEME_BUTTON_Y + MODE_BUTTON_Y;
	tft.fillRect(x, 2 * paddingY + THEME_BUTTON_Y, MODE_BUTTON_X, 3 * MODE_BUTTON_Y + 2 * paddingY, BACKGROUND_COLOUR);
	if (currentUserMode == BEGINNER)
	{
		//Highlight Beginner mode button
		tft.fillRect(x, y - MODE_BUTTON_Y - 5, MODE_BUTTON_X, MODE_BUTTON_Y, HIGHLIGHT_COLOUR);
		tft.drawRect(x, y - MODE_BUTTON_Y - 5, MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
		modeSelectScreenBounds[0][0] = x;
		modeSelectScreenBounds[0][1] = y - MODE_BUTTON_Y - 5;
		int textInset = (MAIN_BUTTON_X - getPixelWidth("Beginner", 4)) / 2;
		tft.setCursor(modeSelectScreenBounds[0][0] + textInset, modeSelectScreenBounds[0][1] + 24);
		tft.setTextColor(TEXT_COLOUR_3);
		tft.setTextSize(4);
		tft.print("Beginner");

		//Normal mode button
		tft.fillRect(x, y, MODE_BUTTON_X, MODE_BUTTON_Y, PRIMARY_COLOUR);
		tft.drawRect(x, y, MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
		modeSelectScreenBounds[1][0] = x;
		modeSelectScreenBounds[1][1] = y;
		textInset = (MAIN_BUTTON_X - getPixelWidth("Normal", 4)) / 2;
		tft.setCursor(modeSelectScreenBounds[1][0] + textInset, modeSelectScreenBounds[1][1] + 24);
		tft.setTextColor(TEXT_COLOUR_1);
		tft.setTextSize(4);
		tft.print("Normal");

		//Engine mode button
		tft.fillRect(x, y + MODE_BUTTON_Y + 5, MODE_BUTTON_X, MODE_BUTTON_Y, PRIMARY_COLOUR);
		tft.drawRect(x, y + MODE_BUTTON_Y + 5, MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
		modeSelectScreenBounds[2][0] = x;
		modeSelectScreenBounds[2][1] = y + MODE_BUTTON_Y + 5;
		textInset = (MAIN_BUTTON_X - getPixelWidth("Engine", 4)) / 2;
		tft.setCursor(modeSelectScreenBounds[2][0] + textInset, modeSelectScreenBounds[2][1] + 24);
		tft.setTextColor(TEXT_COLOUR_1);
		tft.setTextSize(4);
		tft.print("Engine");
	}
	else if (currentUserMode == NORMAL)
	{
		//Highlight Normal
		tft.fillRect(x, y - MODE_BUTTON_Y - 5, MODE_BUTTON_X, MODE_BUTTON_Y, PRIMARY_COLOUR);
		tft.drawRect(x, y - MODE_BUTTON_Y - 5, MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
		modeSelectScreenBounds[0][0] = x;
		modeSelectScreenBounds[0][1] = y - MODE_BUTTON_Y - 5;
		int textInset = (MAIN_BUTTON_X - getPixelWidth("Beginner", 4)) / 2;
		tft.setCursor(modeSelectScreenBounds[0][0] + textInset, modeSelectScreenBounds[0][1] + 24);
		tft.setTextColor(TEXT_COLOUR_1);
		tft.setTextSize(4);
		tft.print("Beginner");

		tft.fillRect(x, y, MODE_BUTTON_X, MODE_BUTTON_Y, HIGHLIGHT_COLOUR);
		tft.drawRect(x, y, MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
		modeSelectScreenBounds[1][0] = x;
		modeSelectScreenBounds[1][1] = y;
		textInset = (MAIN_BUTTON_X - getPixelWidth("Normal", 4)) / 2;
		tft.setCursor(modeSelectScreenBounds[1][0] + textInset, modeSelectScreenBounds[1][1] + 24);
		tft.setTextColor(TEXT_COLOUR_3);
		tft.setTextSize(4);
		tft.print("Normal");

		tft.fillRect(x, y + MODE_BUTTON_Y + 5, MODE_BUTTON_X, MODE_BUTTON_Y, PRIMARY_COLOUR);
		tft.drawRect(x, y + MODE_BUTTON_Y + 5, MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
		modeSelectScreenBounds[2][0] = x;
		modeSelectScreenBounds[2][1] = y + MODE_BUTTON_Y + 5;
		textInset = (MAIN_BUTTON_X - getPixelWidth("Engine", 4)) / 2;
		tft.setCursor(modeSelectScreenBounds[2][0] + textInset, modeSelectScreenBounds[2][1] + 24);
		tft.setTextColor(TEXT_COLOUR_1);
		tft.setTextSize(4);
		tft.print("Engine");
	}
	else if (currentUserMode == ENGINE)
	{
		//Highlight Engine
		tft.fillRect(x, y - MODE_BUTTON_Y - 5, MODE_BUTTON_X, MODE_BUTTON_Y, PRIMARY_COLOUR);
		tft.drawRect(x, y - MODE_BUTTON_Y - 5, MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
		modeSelectScreenBounds[0][0] = x;
		modeSelectScreenBounds[0][1] = y - MODE_BUTTON_Y - 5;
		int textInset = (MAIN_BUTTON_X - getPixelWidth("Beginner", 4)) / 2;
		tft.setCursor(modeSelectScreenBounds[0][0] + textInset, modeSelectScreenBounds[0][1] + 24);
		tft.setTextColor(TEXT_COLOUR_1);
		tft.setTextSize(4);
		tft.print("Beginner");

		tft.fillRect(x, y, MODE_BUTTON_X, MODE_BUTTON_Y, PRIMARY_COLOUR);
		tft.drawRect(x, y, MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
		modeSelectScreenBounds[1][0] = x;
		modeSelectScreenBounds[1][1] = y;
		textInset = (MAIN_BUTTON_X - getPixelWidth("Normal", 4)) / 2;
		tft.setCursor(modeSelectScreenBounds[1][0] + textInset, modeSelectScreenBounds[1][1] + 24);
		tft.setTextColor(TEXT_COLOUR_1);
		tft.setTextSize(4);
		tft.print("Normal");

		tft.fillRect(x, y + MODE_BUTTON_Y + 5, MODE_BUTTON_X, MODE_BUTTON_Y, HIGHLIGHT_COLOUR);
		tft.drawRect(x, y + MODE_BUTTON_Y + 5, MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
		modeSelectScreenBounds[2][0] = x;
		modeSelectScreenBounds[2][1] = y + MODE_BUTTON_Y + 5;
		textInset = (MAIN_BUTTON_X - getPixelWidth("Engine", 4)) / 2;
		tft.setCursor(modeSelectScreenBounds[2][0] + textInset, modeSelectScreenBounds[2][1] + 24);
		tft.setTextColor(TEXT_COLOUR_3);
		tft.setTextSize(4);
		tft.print("Engine");
	}
	time = millis();
}

//Draw game screen on the LCD
void makeGameScreen()
{
	tft.fillScreen(BACKGROUND_COLOUR);
	int padding = 5;
	int startY = 80;
	int x = tft.width() - GAME_BUTTON_X - padding;
	int y = startY;

	//Create buttons
	tft.fillRect(x, y, GAME_BUTTON_X, GAME_BUTTON_Y, PRIMARY_COLOUR);
	tft.drawRect(x, y, GAME_BUTTON_X, GAME_BUTTON_Y, BORDER_COLOUR);
	gameScreenBounds[0][0] = x;
	gameScreenBounds[0][1] = y;

	tft.fillRect(x, y + padding + GAME_BUTTON_Y, GAME_BUTTON_X, GAME_BUTTON_Y, SECONDARY_COLOUR);
	tft.drawRect(x, y + padding + GAME_BUTTON_Y, GAME_BUTTON_X, GAME_BUTTON_Y, BORDER_COLOUR);
	gameScreenBounds[1][0] = x;
	gameScreenBounds[1][1] = y + padding + GAME_BUTTON_Y;

	tft.fillRect(x, y + 2 * padding + 2 * GAME_BUTTON_Y, GAME_BUTTON_X, GAME_BUTTON_Y, TERTIARY_COLOUR);
	tft.drawRect(x, y + 2 * padding + 2 * GAME_BUTTON_Y, GAME_BUTTON_X, GAME_BUTTON_Y, BORDER_COLOUR);
	gameScreenBounds[2][0] = x;
	gameScreenBounds[2][1] = y + 2 * padding + 2 * GAME_BUTTON_Y;

	tft.fillRect(padding, startY, tft.width() - 3 * padding - GAME_BUTTON_X, 3 * GAME_BUTTON_Y + 2 * padding, QUATERNARY_COLOUR);
	tft.drawRect(padding, startY, tft.width() - 3 * padding - GAME_BUTTON_X, 3 * GAME_BUTTON_Y + 2 * padding, BORDER_COLOUR);

	//Add text to buttons

	//Resign white button
	int textInset = (GAME_BUTTON_X - getPixelWidth("Resign", 4)) / 2;
	tft.setCursor(gameScreenBounds[0][0] + textInset, gameScreenBounds[0][1] + 10);
	tft.setTextColor(TEXT_COLOUR_1);
	tft.setTextSize(4);
	tft.println("Resign");
	textInset = (GAME_BUTTON_X - getPixelWidth("(White)", 3)) / 2;
	tft.setTextSize(3);
	tft.setCursor(gameScreenBounds[0][0] + textInset, tft.getCursorY() + 3);
	tft.print("(White)");
	int endButton1Y = tft.getCursorY() + 3 + 3 * 8;

	//Draw button
	textInset = (GAME_BUTTON_X - getPixelWidth("Draw", 4)) / 2;
	tft.setCursor(gameScreenBounds[1][0] + textInset, gameScreenBounds[1][1] + 25);
	tft.setTextColor(TEXT_COLOUR_1);
	tft.setTextSize(4);
	tft.print("Draw");

	//Resign black button
	textInset = (GAME_BUTTON_X - getPixelWidth("Resign", 4)) / 2;
	tft.setCursor(gameScreenBounds[2][0] + textInset, gameScreenBounds[2][1] + 10);
	tft.setTextColor(TEXT_COLOUR_1);
	tft.setTextSize(4);
	tft.println("Resign");
	textInset = (GAME_BUTTON_X - getPixelWidth("(Black)", 3)) / 2;
	tft.setTextSize(3);
	tft.setCursor(gameScreenBounds[0][0] + textInset, tft.getCursorY() + 3);
	tft.print("(Black)");

	//Game mode display
	tft.setTextColor(TEXT_COLOUR_2);
	tft.setTextSize(5);
	char *currModeStr = getUserModeStr(currentUserMode);
	char fullModeStr[strlen(currModeStr) + 6] = {};
	strcat(fullModeStr, currModeStr);
	strcat(fullModeStr, " Mode");
	textInset = ((tft.width() - 2 * padding - THEME_BUTTON_X) - getPixelWidth(fullModeStr, 5)) / 2;
	tft.setCursor(padding + textInset, 20);
	tft.print(fullModeStr);

	//Chess engine calculated move display
	int paddingY = 10;
	textInset = ((tft.width() - 3 * padding - GAME_BUTTON_X) - getPixelWidth("Calculated", 3)) / 2;
	tft.setTextColor(TEXT_COLOUR_1);
	tft.setTextSize(3);
	tft.setCursor(padding + textInset, gameScreenBounds[0][1] + paddingY);
	tft.drawFastHLine(padding, gameScreenBounds[0][1] + GAME_BUTTON_Y - 1, tft.width() - 3 * padding - GAME_BUTTON_X, BORDER_COLOUR);
	tft.println("Calculated");
	textInset = ((tft.width() - 3 * padding - GAME_BUTTON_X) - getPixelWidth("Engine Move", 3)) / 2;
	tft.setCursor(padding + textInset, tft.getCursorY() + 6);
	tft.println("Engine Move");

    //Only display engine move in engine mode
	if (currentUserMode == ENGINE)
	{
		drawEngineMove(currentEngineMove);
	}
	else
	{
		textInset = ((tft.width() - 3 * padding - GAME_BUTTON_X) - getPixelWidth("N/A", 5)) / 2;
		int textInsetY = (tft.height() - (gameScreenBounds[0][1] + GAME_BUTTON_Y + padding) - 5 * 8) / 2;
		tft.setTextSize(5);
		tft.setTextColor(TEXT_COLOUR_1);
		tft.setCursor(padding + textInset, gameScreenBounds[0][1] + GAME_BUTTON_Y + textInsetY);
		tft.print("N/A");
	}

	makeThemeButton();
}

//Draw end screen on the LCD
void makeEndScreen(char *msg)
{
	tft.fillScreen(BACKGROUND_COLOUR);
	//resetState();
	int x = (tft.width() - OK_BUTTON_X) / 2;
	int y = 2 * tft.height() / 3;
	tft.fillRect(x, y, OK_BUTTON_X, OK_BUTTON_Y, PRIMARY_COLOUR);
	tft.drawRect(x, y, OK_BUTTON_X, OK_BUTTON_Y, BORDER_COLOUR);
	okButtonBounds[0][0] = x;
	okButtonBounds[0][1] = y;

	tft.setCursor(x + 15, y + 20);
	tft.setTextColor(TEXT_COLOUR_1);
	tft.setTextSize(6);
	tft.print("OK");

	//Split message into 2 lines for readability
	char *line2 = strstr(msg, "by ") + sizeof(char) * 3;
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

//Draw promotion screen on the LCD
void makePromotionScreen()
{
	currentScreen = PROMOTION_SCREEN;
	tft.fillScreen(BACKGROUND_COLOUR);

	int padding = 10;
	int marginY = tft.height() - 3 * padding - 2 * PROMOTION_Y;

	tft.setCursor(padding, padding + 6);
	tft.setTextColor(TEXT_COLOUR_2);
	tft.setTextSize(4);
	tft.println("Select a Piece for");
	tft.setCursor(padding, tft.getCursorY() + padding);
	tft.print("Pawn Promotion");

	int queenColour = SECONDARY_COLOUR;
	int rbColour = TERTIARY_COLOUR;
	int knightColour = PRIMARY_COLOUR;

	if (currentTheme == DARK)
	{
		queenColour = TERTIARY_COLOUR;
		rbColour = SECONDARY_COLOUR;
		knightColour = PRIMARY_COLOUR;
	}

	//Queen
	tft.fillRect(padding, marginY + padding, PROMOTION_COMMON_X, PROMOTION_Y, queenColour);
	tft.drawRect(padding, marginY + padding, PROMOTION_COMMON_X, PROMOTION_Y, BORDER_COLOUR);
	promotionScreenBounds[0][0] = padding;
	promotionScreenBounds[0][1] = marginY + padding;
	tft.drawBitmap(promotionScreenBounds[0][0], promotionScreenBounds[0][1] + padding / 2, epdBitmapQueen, ICON_SIZE, ICON_SIZE, BLACK);
	int textInset = (PROMOTION_COMMON_X - ICON_SIZE - getPixelWidth("Queen", 6)) / 2 - 20;
	tft.setCursor(promotionScreenBounds[0][0] + ICON_SIZE + textInset, promotionScreenBounds[0][1] + 28);
	tft.setTextColor(TEXT_COLOUR_1);
	tft.setTextSize(6);
	tft.print("Queen");

	//Rook
	tft.fillRect(PROMOTION_COMMON_X + 2 * padding, marginY + padding, PROMOTION_RARE_X, PROMOTION_Y, rbColour);
	tft.drawRect(PROMOTION_COMMON_X + 2 * padding, marginY + padding, PROMOTION_RARE_X, PROMOTION_Y, BORDER_COLOUR);
	promotionScreenBounds[1][0] = PROMOTION_COMMON_X + 2 * padding;
	promotionScreenBounds[1][1] = marginY + padding;
	tft.drawBitmap(promotionScreenBounds[1][0], promotionScreenBounds[1][1], epdBitmapRook, ICON_SIZE, ICON_SIZE, BLACK);

	//Bishop
	tft.fillRect(padding, PROMOTION_Y + marginY + 2 * padding, PROMOTION_RARE_X, PROMOTION_Y, rbColour);
	tft.drawRect(padding, PROMOTION_Y + marginY + 2 * padding, PROMOTION_RARE_X, PROMOTION_Y, BORDER_COLOUR);
	promotionScreenBounds[2][0] = padding;
	promotionScreenBounds[2][1] = PROMOTION_Y + marginY + 2 * padding;
	tft.drawBitmap(promotionScreenBounds[2][0], promotionScreenBounds[2][1] + padding / 2, epdBitmapBishop, ICON_SIZE, ICON_SIZE, BLACK);

	//Knight
	tft.fillRect(PROMOTION_RARE_X + 2 * padding, PROMOTION_Y + marginY + 2 * padding, PROMOTION_COMMON_X, PROMOTION_Y, knightColour);
	tft.drawRect(PROMOTION_RARE_X + 2 * padding, PROMOTION_Y + marginY + 2 * padding, PROMOTION_COMMON_X, PROMOTION_Y, BORDER_COLOUR);
	promotionScreenBounds[3][0] = PROMOTION_RARE_X + 2 * padding;
	promotionScreenBounds[3][1] = PROMOTION_Y + marginY + 2 * padding;
	tft.drawBitmap(promotionScreenBounds[3][0], promotionScreenBounds[3][1], epdBitmapKnight, ICON_SIZE, ICON_SIZE, BLACK);
	textInset = (PROMOTION_COMMON_X - ICON_SIZE - getPixelWidth("Knight", 6)) / 2 - 10;
	tft.setCursor(promotionScreenBounds[3][0] + ICON_SIZE + textInset, promotionScreenBounds[3][1] + 28);
	tft.setTextColor(TEXT_COLOUR_1);
	tft.setTextSize(6);
	tft.print("Knight");
}

//Draw theme button on the LCD
void makeThemeButton()
{
	int padding = 5;
	tft.fillRect(tft.width() - padding - THEME_BUTTON_X, padding, THEME_BUTTON_X, THEME_BUTTON_Y, THEME_BUTTON_COLOUR);
	tft.drawRect(tft.width() - padding - THEME_BUTTON_X, padding, THEME_BUTTON_X, THEME_BUTTON_Y, BORDER_COLOUR);
	themeButtonBounds[0][0] = tft.width() - padding - THEME_BUTTON_X;
	themeButtonBounds[0][1] = padding;
	tft.drawBitmap(themeButtonBounds[0][0] + 5, themeButtonBounds[0][1] + 5, epdBitmapBrush, 60, 60, TEXT_COLOUR_1);
}

//Draw confirmation screen on the LCD
void makeConfirmationScreen(ChessPiece piece)
{
	selectedPromotion = piece;
	tft.fillScreen(BACKGROUND_COLOUR);
	int padding = 10;
	int x = (tft.width() - 2 * YES_NO_X - padding) / 2;
	int y = 2 * tft.height() / 3;
	tft.fillRect(x, y, YES_NO_X, YES_NO_Y, PRIMARY_COLOUR);
	tft.drawRect(x, y, YES_NO_X, YES_NO_Y, BORDER_COLOUR);
	confirmationScreenBounds[0][0] = x;
	confirmationScreenBounds[0][1] = y;

	int textInset = (YES_NO_X - getPixelWidth("Yes", 5)) / 2;
	tft.setCursor(x + textInset, y + 22);
	tft.setTextColor(TEXT_COLOUR_1);
	tft.setTextSize(5);
	tft.print("Yes");

	x += YES_NO_X + padding;
	tft.fillRect(x, y, YES_NO_X, YES_NO_Y, SECONDARY_COLOUR);
	tft.drawRect(x, y, YES_NO_X, YES_NO_Y, BORDER_COLOUR);
	confirmationScreenBounds[1][0] = x;
	confirmationScreenBounds[1][1] = y;

	textInset = (YES_NO_X - getPixelWidth("No", 5)) / 2;
	tft.setCursor(x + textInset, y + 22);
	tft.setTextColor(TEXT_COLOUR_1);
	tft.setTextSize(5);
	tft.print("No");

	tft.setTextColor(TEXT_COLOUR_2);
	tft.setTextSize(5);

	x = (tft.width() - getPixelWidth("Promote to ", 5)) / 2;
	tft.setCursor(x, TERMINATION_TEXT_Y);
	tft.println("Promote to ");

	char confirmationText[10] = "a ";
	if (selectedPromotion == KNIGHT)
	{
		strcat(confirmationText, "Knight?");
	}
	else if (selectedPromotion == ROOK)
	{
		strcat(confirmationText, "Rook?");
	}
	else if (selectedPromotion == BISHOP)
	{
		strcat(confirmationText, "Bishop?");
	}

	x = (tft.width() - getPixelWidth(confirmationText, 5)) / 2;
	tft.setCursor(x, tft.getCursorY());
	tft.println(confirmationText);
	promoting = true;
	time = millis();
}

//Draw error screen on the LCD
void makeErrorScreen(char *err)
{
	tft.fillScreen(BACKGROUND_COLOUR);
	int x = (tft.width() - OK_BUTTON_X) / 2;
	int y = 2 * tft.height() / 3;
	tft.fillRect(x, y, OK_BUTTON_X, OK_BUTTON_Y, PRIMARY_COLOUR);
	tft.drawRect(x, y, OK_BUTTON_X, OK_BUTTON_Y, BORDER_COLOUR);
	okButtonBounds[0][0] = x;
	okButtonBounds[0][1] = y;

	tft.setCursor(x + 15, y + 20);
	tft.setTextColor(TEXT_COLOUR_1);
	tft.setTextSize(6);
	tft.print("OK");

	tft.setTextColor(TEXT_COLOUR_2);
	tft.setTextSize(3);

	x = (tft.width() - getPixelWidth(err, 3)) / 2;
	tft.setCursor(x, TERMINATION_TEXT_Y);
	tft.print(err);
}

//Draw the selected user mode on the LCD
void drawStartMode()
{
	char *tmp = getUserModeStr(currentUserMode);
	char mode[11] = {};
	if (strcmp(tmp, "Beginner") == 0)
	{
		strcat(mode, "(");
		strcat(mode, tmp);
		strcat(mode, ")");
	}
	else
	{
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

//Draw the engine move on the LCD
void drawEngineMove(char *move)
{
	if (currentScreen == GAME_SCREEN)
	{
		int padding = 5;
		tft.fillRect(padding, 80 + GAME_BUTTON_Y + padding, tft.width() - 3 * padding - GAME_BUTTON_X - 1, 2 * GAME_BUTTON_Y + padding, QUATERNARY_COLOUR);
		int textSize = getEngineMoveTextSize(move);
		int textInset = ((tft.width() - 3 * padding - GAME_BUTTON_X) - getPixelWidth(move, textSize)) / 2;
		int textHeight = textSize * 8;
		int textInsetY = (tft.height() - (gameScreenBounds[0][1] + GAME_BUTTON_Y + padding) - textHeight) / 2;
		tft.setCursor(padding + textInset, gameScreenBounds[0][1] + GAME_BUTTON_Y + textInsetY);
		tft.setTextSize(textSize);
		tft.setTextColor(TEXT_COLOUR_1);
		tft.print(move);
	}
	strcpy(currentEngineMove, move);
}

//Draw start game button on the LCD
void startGameButton()
{
	//start button and board in starting position -> GAME_ACTIVE
	if (gameState == GAME_INACTIVE)
	{
		if (validStartingBoard())
		{
			initBoard();
			updateState(currentFen, 's', getUserModeChar(currentUserMode));
			gameState = GAME_ACTIVE;
			currentScreen = GAME_SCREEN;
			makeGameScreen();
			Serial.println("Moving to GAME_ACTIVE");
		}
		else
		{
			Serial.println("Moving to ERROR");
			gameState = ERROR;
			currentScreen = ERROR_SCREEN;
			makeErrorScreen("Not in starting position!");
			afterError = GAME_INACTIVE;
		}
	}
}

//Draw mode select button on the LCD
void selectModeButton()
{
	currentScreen = MODE_SELECT_SCREEN;
	makeModeSelectScreen();
}

//Draw beginner mode button on the LCD
void beginnerModeButton(bool active)
{
	if (active)
	{
		tft.fillRect(modeSelectScreenBounds[0][0], modeSelectScreenBounds[0][1], MODE_BUTTON_X, MODE_BUTTON_Y, HIGHLIGHT_COLOUR);
		tft.drawRect(modeSelectScreenBounds[0][0], modeSelectScreenBounds[0][1], MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
		if (currentUserMode == NORMAL)
		{
			normalModeButton(false);
		}
		else if (currentUserMode == ENGINE)
		{
			engineModeButton(false);
		}
		currentUserMode = BEGINNER;
		drawStartMode();
		tft.setTextColor(TEXT_COLOUR_3);
	}
	else
	{
		tft.fillRect(modeSelectScreenBounds[0][0], modeSelectScreenBounds[0][1], MODE_BUTTON_X, MODE_BUTTON_Y, PRIMARY_COLOUR);
		tft.drawRect(modeSelectScreenBounds[0][0], modeSelectScreenBounds[0][1], MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
		tft.setTextColor(TEXT_COLOUR_1);
	}
	int textInset = (MODE_BUTTON_X - getPixelWidth("Beginner", 4)) / 2;
	tft.setCursor(modeSelectScreenBounds[0][0] + textInset, modeSelectScreenBounds[0][1] + 24);
	tft.setTextSize(4);
	tft.print("Beginner");
}

//Draw normal mode button on the LCD
void normalModeButton(bool active)
{
	if (active)
	{
		tft.fillRect(modeSelectScreenBounds[1][0], modeSelectScreenBounds[1][1], MODE_BUTTON_X, MODE_BUTTON_Y, HIGHLIGHT_COLOUR);
		tft.drawRect(modeSelectScreenBounds[1][0], modeSelectScreenBounds[1][1], MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
		if (currentUserMode == BEGINNER)
		{
			beginnerModeButton(false);
		}
		else if (currentUserMode == ENGINE)
		{
			engineModeButton(false);
		}
		currentUserMode = NORMAL;
		drawStartMode();
		tft.setTextColor(TEXT_COLOUR_3);
	}
	else
	{
		tft.fillRect(modeSelectScreenBounds[1][0], modeSelectScreenBounds[1][1], MODE_BUTTON_X, MODE_BUTTON_Y, PRIMARY_COLOUR);
		tft.drawRect(modeSelectScreenBounds[1][0], modeSelectScreenBounds[1][1], MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
		tft.setTextColor(TEXT_COLOUR_1);
	}
	int textInset = (MODE_BUTTON_X - getPixelWidth("Normal", 4)) / 2;
	tft.setCursor(modeSelectScreenBounds[1][0] + textInset, modeSelectScreenBounds[1][1] + 24);
	tft.setTextSize(4);
	tft.print("Normal");
}

//Draw engine mode button on the LCD
void engineModeButton(bool active)
{
	if (active)
	{
		tft.fillRect(modeSelectScreenBounds[2][0], modeSelectScreenBounds[2][1], MODE_BUTTON_X, MODE_BUTTON_Y, HIGHLIGHT_COLOUR);
		tft.drawRect(modeSelectScreenBounds[2][0], modeSelectScreenBounds[2][1], MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
		if (currentUserMode == BEGINNER)
		{
			beginnerModeButton(false);
		}
		else if (currentUserMode == NORMAL)
		{
			normalModeButton(false);
		}
		currentUserMode = ENGINE;
		drawStartMode();
		tft.setTextColor(TEXT_COLOUR_3);
	}
	else
	{
		tft.fillRect(modeSelectScreenBounds[2][0], modeSelectScreenBounds[2][1], MODE_BUTTON_X, MODE_BUTTON_Y, PRIMARY_COLOUR);
		tft.drawRect(modeSelectScreenBounds[2][0], modeSelectScreenBounds[2][1], MODE_BUTTON_X, MODE_BUTTON_Y, BORDER_COLOUR);
		tft.setTextColor(TEXT_COLOUR_1);
	}
	int textInset = (MODE_BUTTON_X - getPixelWidth("Engine", 4)) / 2;
	tft.setCursor(modeSelectScreenBounds[2][0] + textInset, modeSelectScreenBounds[2][1] + 24);
	tft.setTextSize(4);
	tft.print("Engine");
}

//White resigns
void resignWhiteButton()
{
	updateState(currentFen, 'b', getUserModeChar(currentUserMode));
	gameState = GAME_TERMINATED;
	currentScreen = TERMINATION_SCREEN;
	makeEndScreen("Black Wins by Resignation");
}

//Black resigns
void resignBlackButton()
{
	updateState(currentFen, 'w', getUserModeChar(currentUserMode));
	gameState = GAME_TERMINATED;
	currentScreen = TERMINATION_SCREEN;
	makeEndScreen("White Wins by Resignation");
}

//Draw by agreement
void drawButton()
{
	updateState(currentFen, 'd', getUserModeChar(currentUserMode));
	gameState = GAME_TERMINATED;
	currentScreen = TERMINATION_SCREEN;
	makeEndScreen("Game Drawn by Agreement");
}

//User presses OK on end screen
void terminationOkButton()
{
	gameState = GAME_INACTIVE;
	currentScreen = MAIN_SCREEN;
	resetState();
	makeMainScreen();
}

//User presses OK on error screen
void errorOkButton()
{
	switch (afterError)
	{
	case GAME_ACTIVE:
		gameState = GAME_ACTIVE;
		currentScreen = GAME_SCREEN;
		makeGameScreen();
		Serial.println("Moving to GAME_ACTIVE");
		break;
	case GAME_INACTIVE:
		gameState = GAME_INACTIVE;
		currentScreen = MAIN_SCREEN;
		makeMainScreen();
		Serial.println("Moving to GAME_INACTIVE");
		break;
	case PROMOTING:
		gameState = PROMOTING;
		currentScreen = PROMOTION_SCREEN;
		makePromotionScreen();
		Serial.println("Moving to PROMOTING");
		break;
	}
}

//Logic when theme button is pressed
void themeButton()
{
	if (currentTheme == DARK)
	{
		changeTheme(LIGHT);
	}
	else if (currentTheme == LIGHT)
	{
		changeTheme(DARK);
	}
	refreshScreen();
}

//Logic when promotion button is pressed
void promotionButton(ChessPiece piece)
{
	promotePawn(piece);
	gameState = GAME_ACTIVE; //Piece selected on LCD -> GAME_ACTIVE
	currentScreen = GAME_SCREEN;
	makeGameScreen();
	char currentFenStr[100] = "";
	boardToFen(currentFenStr);
	updateState(currentFenStr, 's', getUserModeChar(currentUserMode)); //Send position over bluetooth
}

//White checkmates black
void checkmateWhite()
{
	updateState(currentFen, 'w', getUserModeChar(currentUserMode));
	gameState = GAME_TERMINATED;
	currentScreen = TERMINATION_SCREEN;
	makeEndScreen("White Wins by Checkmate");
}

//Black checkmates white
void checkmateBlack()
{
	updateState(currentFen, 'b', getUserModeChar(currentUserMode));
	gameState = GAME_TERMINATED;
	currentScreen = TERMINATION_SCREEN;
	makeEndScreen("Black Wins by Checkmate");
}

//Game ends in stalemate
void stalemate()
{
	updateState(currentFen, 'd', getUserModeChar(currentUserMode));
	gameState = GAME_TERMINATED;
	currentScreen = TERMINATION_SCREEN;
	makeEndScreen("Game Drawn by Stalemate");
}

//Logic for selecting a user mode
void selectMode(UserMode mode)
{
	if (currentUserMode != mode)
	{
		if (mode == BEGINNER)
		{
			beginnerModeButton(true);
		}
		else if (mode == NORMAL)
		{
			normalModeButton(true);
		}
		else if (mode == ENGINE)
		{
			engineModeButton(true);
		}
	}
}

//Visually refreshes the currently displayed screen
void refreshScreen()
{
	switch (currentScreen)
	{
        case MAIN_SCREEN: //Main Screen
            makeMainScreen();
            break;
        case MODE_SELECT_SCREEN: //Mode Select Screen
            makeMainScreen();
            makeModeSelectScreen();
            break;
        case GAME_SCREEN: //Game Screen
            makeGameScreen();
            break;
	}
}

//Encode game data (chess board config, game state, and game mode) to send to web app
void updateState(char *FEN, char gameStateData, char userMode)
{
	char stateData[100] = {};
	strncat(stateData, FEN, strlen(FEN));
	strncat(stateData, "@", 1);
	strncat(stateData, &gameStateData, 1);
	strncat(stateData, "@", 1);
	strncat(stateData, &userMode, 1);
	strncat(stateData, "\r", 1);

	strcpy(currentFen, FEN);
	currentBluetoothGameState = gameStateData;
	currentUserMode = getUserModeEnum(userMode);
	sendBluetoothData(stateData);
}

//Reset all data when game ends (checkmate, draw, resign, stalemate)
void resetState()
{
	char *startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	updateState(startingFen, 'n', getUserModeChar(NORMAL));
	currentEngineMove = "N/A";
	selectedPromotion = QUEEN;
	liftedSquare = NULL;
	toSquare = NULL;
	fromSquare = NULL;
	promotionSquare = NULL;
	promoting = false;
	pawnExchangedForPromotion = false;
	whoseTurn = WHITE;
	numTurns = 1;
	liftedPieceColour = NO_COLOUR;
	liftedFlag = false;
	timeLifted = 0;
	afterError = GAME_ACTIVE;
}

//Communicate to the web app server
void sendBluetoothData(char *stateData)
{
	int len = strlen(stateData);
	for (int i = 0; i < len; i++)
	{
		BTserial.write(stateData[i]);
	}
}

//Decode data sent back from the web-abb
void parseWebPayload(char *webData)
{											 //eg. "Nf3@w12@n" or "a@a@n" if not in engine mode
	char *engineMove = strtok(webData, "@"); //eg. Nf3
	char *webCode = strtok(NULL, "@");		 //eg. 'w12', meaning white just moved, and the fullmove number is 12. Can only be 'w' (white), 'b' (black), or 'a' (filler)
	char *gameState = strtok(NULL, "@");	 //eg. 'c' checkmate, 's' stalemate', 'n' nothing

	char *turnColour;
	getSubstring(webCode, turnColour, 0, 1);
	char *moveNumStr;
	getSubstring(webCode, moveNumStr, 1, strlen(webCode));
	int moveNum = (atoi(moveNumStr) - 1) * 2;
	moveNum += (strcmp(turnColour, "b") == 0) ? 1 : 0;

	if (moveNum == 0)
	{ //Starting position
		currentEngineMove = "N/A";
		drawEngineMove(currentEngineMove);
		return;
	}

	if (strcmp(gameState, "c") == 0)
	{ //Checkmate
		if (strcmp(turnColour, "w") == 0)
		{ //White win
			checkmateWhite();
		}
		else if (strcmp(turnColour, "b") == 0)
		{
			checkmateBlack();
		}
		return;
	}
	else if (strcmp(gameState, "s") == 0)
	{ //Stalemate
		stalemate();
		return;
	}

	if (strcmp(engineMove, "a") == 0)
	{ //Not in engine mode
		currentEngineMove = "N/A";
		drawEngineMove(currentEngineMove);
		return;
	}

	if (moveNum == 0)
	{
		return;
	}

	if (strcmp(engineMove, currentEngineMove) != 0)
	{ //Display engine move
		drawEngineMove(engineMove);
	}
}

//Change the colour theme on the LCD
void changeTheme(Theme theme)
{
	if (theme == LIGHT)
	{
		BACKGROUND_COLOUR = LIGHTBROWN;
		PRIMARY_COLOUR = TEAL;
		SECONDARY_COLOUR = GREEN;
		TERTIARY_COLOUR = PURPLE;
		QUATERNARY_COLOUR = DARKGRAY;
		BORDER_COLOUR = BLACK_COLOUR;
		TEXT_COLOUR_1 = WHITE_COLOUR;
		TEXT_COLOUR_2 = BLACK_COLOUR;
		TEXT_COLOUR_3 = BLACK_COLOUR;
		HIGHLIGHT_COLOUR = MUSTARD;
		THEME_BUTTON_COLOUR = PINK;
	}
	else if (theme == DARK)
	{
		BACKGROUND_COLOUR = BLACK_COLOUR;
		PRIMARY_COLOUR = DARK_BLUEGRAY;
		SECONDARY_COLOUR = DARK_BROWN;
		TERTIARY_COLOUR = DARK_GREEN;
		QUATERNARY_COLOUR = DARK_PURPLE;
		BORDER_COLOUR = DARK_TAN;
		TEXT_COLOUR_1 = DARK_BROWNWHITE;
		TEXT_COLOUR_2 = DARK_TAN;
		TEXT_COLOUR_3 = BLACK_COLOUR;
		HIGHLIGHT_COLOUR = DARK_LIGHTBROWN;
		THEME_BUTTON_COLOUR = DARK_PINK;
	}
	currentTheme = theme;
}

//String representation of the user mode
char *getUserModeStr(UserMode mode)
{
	if (mode == BEGINNER)
	{
		return "Beginner";
	}
	else if (mode == NORMAL)
	{
		return "Normal";
	}
	else if (mode == ENGINE)
	{
		return "Engine";
	}
}

//Character representation of the user mode
char getUserModeChar(UserMode mode)
{
	if (mode == BEGINNER)
	{
		return 'b';
	}
	else if (mode == NORMAL)
	{
		return 'n';
	}
	else if (mode == ENGINE)
	{
		return 'e';
	}
}

//UserMode type representation of a user mode character
UserMode getUserModeEnum(char mode)
{
	if (mode == 'b')
	{
		return BEGINNER;
	}
	else if (mode == 'n')
	{
		return NORMAL;
	}
	else if (mode == 'e')
	{
		return ENGINE;
	}
}

//Gets the width in pixels of a string with some text size
int getPixelWidth(char *str, int textSize)
{
	int strLen = strlen(str);
	int pixelSize = textSize * 6;
	int wordPixelWidth = strLen * pixelSize;
	return wordPixelWidth;
}

//Extracts a substring from the given string
char *getSubstring(char *src, char *dest, int start, int end)
{
	memcpy(dest, &src[start], end - start);
	dest[end - start - 1] = '\0';
	return dest;
}

//Calculate the largest possible text size to display a variable-size engine move
int getEngineMoveTextSize(char *move)
{
	int padding = 5;
	int boxWidth = tft.width() - 3 * padding - GAME_BUTTON_X;
	int wordWidth, textSize;
	for (int i = 9; i > 0; i--)
	{
		wordWidth = getPixelWidth(move, i);
		textSize = i - 1;
		if (wordWidth < boxWidth)
		{
			break;
		}
	}
	return textSize;
}