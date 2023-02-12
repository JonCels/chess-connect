#pragma once    
#include <ChessBoard.h>

class ChessBoard {

    Piece whiteRook = {ROOK, WHITE};

    // 2D array to represent the chess board
    Piece board[numRows][numCols] = {
        {{ROOK, WHITE}, {KNIGHT, WHITE}, {BISHOP, WHITE}, {QUEEN, WHITE}, {KING, WHITE}, {BISHOP, WHITE}, {KNIGHT, WHITE}, {ROOK, WHITE}},
        {{PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}},
        {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
        {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
        {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
        {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
        {{PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}},
        {{ROOK, BLACK}, {KNIGHT, BLACK}, {BISHOP, BLACK}, {QUEEN, BLACK}, {KING, BLACK}, {BISHOP, BLACK}, {KNIGHT, BLACK}, {ROOK, BLACK}}};

    void setup()
    {
        
        
    }

    void loop()
    {
        // Update the board based on the hall-effect sensors
        // for (int i = 0; i < numRows; i++)
        // {
        //     for (int j = 0; j < numCols; j++)
        //     {
        //         if (digitalRead(HALL_PINS[i][j]) == HIGH)
        //         {
        //             // Sensor is activated, toggle the piece
        //             Piece &piece = board[i][j];
        //             if (piece.type == NO_PIECE)
        //             {
        //                 piece.type = PAWN;
        //                 piece.colour = 0; // white
        //             }
        //             else
        //             {
        //                 piece.type = NO_PIECE;
        //                 piece.colour = -1;
        //             }
        //         }
        //     }
        // }
        FEN = boardToFen();
    }

    void LightAllPieces(){
        // Update the LEDs based on the board
        for (int i = 0; i < numRows; i++)
        {
            for (int j = 0; j < numCols; j++)
            {
                Piece piece = board[i][j];
                if (piece.type == NO_PIECE)
                {
                    LightSquare(i, j, false);
                }
                else
                {
                    LightSquare(i, j, true);
                }
            }
        }
    }

    // If on = true, light square, else turn off light
    void LightSquare(int row, int col, bool on){
        // if (on)
        // {
        //     digitalWrite(LED_PINS[row][col], HIGH);
        //     digitalWrite(LED_PINS[row][col + 1], HIGH);
        //     digitalWrite(LED_PINS[row + 1][col], HIGH);
        //     digitalWrite(LED_PINS[row + 1][col + 1], HIGH);
        // }
        // else
        // {
        //     digitalWrite(LED_PINS[row][col], LOW);
        // }
    }

    bool movePiece(int fromRow, int fromCol, int toRow, int toCol, PieceType promotionType = NO_PIECE)
    {
        Piece &fromPiece = board[fromRow][fromCol];
        Piece &toPiece = board[toRow][toCol];

        if (fromPiece.type == NO_PIECE)
        {
            return false;
        }
        if (toPiece.type != NO_PIECE && fromPiece.colour == toPiece.colour)
        {
            // Cannot capture own piece
            // illegalMove(int fromRow, int fromCol, int toRow, int toCol);
            return false;
        }
        else if (toPiece.type != NO_PIECE && fromPiece.colour != toPiece.colour) {

        }
        switch (fromPiece.type)
        {
        case PAWN:
            if (fromPiece.colour == 0)
            { // white pawn
                if (fromRow == 1 && toRow == 3 && fromCol == toCol && toPiece.type == NO_PIECE)
                {
                    // White pawn can move two squares from the second row
                    fromPiece.type = NO_PIECE;
                    toPiece = fromPiece;
                    break;
                }
                if (fromRow + 1 == toRow && fromCol == toCol && toPiece.type == NO_PIECE)
                {
                    // White pawn can move one square forward
                    fromPiece.type = NO_PIECE;
                    toPiece = fromPiece;
                    break;
                }
                if (fromRow + 1 == toRow && abs(fromCol - toCol) == 1 && toPiece.type != NO_PIECE)
                {
                    // White pawn can capture diagonally
                    fromPiece.type = NO_PIECE;
                    toPiece = fromPiece;
                    break;
                }
                if (fromRow == 4 && toRow == 5 && abs(fromCol - toCol) == 1 && toPiece.type == NO_PIECE)
                {
                    // White pawn can capture en passant
                    board[toRow - 1][toCol].type = NO_PIECE;
                    fromPiece.type = NO_PIECE;
                    toPiece = fromPiece;
                    break;
                }
            }
            else
            { // black pawn
                if (fromRow == 6 && toRow == 4 && fromCol == toCol && toPiece.type == NO_PIECE)
                {
                    // Black pawn can move two squares from the seventh row
                    fromPiece.type = NO_PIECE;
                    toPiece = fromPiece;
                    break;
                }
                if (fromRow - 1 == toRow && fromCol == toCol && toPiece.type == NO_PIECE)
                {
                    // Black pawn can move one square forward
                    fromPiece.type = NO_PIECE;
                    toPiece = fromPiece;
                    break;
                }
                if (fromRow - 1 == toRow && abs(fromCol - toCol) == 1 && toPiece.type != NO_PIECE)
                {
                    // Black pawn can capture diagonally
                    fromPiece.type = NO_PIECE;
                    toPiece = fromPiece;
                    break;
                }
                if (fromRow == 3 && toRow == 2 && abs(fromCol - toCol) == 1 && toPiece.type == NO_PIECE)
                {
                    // Black pawn can capture en passant
                    board[toRow + 1][toCol].type = NO_PIECE;
                    fromPiece.type = NO_PIECE;
                    toPiece = fromPiece;
                    break;
                }
            }
            if (toRow == 0 || toRow == 7)
            {
                // Pawn has reached the other side of the board, promote it
                fromPiece.type = NO_PIECE;
                toPiece = {promotionType, fromPiece.colour};
                break;
            }
            // Invalid move for pawn
            return;
        case KNIGHT:
            if (abs(fromRow - toRow) == 2 && abs(fromCol - toCol) == 1)
            {
                // Knight can move in an L shape
                fromPiece.type = NO_PIECE;
                toPiece = fromPiece;
                break;
            }
            if (abs(fromRow - toRow) == 1 && abs(fromCol - toCol) == 2)
            {
                // Knight can move in an L shape
                fromPiece.type = NO_PIECE;
                toPiece = fromPiece;
                break;
            }
            // Invalid move for knight
            return;
        case BISHOP:
            if (abs(fromRow - toRow) == abs(fromCol - toCol))
            {
                // Bishop can move diagonally
                fromPiece.type = NO_PIECE;
                toPiece = fromPiece;
                break;
            }
            // Invalid move for bishop
            return;
        case ROOK:
            if (fromRow == toRow || fromCol == toCol)
            {
                // Rook can move horizontally or vertically
                fromPiece.type = NO_PIECE;
                toPiece = fromPiece;
                break;
            }
            // Invalid move for rook
            return;
        case QUEEN:
            if (fromRow == toRow || fromCol == toCol || abs(fromRow - toRow) == abs(fromCol - toCol))
            {
                // Queen can move horizontally, vertically, or diagonally
                fromPiece.type = NO_PIECE;
                toPiece = fromPiece;
                break;
            }
            // Invalid move for queen
            return;
        case KING:
            if (abs(fromRow - toRow) <= 1 && abs(fromCol - toCol) <= 1)
            {
                // King can move one square in any direction
                fromPiece.type = NO_PIECE;
                toPiece = fromPiece;
                break;
            }
            if (fromRow == 0 && fromCol == 4 && toRow == 0 && toCol == 6)
            {
                // White kingside castle
                if (board[0][5].type == NO_PIECE && board[0][6].type == NO_PIECE && board[0][7].type == ROOK)
                {
                    board[0][4].type = NO_PIECE;
                    board[0][6] = board[0][4];
                    board[0][7].type = NO_PIECE;
                    board[0][5] = board[0][7];
                    break;
                }
            }
            if (fromRow == 0 && fromCol == 4 && toRow == 0 && toCol == 2)
            {
                // White queenside castle
                if (board[0][1].type == NO_PIECE && board[0][2].type == NO_PIECE && board[0][3].type == NO_PIECE && board[0][0].type == ROOK)
                {
                    board[0][4].type = NO_PIECE;
                    board[0][2] = board[0][4];
                    board[0][0].type = NO_PIECE;
                    board[0][3] = board[0][0];
                    break;
                }
            }
            if (fromRow == 7 && fromCol == 4 && toRow == 7 && toCol == 6)
            {
                // Black kingside castle
                if (board[7][5].type == NO_PIECE && board[7][6].type == NO_PIECE && board[7][7].type == ROOK)
                {
                    board[7][4].type = NO_PIECE;
                    board[7][6] = board[7][4];
                    board[7][7].type = NO_PIECE;
                    board[7][5] = board[7][7];
                    break;
                }
            }
            if (fromRow == 7 && fromCol == 4 && toRow == 7 && toCol == 2)
            {
                // Black queenside castle
                if (board[7][1].type == NO_PIECE && board[7][2].type == NO_PIECE && board[7][3].type == NO_PIECE && board[7][0].type == ROOK)
                {
                    board[7][4].type = NO_PIECE;
                    board[7][2] = board[7][4];
                    board[7][0].type = NO_PIECE;
                    board[7][3] = board[7][0];
                    break;
                }
            }
            // Invalid move for king
            return;
        }
    }

    std::string boardToFen()
    {
        std::string fen;
        for (int row = 7; row >= 0; row--)
        {
            int emptySquares = 0;
            for (int col = 0; col < 8; col++)
            {
                Piece piece = board[row][col];
                if (piece.type == NO_PIECE)
                {
                    emptySquares++;
                }
                else
                {
                    if (emptySquares > 0)
                    {
                        fen += std::to_string(emptySquares);
                        emptySquares = 0;
                    }
                    fen += pieceToChar(piece);
                }
            }
            if (emptySquares > 0)
            {
                fen += std::to_string(emptySquares);
            }
            if (row > 0)
            {
                fen += '/';
            }
        }
        return fen;
    }



    void sendFen(std::string fen)
    {
        // bluetooth.print(fen);  // send the FEN string over Bluetooth
        // bluetooth.print('\n'); // send a newline character
    }
};