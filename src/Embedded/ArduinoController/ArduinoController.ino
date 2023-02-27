// #include "HallSensors.h"
// #include "ChessBoard.h"
// #include "PieceIdentification.h"

int delay_const = 2000;

int clk = A0; // 97;   // A0
int cs = A3;  // 94;    // A3

int anodes[9] = {29, 35, 25, 41, 27, 31, 37, 39, 33};
int cathodes[9] = {34, 28, 26, 30, 24, 40, 32, 36, 38};

int arx = A1; // 96;   // A1
int atx = A2; // 95;   // A2
int brx = A4; // 93;   // A4
int btx = A5; // 92;   // A5
int crx = A6; // 91;   // A6
int ctx = A7; // 90;   // A7
int drx = A8; // 89;   // A8
int dtx = A9; // 88;   // A9
int erx = 12;
int etx = 13;
int frx = 10;
int ftx = 11;
int grx = 8;
int gtx = 9;
int hrx = 6;
int htx = 7;

enum GameMode : char
{
    BEGINNER_MODE = 'B',
    NORMAL_MODE = 'N',
    ENGINE_MODE = 'E',
    NO_MODE = 'M'
};

enum GameState : char
{
    INIT_GAME = 'i',
    PLAY_GAME = 'p',
    RESET_GAME = 'r',
    WAIT_PICK = 'w',
    PIECE_LIFTED = 'l',
    REMOVE_PIECE = 'x',
    PROMOTING = 'P',
    VALID_MOVE = 'v',
    INVALID_MOVE = 'n',
};

enum GameCommand : char
{
    PROMOTE_QUEEN = 'Q',
    PROMOTE_ROOK = 'R',
    PROMOTE_BISHOP = 'B',
    PROMOTE_KNIGHT = 'K',
    DRAW_ACTION = 'd',
    RESIGN_ACTION = 'q',
    RESET_ACTION = 'r',
    END_GAME = 'e',
    NO_ACTION = '.'
};

// Enum to represent the different types of chess pieces
enum PieceType : char
{
    NO_PIECE = '0',
    PAWN = 'P',
    KNIGHT = 'N',
    BISHOP = 'B',
    ROOK = 'R',
    QUEEN = 'Q',
    KING = 'K'
};

// BLACK is 32 because P + 32 = p
// Use this to convert to FEN string
enum Colour
{
    WHITE = 0,
    BLACK = 32,
    NO_COLOUR = 64
};

// Structure to represent a chess piece
// Members: PieceType, Colour
// Default NO_PIECE, NO_COLOUR
struct Piece
{
    PieceType type;
    Colour colour;
    Piece() : type(PieceType::NO_PIECE), colour(Colour::NO_COLOUR) {}
    Piece(PieceType piece, Colour colourType) : type(piece), colour(colourType) {}
};

// Function delclarations
char pieceToChar(Piece piece);
Piece charToPiece(char pieceChar);
int* detectNewPiece(int col, int row);
int readHall(int adcnum, int rx, int tx);
void readHallRow(int row, int rx, int tx);
void adjust();
void printHall();
void setupHallSensors();
void setupLEDs();
void readHallSensors();
void resetChessBoard();
void loopChessBoard();
void LightAllPieces();
void LightSquare(int row, int col, bool on);
bool movePiece(int* fromSquare, int * toSquare, PieceType promotionType = NO_PIECE);
void printColors();
void printBoard();
void identifyColors();
void updateColors();
bool gameStartValid();
void assignPieces();
void rowToFen(Piece row[8], int &fen_index);
void boardToFen();
void sendFen();
bool checkPick();
bool checkPlace();
void lightUp(int i, int j);
bool lightValidSquare(int row, int col, Colour activeColour);
void flash();
void highlightPawnMoves(int row, int col, Colour activeColour);
void highlightKnightMoves(int row, int col, Colour activeColour);
void highlightBishopMoves(int row, int col, Colour activeColour);
void highlightRookMoves(int row, int col, Colour activeColour);
void highlightQueenMoves(int row, int col, Colour activeColour);
void highlightKingMoves(int row, int col, Colour activeColour);

// Global variable initializations
const int numRows = 8;
const int numCols = 8;

int turns = 1;
GameMode gameMode = BEGINNER_MODE;
GameState gameState = INIT_GAME;
GameCommand gameCommand = NO_ACTION;
Colour whoseTurn = WHITE;
bool promoting = false;

