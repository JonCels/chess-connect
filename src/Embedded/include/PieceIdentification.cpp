#include <PieceIdentification.h>
#include <string>

Piece IdentifyPiece(int sensor = 0)
{
    if (sensor > 0 and sensor <= 30)
    {
        return Piece{PAWN, BLACK};
    }
    else if (sensor > 30 and sensor <= 60)
    {
        return Piece{BISHOP, BLACK};
    }
    else if (sensor > 60 and sensor <= 90)
    {
        return Piece{KNIGHT, BLACK};
    }
    else if (sensor > 90 and sensor <= 120)
    {
        return Piece{ROOK, BLACK};
    }
    else if (sensor > 120 and sensor <= 150)
    {
        return Piece{QUEEN, BLACK};
    }
    else if (sensor > 150 and sensor <= 180)
    {
        return Piece{KING, BLACK};
    }
    else if (sensor > 220 and sensor <= 250)
    {
        return Piece{KING, WHITE};
    }
    else if (sensor > 250 and sensor <= 280)
    {
        return Piece{QUEEN, WHITE};
    }
    else if (sensor > 280 and sensor <= 310)
    {
        return Piece{ROOK, WHITE};
    }
    else if (sensor > 310 and sensor <= 340)
    {
        return Piece{KNIGHT, WHITE};
    }
    else if (sensor > 340 and sensor <= 370)
    {
        return Piece{BISHOP, WHITE};
    }
    else if (sensor > 370 and sensor <= 400)
    {
        return Piece{PAWN, WHITE};
    }
    else
    {
        return Piece{NO_PIECE, NO_COLOUR};
        //Serial.write(13);
    }
}

char pieceToChar(Piece piece)
{
    switch (piece.type)
    {
    case PAWN:
        return piece.colour == 0 ? 'P' : 'p';
    case KNIGHT:
        return piece.colour == 0 ? 'N' : 'n';
    case BISHOP:
        return piece.colour == 0 ? 'B' : 'b';
    case ROOK:
        return piece.colour == 0 ? 'R' : 'r';
    case QUEEN:
        return piece.colour == 0 ? 'Q' : 'q';
    case KING:
        return piece.colour == 0 ? 'K' : 'k';
    default:
        return ' ';
    }
}
