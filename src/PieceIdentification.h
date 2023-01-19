#include <Arduino.h>
#include <stdio.h>

const int RX_PIN = 2; // RX pin of the SerialCommunication module
const int TX_PIN = 3; // TX pin of the SerialCommunication module

// Enum to represent the different types of chess pieces
enum PieceType
{
    NONE,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

// Structure to represent a chess piece
struct Piece
{
    PieceType type;
    int color; // 0 for white, 1 for black
};

int8_t ReadSensors(int sensor = 0);