char CastlingStatus[5] = "KQkq";

char FEN[100] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 1";

int rawStates[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0}};

int adjStates[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0}};

// int newBoardColors[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0},
//                             {0, 0, 0, 0, 0, 0, 0, 0},
//                             {0, 0, 0, 0, 0, 0, 0, 0},
//                             {0, 0, 0, 0, 0, 0, 0, 0},
//                             {0, 0, 0, 0, 0, 0, 0, 0},
//                             {0, 0, 0, 0, 0, 0, 0, 0},
//                             {0, 0, 0, 0, 0, 0, 0, 0},
//                             {0, 0, 0, 0, 0, 0, 0, 0}};

// int oldBoardColors[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0},
//                             {0, 0, 0, 0, 0, 0, 0, 0},
//                             {0, 0, 0, 0, 0, 0, 0, 0},
//                             {0, 0, 0, 0, 0, 0, 0, 0},
//                             {0, 0, 0, 0, 0, 0, 0, 0},
//                             {0, 0, 0, 0, 0, 0, 0, 0},
//                             {0, 0, 0, 0, 0, 0, 0, 0},
//                             {0, 0, 0, 0, 0, 0, 0, 0}};

// char currentBoard[8][8] = {{'0', '0', '0', '0', '0', '0', '0', '0'},
//                              {'0', '0', '0', '0', '0', '0', '0', '0'},
//                              {'0', '0', '0', '0', '0', '0', '0', '0'},
//                              {'0', '0', '0', '0', '0', '0', '0', '0'},
//                              {'0', '0', '0', '0', '0', '0', '0', '0'},
//                              {'0', '0', '0', '0', '0', '0', '0', '0'},
//                              {'0', '0', '0', '0', '0', '0', '0', '0'},
//                              {'0', '0', '0', '0', '0', '0', '0', '0'}};

// char defBoardPieces[8][8] = {{'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
//                              {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
//                              {'0', '0', '0', '0', '0', '0', '0', '0'},
//                              {'0', '0', '0', '0', '0', '0', '0', '0'},
//                              {'0', '0', '0', '0', '0', '0', '0', '0'},
//                              {'0', '0', '0', '0', '0', '0', '0', '0'},
//                              {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
//                              {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}};

int baseReadings[8][8] = {
    {210, 210, 210, 210, 210, 210, 210, 210},
    {210, 210, 210, 210, 210, 210, 210, 210},
    {210, 210, 210, 210, 210, 210, 210, 210},
    {210, 210, 210, 210, 210, 210, 210, 210},
    {210, 210, 210, 210, 210, 210, 210, 210},
    {210, 210, 210, 210, 210, 210, 210, 210},
    {210, 210, 210, 210, 210, 210, 210, 210},
    {210, 210, 210, 210, 210, 210, 210, 210}};

