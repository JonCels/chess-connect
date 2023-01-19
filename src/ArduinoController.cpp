#include <string.h>
#include <stdio.h>
#include <stdlib.h>


enum COLOUR{
    white,
    black        
};

struct Piece
{
    std::string Name;
    COLOUR Colour;
    std::string FEN;
};

struct Position
{
    int X;
    int Y;
};


class ChessBoard
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
    ChessBoard(/* args */);
    ~ChessBoard();
    
};

ChessBoard::ChessBoard(/* args */)
{
}

ChessBoard::~ChessBoard()
{
}

