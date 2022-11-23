#include <Arduino.h>
#include <stdio.h>

struct PIECE
{
    int8_t type;
    String name;
    int value;
    bool isAlive;
    uint8_t position;
};

enum PieceType : int8_t
{
    None = 0,
    WhitePawn = 1,
    WhiteBishop = 2,
    WhiteKnight = 3,
    WhiteRook = 4,
    WhiteQueen = 5,
    WhiteKing = 6,
    BlackPawn = -1,
    BlackBishop = -2,
    BlackKnight = -3,
    BlackRook = -4,
    BlackQueen = -5,
    BlackKing = -6,
    Unknown = INT8_MAX,
};


int8_t ReadSensors(int sensor = 0);