Piece oldBoard[numRows][numCols] = {
    {{ROOK, WHITE}, {KNIGHT, WHITE}, {BISHOP, WHITE}, {QUEEN, WHITE}, {KING, WHITE}, {BISHOP, WHITE}, {KNIGHT, WHITE}, {ROOK, WHITE}},
    {{PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}},
    {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
    {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
    {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
    {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
    {{PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}},
    {{ROOK, BLACK}, {KNIGHT, BLACK}, {BISHOP, BLACK}, {QUEEN, BLACK}, {KING, BLACK}, {BISHOP, BLACK}, {KNIGHT, BLACK}, {ROOK, BLACK}}};

Piece currentBoard[numRows][numCols] = {
    {{ROOK, WHITE}, {KNIGHT, WHITE}, {BISHOP, WHITE}, {QUEEN, WHITE}, {KING, WHITE}, {BISHOP, WHITE}, {KNIGHT, WHITE}, {ROOK, WHITE}},
    {{PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}},
    {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
    {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
    {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
    {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
    {{PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}},
    {{ROOK, BLACK}, {KNIGHT, BLACK}, {BISHOP, BLACK}, {QUEEN, BLACK}, {KING, BLACK}, {BISHOP, BLACK}, {KNIGHT, BLACK}, {ROOK, BLACK}}};

// identify the positions which hold white or black pieces
int whiteSquares[16][16];
int blackSquares[16][16];

// ChessBoard Functions
void resetChessBoard()
{
    PieceType backRows[8] = {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK};
    int i, j;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            Colour col;
            PieceType type;
            if (i > 1 && i < 6)
            {
                currentBoard[i][j] = Piece{};
                oldBoard[i][j] = Piece{};
                continue;
            }
            else if (i < 2)
            {
                col = WHITE;
                if (i == 0)
                    type = backRows[j];
                else
                    type = PAWN;
            }
            else
            {
                col = BLACK;
                if (i == 7)
                    type = backRows[j];
                else
                    type = PAWN;
            }
            currentBoard[i][j] = {type, col};
            oldBoard[i][j] = {type, col};
        }
    }

    for (i = 0; i < 16; i++)
    {
                // row, col
        whiteSquares[i][i] = i / 8;
        whiteSquares[i][i] = i % 8;
        blackSquares[i][i] = i / 8 + 6;
        blackSquares[i][i] = i % 8;
    }
    
}

void loopChessBoard()
{
}

bool movePiece(int* fromSquare, int* toSquare, PieceType promotionType = NO_PIECE)
{
    int fromRow = fromSquare[0];
    int fromCol = fromSquare[1];
    int toRow = toSquare[0];
    int toCol = toSquare[1];

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
void rowToFen(Piece row[8], int &fen_index)
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
                FEN[fen_index++] = (char)('0' + empty_count);
                empty_count = 0;
            }
            FEN[fen_index++] = pieceToChar(row[i]);
        }
    }
    if (empty_count > 0)
    {
        FEN[fen_index++] = '0' + empty_count;
    }
}

// Converts a chess board to a FEN string
void boardToFen()
{
    int fen_index = 0;
    for (int i = 0; i < 8; ++i)
    {
        rowToFen(currentBoard[i], fen_index);
        if (i != 7)
            FEN[fen_index++] = '/';
        else
            FEN[fen_index++] = ' ';
    }

    // w or b
    FEN[fen_index++] = whoseTurn == WHITE ? 'w' : 'b';
    FEN[fen_index++] = ' ';

    int castle_index = 0;
    while (CastlingStatus[castle_index])
    {
        FEN[fen_index++] = CastlingStatus[castle_index++];
    }

    // en passant status. leave as - for now
    FEN[fen_index++] = '-';
    FEN[fen_index++] = ' ';

    // convert turn number to chars
    int temp = turns;
    char buf[6];
    int buf_index = 0;
    buf[buf_index] = '\0';
    while (temp > 0)
    {
        buf[++buf_index] = '0' + (temp % 10);
        temp /= 10;
    }
    while (buf[buf_index])
    {
        FEN[fen_index++] = buf[buf_index--];
    }
    
    FEN[fen_index++] = ' ';

    FEN[fen_index++] = '\r';
    FEN[fen_index] = '\0';
}

void sendFen()
{
    int i = 0;
    boardToFen();

    // while (FEN[i])
    // {
    //     Serial.write(FEN[i++]);
    // }

    for (i = 0; i < 64; i++)
    {
        Serial.write(FEN[i]);
    }
    for (i = 64; i < 127; i++)
    {
        Serial1.write(FEN[i]);
    }

    Serial1.write("\n");
}

int readHall(int adcnum, int rx, int tx)
{
    digitalWrite(cs, HIGH);
    digitalWrite(clk, LOW);
    digitalWrite(cs, LOW);

    int commandout = adcnum;
    commandout |= 0x18;
    commandout <<= 3;

    for (int i = 0; i < 5; i++)
    {
        if (commandout & 0x80)
        {
            digitalWrite(tx, HIGH);
        }
        else
        {
            digitalWrite(tx, LOW);
        }

        commandout <<= 1;
        digitalWrite(clk, HIGH);
        digitalWrite(clk, LOW);
    }

    int adcout = 0;

    for (int i = 0; i < 12; i++)
    {
        digitalWrite(clk, HIGH);
        digitalWrite(clk, LOW);

        adcout <<= 1;

        if (digitalRead(rx))
            adcout |= 0x1;
    }

    digitalWrite(cs, HIGH);

    adcout >>= 1;
    return adcout;
}

void readHallRow(int row, int rx, int tx)
{
    int i;

    for (i = 0; i < 8; i++)
    {
        rawStates[row][i] = readHall(i, rx, tx);
    }
}

void adjust()
{
    adjStates[7][0] = rawStates[2][3];
    adjStates[7][1] = rawStates[2][2];
    adjStates[7][2] = rawStates[2][1];
    adjStates[7][3] = rawStates[2][0];
    adjStates[7][4] = rawStates[1][7];
    adjStates[7][5] = rawStates[1][6];
    adjStates[7][6] = rawStates[1][5];
    adjStates[7][7] = rawStates[1][4];
    adjStates[6][0] = rawStates[2][7];
    adjStates[6][1] = rawStates[2][6];
    adjStates[6][2] = rawStates[2][5];
    adjStates[6][3] = rawStates[2][4];
    adjStates[6][4] = rawStates[1][3];
    adjStates[6][5] = rawStates[1][2];
    adjStates[6][6] = rawStates[1][1];
    adjStates[6][7] = rawStates[1][0];
    adjStates[5][0] = rawStates[3][3];
    adjStates[5][1] = rawStates[3][2];
    adjStates[5][2] = rawStates[3][1];
    adjStates[5][3] = rawStates[3][0];
    adjStates[5][4] = rawStates[0][7];
    adjStates[5][5] = rawStates[0][6];
    adjStates[5][6] = rawStates[0][5];
    adjStates[5][7] = rawStates[0][4];
    adjStates[4][0] = rawStates[3][7];
    adjStates[4][1] = rawStates[3][6];
    adjStates[4][2] = rawStates[3][5];
    adjStates[4][3] = rawStates[3][4];
    adjStates[4][4] = rawStates[0][3];
    adjStates[4][5] = rawStates[0][2];
    adjStates[4][6] = rawStates[0][1];
    adjStates[4][7] = rawStates[0][0];
    adjStates[3][0] = rawStates[7][0];
    adjStates[3][1] = rawStates[7][1];
    adjStates[3][2] = rawStates[7][2];
    adjStates[3][3] = rawStates[7][3];
    adjStates[3][4] = rawStates[5][0];
    adjStates[3][5] = rawStates[5][1];
    adjStates[3][6] = rawStates[5][2];
    adjStates[3][7] = rawStates[5][3];
    adjStates[2][0] = rawStates[7][4];
    adjStates[2][1] = rawStates[7][5];
    adjStates[2][2] = rawStates[7][6];
    adjStates[2][3] = rawStates[7][7];
    adjStates[2][4] = rawStates[5][7];
    adjStates[2][5] = rawStates[5][6];
    adjStates[2][6] = rawStates[5][5];
    adjStates[2][7] = rawStates[5][4];
    adjStates[1][0] = rawStates[6][0];
    adjStates[1][1] = rawStates[6][1];
    adjStates[1][2] = rawStates[6][2];
    adjStates[1][3] = rawStates[6][3];
    adjStates[1][4] = rawStates[4][3];
    adjStates[1][5] = rawStates[4][2];
    adjStates[1][6] = rawStates[4][1];
    adjStates[1][7] = rawStates[4][0];
    adjStates[0][0] = rawStates[6][4];
    adjStates[0][1] = rawStates[6][5];
    adjStates[0][2] = rawStates[6][6];
    adjStates[0][3] = rawStates[6][7];
    adjStates[0][4] = rawStates[4][7];
    adjStates[0][5] = rawStates[4][6];
    adjStates[0][6] = rawStates[4][5];
    adjStates[0][7] = rawStates[4][4];
}

void printHall()
{
    int i, j;

    Serial.println("\ta\tb\tc\td\te\tf\tg\th");
    for (i = 0; i < 8; i++)
    {
        Serial.print(8 - i);
        Serial.print("\t");
        for (j = 0; j < 8; j++)
        {
            Serial.print(adjStates[i][j]);
            Serial.print("\t");
        }
        Serial.print("\n");
        delay(1);
    }
}

void setupLEDs()
{
    for (int i = 0; i < 9; i++)
    {
        pinMode(anodes[i], OUTPUT);
        pinMode(cathodes[i], OUTPUT);
        digitalWrite(anodes[i], HIGH);
        digitalWrite(cathodes[i], LOW);
    }
}

void setupHallSensors()
{
    pinMode(cs, OUTPUT);
    pinMode(arx, INPUT);
    pinMode(atx, OUTPUT);
    pinMode(brx, INPUT);
    pinMode(btx, OUTPUT);
    pinMode(crx, INPUT);
    pinMode(ctx, OUTPUT);
    pinMode(drx, INPUT);
    pinMode(dtx, OUTPUT);
    pinMode(erx, INPUT);
    pinMode(etx, OUTPUT);
    pinMode(frx, INPUT);
    pinMode(ftx, OUTPUT);
    pinMode(grx, INPUT);
    pinMode(gtx, OUTPUT);
    pinMode(hrx, INPUT);
    pinMode(htx, OUTPUT);
    pinMode(clk, OUTPUT);
}

void readHallSensors()
{
    readHallRow(0, arx, atx);
    readHallRow(1, brx, btx);
    readHallRow(2, crx, ctx);
    readHallRow(3, drx, dtx);
    readHallRow(4, erx, etx);
    readHallRow(5, frx, ftx);
    readHallRow(6, grx, gtx);
    readHallRow(7, hrx, htx);

    adjust();

    printHall();
    Serial.print("\n");
}

// PieceIdentification Functions
char pieceToChar(Piece piece)
{
    return (char)(piece.type + piece.colour);
}

Piece charToPiece(char pieceChar)
{
    if ((int)pieceChar > 90)
    {
        // if char is lowercase
        return Piece{(PieceType)pieceChar, BLACK};
    }
    else
    {
        // if char is uppercase
        return Piece{(PieceType)pieceChar, WHITE};
    }
}

void printColors()
{
    int i, j;

    Serial.println("\ta\tb\tc\td\te\tf\tg\th");
    for (i = 0; i < 8; i++)
    {
        Serial.print(8 - i);
        Serial.print("\t");
        for (j = 0; j < 8; j++)
        {
            Serial.print(currentBoard[i][j].colour);
            Serial.print("\t");
        }
        Serial.print("\n");
    }
}

void printBoard()
{
    int i, j;

    Serial.println("\ta\tb\tc\td\te\tf\tg\th");
    for (i = 0; i < 8; i++)
    {
        Serial.print(8 - i);
        Serial.print("\t");
        for (j = 0; j < 8; j++)
        {
            Serial.print(pieceToChar(currentBoard[i][j]));
            Serial.print("\t");
        }
        Serial.print("\n");
    }
}

// 0 is when the game is over and before the game is started.
// Enters state when powered on and when game over button is pressed
// Leaves state when all whites and blacks are in correct spot and the game start is pressed

// 1 Game is active and players are thinking (all pieces are on the board)
// Enters state when game start is pressed
// Leaves state when a piece is picked up or if resign or draw is pressed

// 2 Piece is picked up, LEDs are lit up accordingly
// Keeps track of what piece was picked up and what are the possible moves
// Enters state when piece is picked up
// Leaves state when the piece is placed again

// Start command from LCD
int gameStartPB = 0;

// 0 for white, 1 for black
int turns = 0;

Piece liftedPiece = Piece{};

// row, col
int liftedSquare[2] = {0, 0};
int placedSquare[2] = {0, 0};

bool gameStartValid()
{
    bool valid = true;
    int i;
    for (i = 0; i < 8; i++)
    {
        if (currentBoard[0][i].colour != WHITE)
            valid = false;
        if (currentBoard[1][i].colour != WHITE)
            valid = false;

        if (currentBoard[6][i].colour != BLACK)
            valid = false;
        if (currentBoard[7][i].colour != BLACK)
            valid = false;
    }
    return valid;
}

int* detectNewPiece(int col, int row)
{
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            if (adjStates[row][col] < 140)
            {
            }
            else if (adjStates[row][col] > 310)
            {
            }
            else
            {
            }
        }
    }
}

void identifyColors()
{
    int i, j;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (adjStates[i][j] < 140)
                currentBoard[i][j].colour = WHITE;
            else if (adjStates[i][j] > 310)
                currentBoard[i][j].colour = BLACK;
            else
                currentBoard[i][j].colour = NO_COLOUR;
        }
    }
}

