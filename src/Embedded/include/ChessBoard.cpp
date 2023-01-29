#include <string>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <PieceIdentification.h>

const int numRows = 8;
const int numCols = 8;


// Pins to control the LEDs
const int LED_PINS[numRows+1][numCols+1] = {
    {2, 3, 4, 5, 6, 7, 8, 9, 10},
    {10, 11, 12, 13, 14, 15, 16, 17, 18},
    {18, 19, 20, 21, 22, 23, 24, 25, 26},
    {26, 27, 28, 29, 30, 31, 32, 33, 34},
    {34, 35, 36, 37, 38, 39, 40, 41, 42},
    {42, 43, 44, 45, 46, 47, 48, 49, 50},
    {50, 51, 52, 53, 54, 55, 56, 57, 58},
    {58, 59, 60, 61, 62, 63, 64, 65, 66}};

// Pins to read the hall-effect sensors
const int HALL_PINS[numRows][numCols] = {
    {66, 67, 68, 69, 70, 71, 72, 73},
    {74, 75, 76, 77, 78, 79, 80, 81},
    {82, 83, 84, 85, 86, 87, 88, 89},
    {90, 91, 92, 93, 94, 95, 96, 97},
    {98, 99, 100, 101, 102, 103, 104, 105},
    {106, 107, 108, 109, 110, 111, 112, 113},
    {114, 115, 116, 117, 118, 119, 120, 121},
    {122, 123, 124, 125, 126, 127, 128, 129}};

SoftwareSerial bluetooth(RX_PIN, TX_PIN); // create a SoftwareSerial object

// 2D array to represent the chess board
Piece board[numRows][numCols];

std::string FEN;

void setup()
{
    // Initialize the board to all empty pieces
    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < numCols; j++)
        {
            board[i][j] = {NONE, -1};
        }
    }
    // Set the LED pins to output mode
    for (int i = 0; i < numRows+1; i++)
    {
        for (int j = 0; j < numCols+1; j++)
        {
            pinMode(LED_PINS[i][j], OUTPUT);
        }
    }
    // Set the sensor pins to input mode
    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < numCols; j++)
        {
            pinMode(HALL_PINS[i][j], INPUT);
        }
    }
    bluetooth.begin(9600); // set the baud rate
    while (!bluetooth)
        ; // wait for Bluetooth connection
}

void loop()
{
    // Update the board based on the hall-effect sensors
    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < numCols; j++)
        {
            if (digitalRead(HALL_PINS[i][j]) == HIGH)
            {
                // Sensor is activated, toggle the piece
                Piece &piece = board[i][j];
                if (piece.type == NONE)
                {
                    piece.type = PAWN;
                    piece.color = 0; // white
                }
                else
                {
                    piece.type = NONE;
                    piece.color = -1;
                }
                FEN = boardToFen();
            }
        }
    }
    // Update the LEDs based on the board
    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < numCols; j++)
        {
            Piece piece = board[i][j];
            if (piece.type == NONE)
            {
                digitalWrite(LED_PINS[i][j], LOW);
            }
            else
            {
                digitalWrite(LED_PINS[i][j], HIGH);
                digitalWrite(LED_PINS[i][j+1], HIGH);
                digitalWrite(LED_PINS[i+1][j], HIGH);
                digitalWrite(LED_PINS[i+1][j+1], HIGH);
            }
        }
    }
}

