// #include "HallSensors.h"
// #include "ChessBoard.h"
// #include "PieceIdentification.h"

#define delay_const 2000

#define clk 97
#define cs 94

#define Row1LedAnode 35
#define Row2LedAnode 41
#define Row3LedAnode 39
#define Row4LedAnode 33
#define Row5LedAnode 29
#define Row6LedAnode 25
#define Row7LedAnode 27
#define Row8LedAnode 37
#define Row9LedAnode 39

#define aColLedCathode 34
#define bColLedCathode 28
#define cColLedCathode 26
#define dColLedCathode 30
#define eColLedCathode 24
#define fColLedCathode 40
#define gColLedCathode 32
#define hColLedCathode 36
#define iColLedCathode 38

#define arx 96
#define atx 95
#define brx 93
#define btx 92
#define crx 91
#define ctx 90
#define drx 89
#define dtx 88
#define erx 12
#define etx 13
#define frx 10
#define ftx 11
#define grx 8
#define gtx 9
#define hrx 6
#define htx 7

// Enum to represent the different types of chess pieces
enum PieceType
{
    NO_PIECE,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

enum Colour
{
    WHITE,
    BLACK,
    NO_COLOUR
};

enum GameMode
{
    BEGINNER_MODE,
    NORMAL_MODE,
    ENGINE_MODE
};

enum GameState
{
    INIT_GAME,
    PLAY_GAME,
    END_GAME,
    RESET_GAME
};

enum UserAction
{
    WAIT_WHITE,
    WAIT_BLACK,
    PIECE_LIFTED,
    REMOVE_PIECE,
    PROMOTING,
    VALID_MOVE,
    INVALID_MOVE,
    DRAW_ACTION,
    RESIGN_ACTION,
    RESET_ACTION,
    NO_ACTION
};

// Struct for returning piece position from functions
struct Position
{
    int row;
    int col;
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
Position detectNewPiece(int col, int row);
int readHall(int adcnum, int rx, int tx);
void readRow(int row, int rx, int tx);
void adjust();
void printHall();
void setupHallSensors();
void loopHallSensors();
void setupChessBoard();
void loopChessBoard();
void LightAllPieces();
void LightSquare(int row, int col, bool on);
bool movePiece(int fromRow, int fromCol, int toRow, int toCol, PieceType promotionType = NO_PIECE);
void boardToFen();
void sendFen(char *fen);

// Global variable initializations
const int numRows = 8;
const int numCols = 8;

char FEN[100] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

int rawStates[8][8] = {
    {1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}};

int adjStates[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}};

int baseReadings[8][8] = {
    {210, 210, 210, 210, 210, 210, 210, 210},
    {210, 210, 210, 210, 210, 210, 210, 210},
    {210, 210, 210, 210, 210, 210, 210, 210},
    {210, 210, 210, 210, 210, 210, 210, 210},
    {210, 210, 210, 210, 210, 210, 210, 210},
    {210, 210, 210, 210, 210, 210, 210, 210},
    {210, 210, 210, 210, 210, 210, 210, 210},
    {210, 210, 210, 210, 210, 210, 210, 210}};

Piece board[numRows][numCols] = {
    {{ROOK, WHITE}, {KNIGHT, WHITE}, {BISHOP, WHITE}, {QUEEN, WHITE}, {KING, WHITE}, {BISHOP, WHITE}, {KNIGHT, WHITE}, {ROOK, WHITE}},
    {{PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}, {PAWN, WHITE}},
    {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
    {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
    {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
    {Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}, Piece{}},
    {{PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}, {PAWN, BLACK}},
    {{ROOK, BLACK}, {KNIGHT, BLACK}, {BISHOP, BLACK}, {QUEEN, BLACK}, {KING, BLACK}, {BISHOP, BLACK}, {KNIGHT, BLACK}, {ROOK, BLACK}}};

// ChessBoard Functions
void setupChessBoard()
{
    //initializeChessBoardVariables();
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
                board[toRow - 1][toCol].type = NO_PIECE;
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
                board[toRow + 1][toCol].type = NO_PIECE;
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
            if (board[0][5].type == NO_PIECE && board[0][6].type == NO_PIECE && board[0][7].type == ROOK)
            {
                board[0][6] = board[0][4];
                board[0][4] = Piece{};
                board[0][5] = board[0][7];
                board[0][7] = Piece{};
                break;
            }
        }
        if (fromRow == 0 && fromCol == 4 && toRow == 0 && toCol == 2)
        {
            // White queenside castle
            if (board[0][1].type == NO_PIECE && board[0][2].type == NO_PIECE && board[0][3].type == NO_PIECE && board[0][0].type == ROOK)
            {
                board[0][2] = board[0][4];
                board[0][4] = Piece{};
                board[0][3] = board[0][0];
                board[0][0] = Piece{};
                break;
            }
        }
        if (fromRow == 7 && fromCol == 4 && toRow == 7 && toCol == 6)
        {
            // Black kingside castle
            if (board[7][5].type == NO_PIECE && board[7][6].type == NO_PIECE && board[7][7].type == ROOK)
            {
                board[7][6] = board[7][4];
                board[7][4] = Piece{};
                board[7][5] = board[7][7];
                board[7][7] = Piece{};
                break;
            }
        }
        if (fromRow == 7 && fromCol == 4 && toRow == 7 && toCol == 2)
        {
            // Black queenside castle
            if (board[7][1].type == NO_PIECE && board[7][2].type == NO_PIECE && board[7][3].type == NO_PIECE && board[7][0].type == ROOK)
            {
                board[7][2] = board[7][4];
                board[7][4] = Piece{};
                board[7][3] = board[7][0];
                board[7][0] = Piece{};
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
                FEN[fen_index++] = '0' + empty_count;
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
        rowToFen(board[i], fen_index);
        FEN[fen_index++] = '/';
    }
    FEN[--fen_index] = '\0';
}

void sendFen()
{
    // bluetooth.print(fen);  // send the FEN string over Bluetooth
    // bluetooth.print('\n'); // send a newline character
}

// HallSensors Functions
Position detectNewPiece(int col, int row)
{
    // for (int row = 0; row < 8; row++)
    // {
    //     for (int col = 0; col < 8; col++)
    //     {
    if (adjStates[row][col] < baseReadings[row][col])
    {
        if (baseReadings[row][col] - adjStates[row][col] > 100)
        {
            return {row, col};
        }
    }
    else
    {
        if (adjStates[row][col] - baseReadings[row][col] > 100)
        {
            return {row, col};
        }
    }
    //     }
    // }
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

void readRow(int row, int rx, int tx)
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
    // char columns[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};

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
void setupHallSensors()
{
    // initializeHallVariables();
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

void loopHallSensors()
{
    readRow(0, arx, atx);
    readRow(1, brx, btx);
    readRow(2, crx, ctx);
    readRow(3, drx, dtx);
    readRow(4, erx, etx);
    readRow(5, frx, ftx);
    readRow(6, grx, gtx);
    readRow(7, hrx, htx);

    adjust();

    printHall();
    Serial.print("\n");
}

//PieceIdentification Functions
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
        return '.';
    }
}

void setup()
{
    setupHallSensors();
    
    for (int i = 24; i <= 41; i++)
    {
        pinMode(i, OUTPUT);
    }

    for (int i = 24; i <= 41; i+=2)
    {
        digitalWrite(i, LOW);
    }
    

    // open serial port
    Serial.begin(9600);
}

int LED_NUM = 24;

void loop()
{
    loopHallSensors();

    digitalWrite(LED_NUM, HIGH);

    if(LED_NUM == 40){
        digitalWrite(LED_NUM-2, LOW);
        LED_NUM = 24;
    }
    else{
        if (LED_NUM == 24)
        {
            digitalWrite(40, LOW);
            
        }
        digitalWrite(LED_NUM-2, LOW);
        LED_NUM += 2;
    }

    delay(delay_const);
}