void updateColors()
{
    int i, j;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            oldBoard[i][j].colour = currentBoard[i][j].colour;
        }
    }
}

bool checkPick()
{
    bool change = false;
    int i, j;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (currentBoard[i][j].colour != oldBoard[i][j].colour)
            {
                liftedPiece = currentBoard[i][j];
                liftedSquare[0] = i;
                liftedSquare[1] = j;
                change = true;
            }
        }
    }
    return change;
}

bool checkPlace()
{
    bool change = false;
    int i, j;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (currentBoard[i][j].colour != oldBoard[i][j].colour)
            {
                currentBoard[i][j] = liftedPiece;
                currentBoard[liftedSquare[0]][liftedSquare[1]] = Piece{};
                change = true;
            }
        }
    }
    return change;
}

void lightUp(int row, int col)
{
    digitalWrite(anodes[row], LOW);
    digitalWrite(anodes[row + 1], LOW);
    digitalWrite(cathodes[col], HIGH);
    digitalWrite(cathodes[col + 1], HIGH);
}

// return true to continue checking for suqares to light
// return false to stop checking (for pieces that move in straight lines)
bool lightValidSquare(int row, int col, Colour activeColour)
{
    if (currentBoard[row][col].type == NO_PIECE)
    {
        lightUp(row, col);
        return true;
    }
    else if (currentBoard[row][col].colour != activeColour)
    {
        lightUp(row, col);
        return false;
    }
    else
    {
        return false;
    }
}

