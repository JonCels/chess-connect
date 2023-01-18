#include <PieceIdentification.h>

int ReadSensors(int sensor = 0)
{
    if (sensor > 0 and sensor <= 30)
    {
        return BlackPawn;
    }
    else if (sensor > 30 and sensor <= 60)
    {
        return BlackBishop;
    }
    else if (sensor > 60 and sensor <= 90)
    {
        return BlackKnight;
    }
    else if (sensor > 90 and sensor <= 120)
    {
        return BlackRook;
    }
    else if (sensor > 120 and sensor <= 150)
    {
        return BlackQueen;
    }
    else if (sensor > 150 and sensor <= 180)
    {
        return BlackKing;
    }
    else if (sensor > 220 and sensor <= 250)
    {
        return WhiteKing;
    }
    else if (sensor > 250 and sensor <= 280)
    {
        return WhiteQueen;
    }
    else if (sensor > 280 and sensor <= 310)
    {
        return WhiteRook;
    }
    else if (sensor > 310 and sensor <= 340)
    {
        return WhiteKnight;
    }
    else if (sensor > 340 and sensor <= 370)
    {
        return WhiteBishop;
    }
    else if (sensor > 370 and sensor <= 400)
    {
        return WhitePawn;
    }
    else
    {
        return PieceType::None;
        Serial.write(13);
    }
}

String GetName(PieceType piece){
    switch (piece)
    {
        case None:
            return "No Piece";
        case WhitePawn:
            return "White Pawn";
        case WhiteBishop:
            return "White Bishop";
        case WhiteKnight:
            return "White Knight";
        case WhiteRook:
            return "White Rook";
        case WhiteQueen:
            return "White Queen";
        case WhiteKing:
            return "White King";
        case BlackPawn:
            return "Black Pawn";
        case BlackBishop:
            return "Black Bishop";
        case BlackKnight:
            return "Black Knight";
        case BlackRook:
            return "Black Rook";
        case BlackQueen:
            return "Black Queen";
        case BlackKing:
            return "Black King";
    default:
        break;
    }
}
