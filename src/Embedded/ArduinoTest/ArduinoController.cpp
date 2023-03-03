#include "MockArduinoController.cpp"

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
    Piece(PieceType pieceType, Colour colourType) : type(pieceType), colour(colourType) {}
};

struct Square
{
    Piece piece;
    int row;
    int col;
    Square() : piece(Piece()), row(0), col(0) {}
    Square(int r, int c) : piece(Piece{}), row(r), col(c) {}
    Square(Piece piece, int r, int c) : piece(piece), row(r), col(c) {}
    Square(PieceType type, Colour colour, int r, int c) : piece(Piece{type, colour}), row(r), col(c) {}
};

// Function delclarations
char pieceToChar(Piece piece);
Piece charToPiece(char pieceChar);
int *detectNewPiece(int col, int row);
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
bool movePiece(int *fromSquare, int *toSquare, PieceType promotionType = NO_PIECE);
void printColors();
void printBoard();
void identifyColors();
void updateBoard();
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

// Start command from LCD
int gameStartPB = 0;

Square *liftedSquare;
Square *placed;

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

// Piece oldPieces[numRows][numCols] = {
//     {{ROOK, BLACK}, {KNIGHT, BLACK}, {BISHOP, BLACK}, {QUEEN, BLACK}, {KING, BLACK}, {BISHOP, BLACK}, {KNIGHT, BLACK}, {ROOK, BLACK}},
//     {{PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}},
//     {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
//     {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
//     {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
//     {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
//     {{PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}},
//     {{ROOK, WHITE}, {KNIGHT, WHITE}, {BISHOP, WHITE}, {QUEEN, WHITE}, {KING, WHITE}, {BISHOP, WHITE}, {KNIGHT, WHITE}, {ROOK, WHITE}}};

// Piece currentPieces[numRows][numCols] = {
//     {{ROOK, BLACK}, {KNIGHT, BLACK}, {BISHOP, BLACK}, {QUEEN, BLACK}, {KING, BLACK}, {BISHOP, BLACK}, {KNIGHT, BLACK}, {ROOK, BLACK}},
//     {{PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}},
//     {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
//     {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
//     {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
//     {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
//     {{PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}},
//     {{ROOK, WHITE}, {KNIGHT, WHITE}, {BISHOP, WHITE}, {QUEEN, WHITE}, {KING, WHITE}, {BISHOP, WHITE}, {KNIGHT, WHITE}, {ROOK, WHITE}}};

Square currentBoard[numRows][numCols];
Square oldBoard[numRows][numCols];

// ChessBoard Functions
void resetChessBoard()
{
    PieceType backRows[8] = {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK};
    int i, j;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            currentBoard[i][j] = Square(i, j);
            oldBoard[i][j] = Square(i, j);
            Colour col;
            PieceType type;
            if (i > 1 && i < 6)
            {
                currentBoard[i][j].piece = Piece();
                oldBoard[i][j].piece = Piece();
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
            currentBoard[i][j].piece = Piece(type, col);
            oldBoard[i][j].piece = Piece(type, col);
        }
    }
}

void loopChessBoard()
{
}

bool movePiece(int *fromSquare, int *toSquare, PieceType promotionType = NO_PIECE)
{
    int fromRow = fromSquare[0];
    int fromCol = fromSquare[1];
    int toRow = toSquare[0];
    int toCol = toSquare[1];

    Piece &fromPiece = currentBoard[fromRow][fromCol].piece;
    Piece &toPiece = currentBoard[toRow][toCol].piece;

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
                currentBoard[toRow - 1][toCol].piece.type = NO_PIECE;
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
                currentBoard[toRow + 1][toCol].piece.type = NO_PIECE;
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
            if (currentBoard[0][5].piece.type == NO_PIECE && currentBoard[0][6].piece.type == NO_PIECE && currentBoard[0][7].piece.type == ROOK)
            {
                currentBoard[0][6].piece = currentBoard[0][4].piece;
                currentBoard[0][4].piece = Piece{};
                currentBoard[0][5].piece = currentBoard[0][7].piece;
                currentBoard[0][7].piece = Piece{};
                break;
            }
        }
        if (fromRow == 0 && fromCol == 4 && toRow == 0 && toCol == 2)
        {
            // White queenside castle
            if (currentBoard[0][1].piece.type == NO_PIECE && currentBoard[0][2].piece.type == NO_PIECE && currentBoard[0][3].piece.type == NO_PIECE && currentBoard[0][0].piece.type == ROOK)
            {
                currentBoard[0][2].piece = currentBoard[0][4].piece;
                currentBoard[0][4].piece = Piece{};
                currentBoard[0][3].piece = currentBoard[0][0].piece;
                currentBoard[0][0].piece = Piece{};
                break;
            }
        }
        if (fromRow == 7 && fromCol == 4 && toRow == 7 && toCol == 6)
        {
            // Black kingside castle
            if (currentBoard[7][5].piece.type == NO_PIECE && currentBoard[7][6].piece.type == NO_PIECE && currentBoard[7][7].piece.type == ROOK)
            {
                currentBoard[7][6].piece = currentBoard[7][4].piece;
                currentBoard[7][4].piece = Piece{};
                currentBoard[7][5].piece = currentBoard[7][7].piece;
                currentBoard[7][7].piece = Piece{};
                break;
            }
        }
        if (fromRow == 7 && fromCol == 4 && toRow == 7 && toCol == 2)
        {
            // Black queenside castle
            if (currentBoard[7][1].piece.type == NO_PIECE && currentBoard[7][2].piece.type == NO_PIECE && currentBoard[7][3].piece.type == NO_PIECE && currentBoard[7][0].piece.type == ROOK)
            {
                currentBoard[7][2].piece = currentBoard[7][4].piece;
                currentBoard[7][4].piece = Piece{};
                currentBoard[7][3].piece = currentBoard[7][0].piece;
                currentBoard[7][0].piece = Piece{};
                break;
            }
        }
        // Invalid move for king
        return;
    }
}