void lightsOff()
{
    int m;
    for (m = 0; m < 9; m++)
    {
        digitalWrite(anodes[m], HIGH);
        digitalWrite(cathodes[m], LOW);
    }
}

void highlightPawnMoves(int row, int col, Colour activeColour)
{
    int direction = currentBoard[row][col].colour == WHITE ? 1 : -1;

    if (row + direction >= 0 && row + direction <= 7)
    {
        // Pawn can move forward by one square
        if (currentBoard[row + direction][col].type == NO_PIECE)
        {
            lightUp(row + direction, col);
        }

        // Pawn can capture enemy piece diagonally
        if (col > 0 && currentBoard[row + direction][col - 1].colour != activeColour)
        {
            lightUp(row + direction, col - 1);
        }
        if (col < 7 && currentBoard[row + direction][col + 1].colour != activeColour)
        {
            lightUp(row + direction, col + 1);
        }
    }
}

void highlightKnightMoves(int row, int col, Colour activeColour)
{
    int moves[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
    for (int i = 0; i < 8; i++)
    {
        int newRow = row + moves[i][0];
        int newCol = col + moves[i][1];
        if (newRow >= 0 && newRow <= 7 && newCol >= 0 && newCol <= 7)
        {
            Piece targetPiece = currentBoard[newRow][newCol];
            if (targetPiece.type == NO_PIECE || targetPiece.colour != activeColour)
            {
                lightUp(newRow, newCol);
            }
        }
    }
}

void highlightBishopMoves(int row, int col, Colour activeColour)
{
    // Move up and right
    for (int i = row + 1; i <= 7; i++)
    {
        for (int j = 0; j <= 7; j++)
        {
            if (!lightValidSquare(i, j, activeColour))
            {
                break;
            }
        }
    }

    // Move up and left
    for (int i = row + 1; i <= 7; i++)
    {
        for (int j = col - 1; j >= 0; j--)
        {
            if (!lightValidSquare(i, j, activeColour))
            {
                break;
            }
        }
    }

    // Check for moves in the down-right direction
    for (int i = row - 1; i >= 0; i--)
    {
        for (int j = 0; j <= 7; j++)
        {
            if (!lightValidSquare(i, j, activeColour))
            {
                break;
            }
        }
    }

    // Check for moves in the down-left direction
    for (int i = row - 1; i >= 0; i--)
    {
        for (int j = col - 1; j >= 0; j--)
        {
            if (!lightValidSquare(i, j, activeColour))
            {
                break;
            }
        }
    }
}

void highlightRookMoves(int row, int col, Colour activeColour)
{
    if (col + 1 < 8)
    {
        // Check moves to the right
        for (int i = col + 1; i < 8; i++)
        {
            if (!lightValidSquare(row, i, activeColour))
            {
                break;
            }
        }
    }

    if (col - 1 >= 0)
    {
        // Check moves to the left
        for (int i = col - 1; i >= 0; i--)
        {
            if (!lightValidSquare(row, i, activeColour))
            {
                break;
            }
        }
    }

    if (row - 1 >= 0)
    {
        // Check moves down
        for (int i = row - 1; i >= 0; i--)
        {
            if (!lightValidSquare(i, col, activeColour))
            {
                break;
            }
        }
    }

    if (row + 1 < 8)
    {
        // Check moves up
        for (int i = row + 1; i < 8; i++)
        {
            if (!lightValidSquare(i, col, activeColour))
            {
                break;
            }
        }
    }
}

void highlightQueenMoves(int row, int col, Colour activeColour)
{
    // Move up
    for (int i = row - 1; i >= 0; i--)
    {
        if (!lightValidSquare(i, col, activeColour))
        {
            break;
        }
    }

    // Move down
    for (int i = row + 1; i <= 7; i++)
    {
        if (!lightValidSquare(i, col, activeColour))
        {
            break;
        }
    }

    // Move left
    for (int i = col - 1; i >= 0; i--)
    {
        if (!lightValidSquare(row, i, activeColour))
        {
            break;
        }
    }

    // Move right
    for (int i = col + 1; i <= 7; i++)
    {
        if (!lightValidSquare(row, i, activeColour))
        {
            break;
        }
    }

    // Move down and right
    for (int i = row - 1; i >= 0; i--)
    {
        for (int j = 0; j <= 7; j++)
        {
            if (!lightValidSquare(i, j, activeColour))
            {
                break;
            }
        }
    }

    // Move down and left
    for (int i = row - 1; i >= 0; i--)
    {
        for (int j = col - 1; j >= 0; j--)
        {
            if (!lightValidSquare(i, j, activeColour))
            {
                break;
            }
        }
    }

    // Move up and right
    for (int i = row + 1; i <= 7; i++)
    {
        for (int j = 0; j <= 7; j++)
        {
            if (!lightValidSquare(i, j, activeColour))
            {
                break;
            }
        }
    }

    // Move up and left
    for (int i = row + 1; i <= 7; i++)
    {
        for (int j = col - 1; j >= 0; j--)
        {
            if (!lightValidSquare(i, j, activeColour))
            {
                break;
            }
        }
    }
}

void highlightKingMoves(int row, int col, Colour activeColour)
{
    // Check the squares directly adjacent to the king
    if (row > 0)
    {
        if (currentBoard[row - 1][col].type == NO_PIECE || currentBoard[row - 1][col].colour != activeColour)
        {
            lightUp(row - 1, col);
        }
        if (col > 0 && currentBoard[row - 1][col - 1].type == NO_PIECE || currentBoard[row - 1][col - 1].colour != activeColour)
        {
            lightUp(row - 1, col - 1);
        }
        if (col < 7 && currentBoard[row - 1][col + 1].type == NO_PIECE || currentBoard[row - 1][col + 1].colour != activeColour)
        {
            lightUp(row - 1, col + 1);
        }
    }
    if (row < 7)
    {
        if (currentBoard[row + 1][col].type == NO_PIECE || currentBoard[row + 1][col].colour != activeColour)
        {
            lightUp(row + 1, col);
        }
        if (col > 0 && currentBoard[row + 1][col - 1].type == NO_PIECE || currentBoard[row + 1][col - 1].colour != activeColour)
        {
            lightUp(row + 1, col - 1);
        }
        if (col < 7 && currentBoard[row + 1][col + 1].type == NO_PIECE || currentBoard[row + 1][col + 1].colour != activeColour)
        {
            lightUp(row + 1, col + 1);
        }
    }
    if (col > 0 && currentBoard[row][col - 1].type == NO_PIECE || currentBoard[row][col - 1].colour != activeColour)
    {
        lightUp(row, col - 1);
    }
    if (col < 7 && currentBoard[row][col + 1].type == NO_PIECE || currentBoard[row][col + 1].colour != activeColour)
    {
        lightUp(row, col + 1);
    }
}

void flash()
{
    Piece fromPiece = liftedPiece;
    int fromRow = liftedSquare[0];
    int fromCol = liftedSquare[1];

    switch (fromPiece.type)
    {
    case PAWN:
        highlightPawnMoves(fromRow, fromCol, fromPiece.colour);
        return;
    case KNIGHT:
        highlightKnightMoves(fromRow, fromCol, fromPiece.colour);
        return;
    case ROOK:
        highlightRookMoves(fromRow, fromCol, fromPiece.colour);
        return;
    case BISHOP:
        highlightBishopMoves(fromRow, fromCol, fromPiece.colour);
        return;
    case QUEEN:
        highlightQueenMoves(fromRow, fromCol, fromPiece.colour);
        return;
    case KING:
        highlightKingMoves(fromRow, fromCol, fromPiece.colour);
        return;
    }
}

void setup()
{
    setupHallSensors();
    setupLEDs();

    Serial.begin(9600);
    Serial1.begin(9600);
}

int LED_NUM = 24;

void loop()
{
    if (Serial1.available() > 0)
    {
        gameCommand = Serial1.read();
    }

    if (gameCommand == 'e')
    {
        gameState = INIT_GAME;
    }
    

    readHallSensors();

    identifyColors();

    Serial.print((char)gameState);
    Serial.print("\t");
    Serial.print(turns);
    Serial.print("\t");
    Serial.println(gameCommand);
    Serial.print("\n");

    switch (gameState)
    {
    case INIT_GAME:
        // wait for game mode selection from LCD screen
        // if (Serial1.available() > 0)
        // {
        //     int temp = Serial1.read();
        //     if (temp == BEGINNER_MODE || temp == NORMAL_MODE || temp == ENGINE_MODE)
        //     {
        //         gameMode = (GameMode)temp;
        //         gameState = PLAY_GAME;
        //     }
        // }
        gameState = PLAY_GAME;
        break;
    case PLAY_GAME:
        if (gameStartValid())
        {
            resetChessBoard();
            gameState = WAIT_PICK;
        }
        else
        {
            // figure out some way to indicate that the game isn't set up properly.
            // light squares??
        }
        break;
    case RESET_GAME:
        // do we need this state?
        break;
    case WAIT_PICK:
        // sendFen();
        if (checkPick())
        {
            gameState = PIECE_LIFTED;
        }
        else if (gameCommand == 'e')
            gameState = INIT_GAME;
        break;
    case PIECE_LIFTED:
        // sendFen();
        flash();
        if (checkPlace())
        {
            lightsOff();
            gameState = VALID_MOVE;
            // if (movePiece(liftedSquare, placedSquare, QUEEN))
            // {
            //     gameState = VALID_MOVE;
            // }
            // else
            // {
            //     gameState = INVALID_MOVE;
            // }
        }
        else if (gameCommand == 'e')
            gameState = INIT_GAME;
        break;
    case REMOVE_PIECE:
        // should we keep track of pieces taken?
        gameState = WAIT_PICK;
        break;
    case PROMOTING:
        // wait for piece selection from LCD screen
        if (promoting)
        {
            if (checkPick())
            {
                gameState = PIECE_LIFTED;
            }
        }
        
        gameState = WAIT_PICK;
        break;
    case VALID_MOVE:
        if (whoseTurn == WHITE)
            whoseTurn = BLACK;
        else
        {
            whoseTurn = WHITE;
            turns += 1;
        }
        gameState = WAIT_PICK;
        break;
    case INVALID_MOVE:
        lightUp(liftedSquare[0], liftedSquare[1]);
        gameState = WAIT_PICK;
        break;
    default:
        break;
    }

    // switch (gameState)
    // {
    // case 0:
    //     if (gameStartValid())
    //     { //&& gameStartPB) {
    //         resetChessBoard();
    //         gameState = WAIT_PICK;
    //     }
    //     break;
    // case 1:
    //     // sendFen();
    //     if (checkPick())
    //     {
    //         gameState = PIECE_LIFTED;
    //     }
    //     else if (gameCommand == 'e')
    //         gameState = WAIT_PICK;
    //     break;
    // case 2:
    //     // sendFen();
    //     flash();
    //     if (checkPlace())
    //     {
    //         if (whoseTurn == 'w')
    //             whoseTurn = 'b';
    //         else
    //         {
    //             whoseTurn = 'w';
    //             turns += 1;
    //         }
    //         gameState = WAIT_PICK;
    //         lightsOff();
    //     }
    //     else if (gameCommand == 'e')
    //         gameState = INIT_GAME;
    //     break;
    // default:
    //     break;
    // }

    // printHall();
    // delay(delay_const);
    // Serial.print("\n");
    // Serial.print("\n");
    // printColors();
    // delay(delay_const);
    // Serial.print("\n");
    // Serial.print("\n");
    printBoard();
    delay(delay_const);
    Serial.print("\n");

    updateColors();
    delay(delay_const);
}
