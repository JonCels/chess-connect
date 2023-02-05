#include <string>
#include <vector>
#include <PieceIdentification.h>

const int numRows = 8;
const int numCols = 8;

// Pins to control the LEDs
const int LED_PINS[numRows + 1][numCols + 1] = {
    {2, 3, 4, 5, 6, 7, 8, 9, 10},
    {10, 11, 12, 13, 14, 15, 16, 17, 18},
    {18, 19, 20, 21, 22, 23, 24, 25, 26},
    {26, 27, 28, 29, 30, 31, 32, 33, 34},
    {34, 35, 36, 37, 38, 39, 40, 41, 42},
    {42, 43, 44, 45, 46, 47, 48, 49, 50},
    {50, 51, 52, 53, 54, 55, 56, 57, 58},
    {58, 59, 60, 61, 62, 63, 64, 65, 66}
};

// Pins to read the hall-effect sensors
const int HALL_PINS[numRows][numCols] = {
    {66, 67, 68, 69, 70, 71, 72, 73},
    {74, 75, 76, 77, 78, 79, 80, 81},
    {82, 83, 84, 85, 86, 87, 88, 89},
    {90, 91, 92, 93, 94, 95, 96, 97},
    {98, 99, 100, 101, 102, 103, 104, 105},
    {106, 107, 108, 109, 110, 111, 112, 113},
    {114, 115, 116, 117, 118, 119, 120, 121},
    {122, 123, 124, 125, 126, 127, 128, 129}
};

std::vector<Piece *> capturedPieces;

// 2D array to represent the chess board
Piece board[numRows][numCols];

std::string FEN;