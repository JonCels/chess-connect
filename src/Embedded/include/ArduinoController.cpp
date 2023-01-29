#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <PieceIdentification.h>

struct Position
{
    int X;
    int Y;
};

class ArduinoController
{
private:
    /* data */
    void movePiece(Position start, Position end);
    bool isValidMove(Piece piece);
    bool removePiece(Piece piece, Position position);
    bool isCheck();
    bool isCheckmate();
    std::string translateToFEN();
    

public:
    ArduinoController(/* args */);
    ~ArduinoController();
    
};

ArduinoController::ArduinoController(/* args */)
{
}

ArduinoController::~ArduinoController()
{
}