// Converts a single row of the chess board to a FEN string
void rowToFen(Square row[8], int &fen_index)
{
    int empty_count = 0;
    for (int i = 0; i < 8; ++i)
    {
        Piece piece = row[i].piece;
        if (piece.type == PieceType::NO_PIECE)
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
            FEN[fen_index++] = pieceToChar(piece);
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
    if (piece.colour == NO_COLOUR)
    {
        return '0';
    }

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
            Serial.print(currentBoard[i][j].piece.colour);
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
            Serial.print(pieceToChar(currentBoard[i][j].piece));
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

bool gameStartValid()
{
    bool valid = true;
    int i;
    for (i = 0; i < 8; i++)
    {
        if (currentBoard[0][i].piece.colour != WHITE)
            valid = false;
        if (currentBoard[1][i].piece.colour != WHITE)
            valid = false;

        if (currentBoard[6][i].piece.colour != BLACK)
            valid = false;
        if (currentBoard[7][i].piece.colour != BLACK)
            valid = false;
    }
    return valid;
}

void identifyColors()
{
    int i, j;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (adjStates[i][j] < 140)
                currentBoard[i][j].piece.colour = WHITE;
            else if (adjStates[i][j] > 310)
                currentBoard[i][j].piece.colour = BLACK;
            else
                currentBoard[i][j].piece.colour = NO_COLOUR;
        }
    }
}

