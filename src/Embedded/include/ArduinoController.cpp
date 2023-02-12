#include <string>
#include <iostream>
#include <stdlib.h>
#include <PieceIdentification.h>


class ArduinoController
{
public:
    /* data */
    void movePiece(Position start, Position end);
    bool isValidMove(Piece piece);
    bool removePiece(Piece piece, Position position);
    bool isCheck();
    bool isCheckmate();
    std::string translateToFEN();
    
    ArduinoController(/* args */);
    ~ArduinoController();
    
};

ArduinoController::ArduinoController(/* args */)
{
    //SoftwareSerial bluetooth(RX_PIN, TX_PIN); // create a SoftwareSerial object
}

ArduinoController::~ArduinoController()
{
}

