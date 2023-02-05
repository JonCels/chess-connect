#include <string>
#include <iostream>
#include <stdlib.h>
#include <PieceIdentification.h>

#include <Arduino.h>
#include <SoftwareSerial.h>


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
    SoftwareSerial bluetooth(RX_PIN, TX_PIN); // create a SoftwareSerial object
}

ArduinoController::~ArduinoController()
{
}