void updateBoard()
{
    int i, j;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            oldBoard[i][j] = currentBoard[i][j];
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
            if (currentBoard[i][j].piece.colour != oldBoard[i][j].piece.colour)
            {
                liftedSquare = &currentBoard[i][j];
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
            if (currentBoard[i][j].piece.colour != oldBoard[i][j].piece.colour)
            {
                currentBoard[i][j] = *liftedSquare;
                (*liftedSquare).piece = Piece();
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
    if (currentBoard[row][col].piece.type == NO_PIECE)
    {
        lightUp(row, col);
        return true;
    }
    else if (currentBoard[row][col].piece.colour != activeColour)
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

void highlightPawnMoves(Square square)
{
    int row = square.row;
    int col = square.col;
    Colour activeColour = square.piece.colour;

    int direction = activeColour == WHITE ? 1 : -1;

    if (row + direction >= 0 && row + direction <= 7)
    {
        // Pawn can move forward by one square
        if (currentBoard[row + direction][col].piece.type == NO_PIECE)
        {
            lightUp(row + direction, col);
        }

        // Pawn can capture enemy piece diagonally
        if (col > 0 && currentBoard[row + direction][col - 1].piece.colour != activeColour)
        {
            lightUp(row + direction, col - 1);
        }
        if (col < 7 && currentBoard[row + direction][col + 1].piece.colour != activeColour)
        {
            lightUp(row + direction, col + 1);
        }
    }
}

void highlightKnightMoves(Square square)
{
    int row = square.row;
    int col = square.col;
    Colour activeColour = square.piece.colour;

    int moves[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
    for (int i = 0; i < 8; i++)
    {
        int newRow = row + moves[i][0];
        int newCol = col + moves[i][1];
        if (newRow >= 0 && newRow <= 7 && newCol >= 0 && newCol <= 7)
        {
            Piece targetPiece = currentBoard[newRow][newCol].piece;
            if (targetPiece.colour != activeColour)
            {
                lightUp(newRow, newCol);
            }
        }
    }
}

void highlightBishopMoves(Square square)
{
    int row = square.row;
    int col = square.col;
    Colour activeColour = square.piece.colour;

    // maximum possible distances from piece to edge of board
    // ul = up left, dr = down right, etc.
    int dist_ul = min(7 - row, col);
    int dist_ur = min(7 - row, 7 - col);
    int dist_dl = min(row, col);
    int dist_dr = min(row, 7 - col);

    // Move up left
    for (int i = 1; i <= dist_ul; i++)
    {
        if (!lightValidSquare(row + i, col - i, activeColour))
        {
            break;
        }
    }

    // Move up right
    for (int i = 1; i <= dist_ur; i++)
    {
        if (!lightValidSquare(row + i, col + i, activeColour))
        {
            break;
        }
    }

    // Move down left
    for (int i = 1; i <= dist_dl; i++)
    {
        if (!lightValidSquare(row - i, col - i, activeColour))
        {
            break;
        }
    }

    // Move down right
    for (int i = 1; i <= dist_dr; i++)
    {
        if (!lightValidSquare(row - i, col + i, activeColour))
        {
            break;
        }
    }
}

void highlightRookMoves(Square square)
{
    int row = square.row;
    int col = square.col;
    Colour activeColour = square.piece.colour;

    // Check moves to the right
    for (int i = col + 1; i = 7; i++)
    {
        if (!lightValidSquare(row, i, activeColour))
        {
            break;
        }
    }

    // Check moves to the left
    for (int i = col - 1; i >= 0; i--)
    {
        if (!lightValidSquare(row, i, activeColour))
        {
            break;
        }
    }

    // Check moves down
    for (int i = row - 1; i >= 0; i--)
    {
        if (!lightValidSquare(i, col, activeColour))
        {
            break;
        }
    }

    // Check moves up
    for (int i = row + 1; i <= 7; i++)
    {
        if (!lightValidSquare(i, col, activeColour))
        {
            break;
        }
    }
}

void highlightQueenMoves(Square square)
{
    int row = square.row;
    int col = square.col;
    Colour activeColour = square.piece.colour;

    // maximum possible distances from piece to edge of board
    // ul = up left, dr = down right, etc.
    int dist_ul = min(7 - row, col);
    int dist_ur = min(7 - row, 7 - col);
    int dist_dl = min(row, col);
    int dist_dr = min(row, 7 - col);

    // Move up left
    for (int i = 1; i <= dist_ul; i++)
    {
        if (!lightValidSquare(row + i, col - i, activeColour))
        {
            break;
        }
    }

    // Move up right
    for (int i = 1; i <= dist_ur; i++)
    {
        if (!lightValidSquare(row + i, col + i, activeColour))
        {
            break;
        }
    }

    // Move down left
    for (int i = 1; i <= dist_dl; i++)
    {
        if (!lightValidSquare(row - i, col - i, activeColour))
        {
            break;
        }
    }

    // Move down right
    for (int i = 1; i <= dist_dr; i++)
    {
        if (!lightValidSquare(row - i, col + i, activeColour))
        {
            break;
        }
    }

    // Move down
    for (int i = row - 1; i >= 0; i--)
    {
        if (!lightValidSquare(i, col, activeColour))
        {
            break;
        }
    }

    // Move up
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
}

void highlightKingMoves(Square square)
{
    int row = liftedSquare->row;
    int col = liftedSquare->col;
    Colour activeColour = liftedSquare->piece.colour;

    // Check the squares directly adjacent to the king
    if (row > 0)
    {
        if (currentBoard[row - 1][col].piece.colour != activeColour)
        {
            lightUp(row - 1, col);
        }
        if (col > 0 && currentBoard[row - 1][col - 1].piece.colour != activeColour)
        {
            lightUp(row - 1, col - 1);
        }
        if (col < 7 && currentBoard[row - 1][col + 1].piece.colour != activeColour)
        {
            lightUp(row - 1, col + 1);
        }
    }
    if (row < 7)
    {
        if (currentBoard[row + 1][col].piece.colour != activeColour)
        {
            lightUp(row + 1, col);
        }
        if (col > 0 && currentBoard[row + 1][col - 1].piece.colour != activeColour)
        {
            lightUp(row + 1, col - 1);
        }
        if (col < 7 && currentBoard[row + 1][col + 1].piece.colour != activeColour)
        {
            lightUp(row + 1, col + 1);
        }
    }
    if (col > 0 && currentBoard[row][col - 1].piece.colour != activeColour)
    {
        lightUp(row, col - 1);
    }
    if (col < 7 && currentBoard[row][col + 1].piece.colour != activeColour)
    {
        lightUp(row, col + 1);
    }
}

void flash()
{
    Square square = *liftedSquare;
    switch (square.piece.type)
    {
    case PAWN:
        highlightPawnMoves(square);
        return;
    case KNIGHT:
        highlightKnightMoves(square);
        return;
    case ROOK:
        highlightRookMoves(square);
        return;
    case BISHOP:
        highlightBishopMoves(square);
        return;
    case QUEEN:
        highlightQueenMoves(square);
        return;
    case KING:
        highlightKingMoves(square);
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
    // Serial.println((char)gameCommand);
    Serial.print("\n");

    switch (gameState)
    {
    case INIT_GAME:
        resetChessBoard();
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
        break;
    case PIECE_LIFTED:
        // sendFen();
        flash();
        if (checkPlace())
        {
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
        lightsOff();
        gameState = WAIT_PICK;
        break;
    case INVALID_MOVE:
        lightUp((*liftedSquare).row, (*liftedSquare).col);
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
    Serial.println(FEN);
    printBoard();
    delay(delay_const);
    Serial.print("\n");

    updateBoard();
    delay(delay_const);
}