bool movePiece(int fromRow, int fromCol, int toRow, int toCol, PieceType promotionType = NONE)
{
    Piece &fromPiece = board[fromRow][fromCol];
    Piece &toPiece = board[toRow][toCol];
    if (fromPiece.type == NONE)
    {
        // There is no piece to move
        return;
    }
    if (toPiece.type != NONE && fromPiece.color == toPiece.color)
    {
        // Cannot capture own piece
        // illegalMove(int fromRow, int fromCol, int toRow, int toCol);
        return;
    }
    switch (fromPiece.type)
    {
    case PAWN:
        if (fromPiece.color == 0)
        { // white pawn
            if (fromRow == 1 && toRow == 3 && fromCol == toCol && toPiece.type == NONE)
            {
                // White pawn can move two squares from the second row
                fromPiece.type = NONE;
                toPiece = fromPiece;
                break;
            }
            if (fromRow + 1 == toRow && fromCol == toCol && toPiece.type == NONE)
            {
                // White pawn can move one square forward
                fromPiece.type = NONE;
                toPiece = fromPiece;
                break;
            }
            if (fromRow + 1 == toRow && abs(fromCol - toCol) == 1 && toPiece.type != NONE)
            {
                // White pawn can capture diagonally
                fromPiece.type = NONE;
                toPiece = fromPiece;
                break;
            }
            if (fromRow == 4 && toRow == 5 && abs(fromCol - toCol) == 1 && toPiece.type == NONE)
            {
                // White pawn can capture en passant
                board[toRow - 1][toCol].type = NONE;
                fromPiece.type = NONE;
                toPiece = fromPiece;
                break;
            }
        }
        else
        { // black pawn
            if (fromRow == 6 && toRow == 4 && fromCol == toCol && toPiece.type == NONE)
            {
                // Black pawn can move two squares from the seventh row
                fromPiece.type = NONE;
                toPiece = fromPiece;
                break;
            }
            if (fromRow - 1 == toRow && fromCol == toCol && toPiece.type == NONE)
            {
                // Black pawn can move one square forward
                fromPiece.type = NONE;
                toPiece = fromPiece;
                break;
            }
            if (fromRow - 1 == toRow && abs(fromCol - toCol) == 1 && toPiece.type != NONE)
            {
                // Black pawn can capture diagonally
                fromPiece.type = NONE;
                toPiece = fromPiece;
                break;
            }
            if (fromRow == 3 && toRow == 2 && abs(fromCol - toCol) == 1 && toPiece.type == NONE)
            {
                // Black pawn can capture en passant
                board[toRow + 1][toCol].type = NONE;
                fromPiece.type = NONE;
                toPiece = fromPiece;
                break;
            }
        }
        if (toRow == 0 || toRow == 7)
        {
            // Pawn has reached the other side of the board, promote it
            fromPiece.type = NONE;
            toPiece = {promotionType, fromPiece.color};
            break;
        }
        // Invalid move for pawn
        return;
    case KNIGHT:
        if (abs(fromRow - toRow) == 2 && abs(fromCol - toCol) == 1)
        {
            // Knight can move in an L shape
            fromPiece.type = NONE;
            toPiece = fromPiece;
            break;
        }
        if (abs(fromRow - toRow) == 1 && abs(fromCol - toCol) == 2)
        {
            // Knight can move in an L shape
            fromPiece.type = NONE;
            toPiece = fromPiece;
            break;
        }
        // Invalid move for knight
        return;
    case BISHOP:
        if (abs(fromRow - toRow) == abs(fromCol - toCol))
        {
            // Bishop can move diagonally
            fromPiece.type = NONE;
            toPiece = fromPiece;
            break;
        }
        // Invalid move for bishop
        return;
    case ROOK:
        if (fromRow == toRow || fromCol == toCol)
        {
            // Rook can move horizontally or vertically
            fromPiece.type = NONE;
            toPiece = fromPiece;
            break;
        }
        // Invalid move for rook
        return;
    case QUEEN:
        if (fromRow == toRow || fromCol == toCol || abs(fromRow - toRow) == abs(fromCol - toCol))
        {
            // Queen can move horizontally, vertically, or diagonally
            fromPiece.type = NONE;
            toPiece = fromPiece;
            break;
        }
        // Invalid move for queen
        return;
    case KING:
        if (abs(fromRow - toRow) <= 1 && abs(fromCol - toCol) <= 1)
        {
            // King can move one square in any direction
            fromPiece.type = NONE;
            toPiece = fromPiece;
            break;
        }
        if (fromRow == 0 && fromCol == 4 && toRow == 0 && toCol == 6)
        {
            // White kingside castle
            if (board[0][5].type == NONE && board[0][6].type == NONE && board[0][7].type == ROOK)
            {
                board[0][4].type = NONE;
                board[0][6] = board[0][4];
                board[0][7].type = NONE;
                board[0][5] = board[0][7];
                break;
            }
        }
        if (fromRow == 0 && fromCol == 4 && toRow == 0 && toCol == 2)
        {
            // White queenside castle
            if (board[0][1].type == NONE && board[0][2].type == NONE && board[0][3].type == NONE && board[0][0].type == ROOK)
            {
                board[0][4].type = NONE;
                board[0][2] = board[0][4];
                board[0][0].type = NONE;
                board[0][3] = board[0][0];
                break;
            }
        }
        if (fromRow == 7 && fromCol == 4 && toRow == 7 && toCol == 6)
        {
            // Black kingside castle
            if (board[7][5].type == NONE && board[7][6].type == NONE && board[7][7].type == ROOK)
            {
                board[7][4].type = NONE;
                board[7][6] = board[7][4];
                board[7][7].type = NONE;
                board[7][5] = board[7][7];
                break;
            }
        }
        if (fromRow == 7 && fromCol == 4 && toRow == 7 && toCol == 2)
        {
            // Black queenside castle
            if (board[7][1].type == NONE && board[7][2].type == NONE && board[7][3].type == NONE && board[7][0].type == ROOK)
            {
                board[7][4].type = NONE;
                board[7][2] = board[7][4];
                board[7][0].type = NONE;
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
            if (piece.type == NONE)
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
    bluetooth.print(fen);  // send the FEN string over Bluetooth
    bluetooth.print('\n'); // send a newline character
}
