#include "ChessBoard.h"
#include "PieceIdentification.h"



void initializeChessBoardVariables(){
    Piece board[numRows][numCols] = {
        {{ROOK, WHITE}, {KNIGHT, WHITE}, {BISHOP, WHITE}, {QUEEN, WHITE}, {KING, WHITE}, {BISHOP, WHITE}, {KNIGHT, WHITE}, {ROOK, WHITE}},
        {{PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}},
        {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
        {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
        {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
        {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
        {{PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}},
        {{ROOK, BLACK}, {KNIGHT, BLACK}, {BISHOP, BLACK}, {QUEEN, BLACK}, {KING, BLACK}, {BISHOP, BLACK}, {KNIGHT, BLACK}, {ROOK, BLACK}}};
}

void resetChessBoard()
{
  initializeChessBoardVariables();
}

void loopChessBoard()
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
    
}

void LightAllPieces()
{
    // Update the LEDs based on the board
    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < numCols; j++)
        {
            Piece piece = currentBoard[i][j];
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
void LightSquare(int row, int col, bool on)
{
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
    Piece &fromPiece = currentBoard[fromRow][fromCol];
    Piece &toPiece = currentBoard[toRow][toCol];

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
    else if (toPiece.type != NO_PIECE && fromPiece.colour != toPiece.colour)
    {
    }
    switch (fromPiece.type)
    {
    case PAWN:
        if (fromPiece.colour == 0)
        { // white pawn
            if (fromRow == 1 && toRow == 3 && fromCol == toCol && toPiece.type == NO_PIECE)
            {
                // White pawn can move two squares from the second row
                toPiece = fromPiece;
                fromPiece = Piece{};
                break;
            }
            if (fromRow + 1 == toRow && fromCol == toCol && toPiece.type == NO_PIECE)
            {
                // White pawn can move one square forward
                toPiece = fromPiece;
                fromPiece = Piece{};
                break;
            }
            if (fromRow + 1 == toRow && abs(fromCol - toCol) == 1 && toPiece.type != NO_PIECE)
            {
                // White pawn can capture diagonally
                toPiece = fromPiece;
                fromPiece = Piece{};
                break;
            }
            if (fromRow == 4 && toRow == 5 && abs(fromCol - toCol) == 1 && toPiece.type == NO_PIECE)
            {
                // White pawn can capture en passant
                currentBoard[toRow - 1][toCol].type = NO_PIECE;
                toPiece = fromPiece;
                fromPiece = Piece{};
                break;
            }
        }
        else
        { // black pawn
            if (fromRow == 6 && toRow == 4 && fromCol == toCol && toPiece.type == NO_PIECE)
            {
                // Black pawn can move two squares from the seventh row
                toPiece = fromPiece;
                fromPiece = Piece{};
                break;
            }
            if (fromRow - 1 == toRow && fromCol == toCol && toPiece.type == NO_PIECE)
            {
                // Black pawn can move one square forward
                toPiece = fromPiece;
                fromPiece = Piece{};
                break;
            }
            if (fromRow - 1 == toRow && abs(fromCol - toCol) == 1 && toPiece.type != NO_PIECE)
            {
                // Black pawn can capture diagonally
                toPiece = fromPiece;
                fromPiece = Piece{};
                break;
            }
            if (fromRow == 3 && toRow == 2 && abs(fromCol - toCol) == 1 && toPiece.type == NO_PIECE)
            {
                // Black pawn can capture en passant
                currentBoard[toRow + 1][toCol].type = NO_PIECE;
                toPiece = fromPiece;
                fromPiece = Piece{};
                break;
            }
        }
        if (toRow == 0 || toRow == 7)
        {
            // Pawn has reached the other side of the board, promote it
            toPiece = (Piece){promotionType, fromPiece.colour};
            fromPiece = Piece{};
            break;
        }
        // Invalid move for pawn
        return;
    case KNIGHT:
        if (abs(fromRow - toRow) == 2 && abs(fromCol - toCol) == 1)
        {
            // Knight can move in an L shape
            toPiece = fromPiece;
            fromPiece = Piece{};
            break;
        }
        if (abs(fromRow - toRow) == 1 && abs(fromCol - toCol) == 2)
        {
            // Knight can move in an L shape
            toPiece = fromPiece;
            fromPiece = Piece{};
            break;
        }
        // Invalid move for knight
        return;
    case BISHOP:
        if (abs(fromRow - toRow) == abs(fromCol - toCol))
        {
            // Bishop can move diagonally
            toPiece = fromPiece;
            fromPiece = Piece{};
            break;
        }
        // Invalid move for bishop
        return;
    case ROOK:
        if (fromRow == toRow || fromCol == toCol)
        {
            // Rook can move horizontally or vertically
            toPiece = fromPiece;
            fromPiece = Piece{};
            break;
        }
        // Invalid move for rook
        return;
    case QUEEN:
        if (fromRow == toRow || fromCol == toCol || abs(fromRow - toRow) == abs(fromCol - toCol))
        {
            // Queen can move horizontally, vertically, or diagonally
            toPiece = fromPiece;
            fromPiece = Piece{};
            break;
        }
        // Invalid move for queen
        return;
    case KING:
        if (abs(fromRow - toRow) <= 1 && abs(fromCol - toCol) <= 1)
        {
            // King can move one square in any direction
            toPiece = fromPiece;
            fromPiece = Piece{};
            break;
        }
        if (fromRow == 0 && fromCol == 4 && toRow == 0 && toCol == 6)
        {
            // White kingside castle
            if (currentBoard[0][5].type == NO_PIECE && currentBoard[0][6].type == NO_PIECE && currentBoard[0][7].type == ROOK)
            {
                currentBoard[0][6] = currentBoard[0][4];
                currentBoard[0][4] = Piece{};
                currentBoard[0][5] = currentBoard[0][7];
                currentBoard[0][7] = Piece{};
                break;
            }
        }
        if (fromRow == 0 && fromCol == 4 && toRow == 0 && toCol == 2)
        {
            // White queenside castle
            if (currentBoard[0][1].type == NO_PIECE && currentBoard[0][2].type == NO_PIECE && currentBoard[0][3].type == NO_PIECE && currentBoard[0][0].type == ROOK)
            {
                currentBoard[0][2] = currentBoard[0][4];
                currentBoard[0][4] = Piece{};
                currentBoard[0][3] = currentBoard[0][0];
                currentBoard[0][0] = Piece{};
                break;
            }
        }
        if (fromRow == 7 && fromCol == 4 && toRow == 7 && toCol == 6)
        {
            // Black kingside castle
            if (currentBoard[7][5].type == NO_PIECE && currentBoard[7][6].type == NO_PIECE && currentBoard[7][7].type == ROOK)
            {
                currentBoard[7][6] = currentBoard[7][4];
                currentBoard[7][4] = Piece{};
                currentBoard[7][5] = currentBoard[7][7];
                currentBoard[7][7] = Piece{};
                break;
            }
        }
        if (fromRow == 7 && fromCol == 4 && toRow == 7 && toCol == 2)
        {
            // Black queenside castle
            if (currentBoard[7][1].type == NO_PIECE && currentBoard[7][2].type == NO_PIECE && currentBoard[7][3].type == NO_PIECE && currentBoard[7][0].type == ROOK)
            {
                currentBoard[7][2] = currentBoard[7][4];
                currentBoard[7][4] = Piece{};
                currentBoard[7][3] = currentBoard[7][0];
                currentBoard[7][0] = Piece{};
                break;
            }
        }
        // Invalid move for king
        return;
    }
}

// Converts a single row of the chess board to a FEN string
void rowToFen(Piece row[8], char fen[], int &fen_index)
{
    int empty_count = 0;
    for (int i = 0; i < 8; ++i)
    {
        if (row[i].type == PieceType::NO_PIECE)
        {
            ++empty_count;
        }
        else
        {
            if (empty_count > 0)
            {
                fen[fen_index++] = '0' + empty_count;
                empty_count = 0;
            }
            fen[fen_index++] = pieceToChar(row[i]);
        }
    }
    if (empty_count > 0)
    {
        fen[fen_index++] = '0' + empty_count;
    }
}

// Converts a chess board to a FEN string
void boardToFen(char fen[])
{
    int fen_index = 0;
    for (int i = 0; i < 8; ++i)
    {
        rowToFen(currentBoard[i], fen, fen_index);
        fen[fen_index++] = '/';
    }
    fen[--fen_index] = '\0';
}

// char* boardToFen()
// {
//     std::string fen;
//     for (int row = 7; row >= 0; row--)
//     {
//         int emptySquares = 0;
//         for (int col = 0; col < 8; col++)
//         {
//             Piece piece = board[row][col];
//             if (piece.type == NO_PIECE)
//             {
//                 emptySquares++;
//             }
//             else
//             {
//                 if (emptySquares > 0)
//                 {
//                     fen += std::to_string(emptySquares);
//                     emptySquares = 0;
//                 }
//                 fen += pieceToChar(piece);
//             }
//         }
//         if (emptySquares > 0)
//         {
//             fen += std::to_string(emptySquares);
//         }
//         if (row > 0)
//         {
//             fen += '/';
//         }
//     }
//     return fen;
// }

void sendFen(char* fen)
{
    // bluetooth.print(fen);  // send the FEN string over Bluetooth
    // bluetooth.print('\n'); // send a newline character
}