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

// WHITE = 0 , BLACK = 1, NO_COLOUR = 2
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
// Members: row, col
// Default: 0, 0
struct Position
{
  int row;
  int col;
  Position() : row(0), col(0) {}
  Position(int r, int c) : row(r), col(c) {}
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
void printColors();
void printBoard();
void identifyColors();
void updateColors();
int gameStartValid();
void assignPieces();
void sendFen();
int checkPick();
int checkPlace();
void lightUp(int i, int j);
void highlightPawnMoves(int row, int col, Colour activeColour);
void highlightKnightMoves(int row, int col, Colour activeColour);
void highlightBishopMoves(int row, int col, Colour activeColour);
void highlightRookMoves(int row, int col, Colour activeColour);
void highlightQueenMoves(int row, int col, Colour activeColour);
void highlightKingMoves(int row, int col, Colour activeColour);
void flash();

// Global variable initializations
const int numRows = 8;
const int numCols = 8;

char CastlingStatus[5] = "KQkq";

char FEN[100] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

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

int newBoardColors[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0}};

int oldBoardColors[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0}};

char curBoardPieces[8][8] = {{'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'}};

char defBoardPieces[8][8] = {{'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
                             {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
                             {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}};

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
  // initializeChessBoardVariables();
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

// PieceIdentification Functions
char pieceToChar(Piece piece)
{
  switch (piece.type)
  {
  case PAWN:
    return piece.colour == Colour::WHITE ? 'P' : 'p';
  case KNIGHT:
    return piece.colour == Colour::WHITE ? 'N' : 'n';
  case BISHOP:
    return piece.colour == Colour::WHITE ? 'B' : 'b';
  case ROOK:
    return piece.colour == Colour::WHITE ? 'R' : 'r';
  case QUEEN:
    return piece.colour == Colour::WHITE ? 'Q' : 'q';
  case KING:
    return piece.colour == Colour::WHITE ? 'K' : 'k';
  default:
    return '.';
  }
}

Piece charToPiece(char pieceChar){
  switch(pieceChar)
  {
    case 'P':
      return Piece {PAWN, WHITE};
    case 'p':
      return Piece {PAWN, BLACK};
    case 'N':
      return Piece {KNIGHT, WHITE};
    case 'n':
      return Piece {KNIGHT, BLACK};
    case 'B':
      return Piece {BISHOP, WHITE};
    case 'b':
      return Piece {BISHOP, BLACK};
    case 'R':
      return Piece {ROOK, WHITE};
    case 'r':
      return Piece {ROOK, BLACK};
    case 'Q':
      return Piece {QUEEN, WHITE};
    case 'q':
      return Piece {QUEEN, BLACK};
    case 'K':
      return Piece {KING, WHITE};
    case 'k':
      return Piece {KING, BLACK};
    default:
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
      Serial.print(newBoardColors[i][j]);
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
      Serial.print(curBoardPieces[i][j]);
      Serial.print("\t");
    }
    Serial.print("\n");
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
        newBoardColors[i][j] = 0;
      else if (adjStates[i][j] > 310)
        newBoardColors[i][j] = 1;
      else
        newBoardColors[i][j] = 2;
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
      oldBoardColors[i][j] = newBoardColors[i][j];
    }
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
int gameState = 0;

// Start command from LCD
int gameStartPB = 0;

// 0 for white, 1 for black
int turns = 0;

char liftedPiece = ' ';

// row, col
int liftedSquare[2] = {0, 0};

int gameStartValid()
{
  int valid = 1;
  int i;
  for (i = 0; i < 8; i++)
  {
    if (newBoardColors[0][i] != 0)
      valid = 0;
    if (newBoardColors[1][i] != 0)
      valid = 0;

    if (newBoardColors[6][i] != 1)
      valid = 0;
    if (newBoardColors[7][i] != 1)
      valid = 0;
  }
  return valid;
}

void assignPieces()
{
  int i, j;
  for (i = 0; i < 8; i++)
  {
    for (j = 0; j < 8; j++)
    {
      curBoardPieces[i][j] = defBoardPieces[i][j];
    }
  }
}

void sendFen()
{
  int k, l;
  String fen = "";
  for (k = 0; k < 8; k++)
  {
    int itemp = 0;
    char ctemp[1];
    for (l = 0; l < 8; l++)
    {
      if (curBoardPieces[k][l] == '0')
      {
        itemp += 1;
      }
      else
      {
        if (itemp != 0)
        {
          itoa(itemp, ctemp, 10);
          fen += ctemp;
        }
        fen += curBoardPieces[k][l];
        itemp = 0;
      }
    }
    if (itemp != 0)
    {
      itoa(itemp, ctemp, 10);
      fen += ctemp;
      fen += '/';
    }
    else if (k != 7)
      fen += '/';
    else
      fen += ' ';
  }

  if (turns % 2 == 0)
    fen += "w ";
  else
    fen += "b ";

  // adjust later for castling
  fen += "KQkq - 0 ";

  char t[1];
  itoa(turns, t, 10);
  fen += t;

  Serial.write(fen[k]);
  Serial.write("\n");
}

int checkPick()
{
  int change = 0;
  int i, j;
  for (i = 0; i < 8; i++)
  {
    for (j = 0; j < 8; j++)
    {
      if (newBoardColors[i][j] != oldBoardColors[i][j])
      {
        liftedPiece = curBoardPieces[i][j];
        liftedSquare[0] = i;
        liftedSquare[1] = j;
        change = 1;
      }
    }
  }
  return change;
}

int checkPlace()
{
  int change = 0;
  int i, j;
  for (i = 0; i < 8; i++)
  {
    for (j = 0; j < 8; j++)
    {
      if (newBoardColors[i][j] != oldBoardColors[i][j])
      {
        curBoardPieces[i][j] = liftedPiece;
        curBoardPieces[liftedSquare[0]][liftedSquare[1]] = '0';
        change = 1;
      }
    }
  }
  return change;
}

void highlightPawnMoves(int row, int col, Colour activeColour)
{
  int direction = charToPiece(curBoardPieces[row][col]).colour == WHITE ? -1 : 1;

  if (row + direction >= 0 && row + direction <= 7)
  {
    // Pawn can move forward by one square
    if (curBoardPieces[row + direction][col] == '0')
    {
      lightUp(row + direction, col);
    }

    // Pawn can capture enemy piece diagonally
    if (col > 0 && charToPiece(curBoardPieces[row + direction][col - 1]).colour != activeColour)
    {
      lightUp(row + direction, col - 1);
    }
    if (col < 7 && charToPiece(curBoardPieces[row + direction][col + 1]).colour != activeColour)
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
      char targetPiece = curBoardPieces[newRow][newCol];
      if (targetPiece == '0' || charToPiece(targetPiece).colour != activeColour)
      {
        lightUp(newRow, newCol);
      }
    }
  }
}

void highlightBishopMoves(int row, int col, Colour activeColour)
{
  // Check for moves in the up-right direction
  for (int i = row + 1, j = col + 1; i < 8 && j < 8; i++, j++)
  {
    if (curBoardPieces[i][j] == '0' || charToPiece(curBoardPieces[i][j]).colour != activeColour)
    {
      lightUp(i, j);
    }
    else
    {
      break;
    }
  }

  // Check for moves in the up-left direction
  for (int i = row + 1, j = col - 1; i < 8 && j >= 0; i++, j--)
  {
    if (curBoardPieces[i][j] == '0' || charToPiece(curBoardPieces[i][j]).colour != activeColour)
    {
      lightUp(i, j);
    }
    else
    {
      break;
    }
  }

  // Check for moves in the down-right direction
  for (int i = row - 1, j = col + 1; i >= 0 && j < 8; i--, j++)
  {
    if (curBoardPieces[i][j] == '0' || charToPiece(curBoardPieces[i][j]).colour != activeColour)
    {
      lightUp(i, j);
    }
    else
    {
      break;
    }
  }

  // Check for moves in the down-left direction
  for (int i = row - 1, j = col - 1; i >= 0 && j >= 0; i--, j--)
  {
    if (curBoardPieces[i][j] == '0' || charToPiece(curBoardPieces[i][j]).colour != activeColour)
    {
      lightUp(i, j);
    }
    else
    {
      break;
    }
  }
}

void highlightRookMoves(int row, int col, Colour activeColour)
{
  int i, j;

  if(col + 1 < 8){
    // Check moves to the right
    for (i = col + 1; i < 8; i++)
    {
      if (curBoardPieces[row][i] == '0' || charToPiece(curBoardPieces[row][i]).colour != activeColour)
      {
        lightUp(row, i);
      }
      else
      {
        if (charToPiece(curBoardPieces[row][i]).colour == activeColour)
        {
          break;
        }
      }
    }
  }

  if(col - 1 >= 0){
    // Check moves to the left
    for (i = col - 1; i >= 0; i--)
    {
      if (curBoardPieces[row][i] == '0' || charToPiece(curBoardPieces[row][i]).colour != activeColour)
      {
        lightUp(row, i);
      }
      else
      {
        if (charToPiece(curBoardPieces[row][i]).colour == activeColour)
        {
          break;
        }
      }
    }
  }

  if(row - 1 >= 0){
    // Check moves down
    for (j = row - 1; j >= 0; j--)
    {
      if (curBoardPieces[j][col] == '0' || charToPiece(curBoardPieces[j][col]).colour != activeColour)
      {
        lightUp(j, col);
      }
      else
      {
        if (charToPiece(curBoardPieces[j][col]).colour == activeColour)
        {
          break;
        }
      }
    }
  }

  if (row + 1 < 8)
  {
    // Check moves up
    for (j = row + 1; j < 8; j++)
    {
      if (curBoardPieces[j][col] == '0' || charToPiece(curBoardPieces[j][col]).colour != activeColour)
      {
        lightUp(j, col);
      }
      else
      {
        if (charToPiece(curBoardPieces[j][col]).colour == activeColour)
        {
          break;
        }
      }
    }
  }
}

void highlightQueenMoves(int row, int col, Colour activeColour)
{
  // Move up
  for (int i = row - 1; i >= 0; i--)
  {
    if (curBoardPieces[i][col] == '0' || charToPiece(curBoardPieces[i][col]).colour != activeColour)
    {
      lightUp(i, col);
    }
    else if (charToPiece(curBoardPieces[i][col]).colour == activeColour)
    {
      break;
    }
  }

  // Move down
  for (int i = row + 1; i <= 7; i++)
  {
    if (curBoardPieces[i][col] == '0' || charToPiece(curBoardPieces[i][col]).colour != activeColour)
    {
      lightUp(i, col);
    }
    else if (charToPiece(curBoardPieces[i][col]).colour == activeColour)
    {
      break;
    }
  }

  // Move left
  for (int j = col - 1; j >= 0; j--)
  {
    if (curBoardPieces[row][j] == '0' || charToPiece(curBoardPieces[row][j]).colour != activeColour)
    {
      lightUp(row, j);
    }
    else if (charToPiece(curBoardPieces[row][j]).colour == activeColour)
    {
      break;
    }
  }

  // Move right
  for (int j = col + 1; j <= 7; j++)
  {
    if (curBoardPieces[row][j] == '0' || charToPiece(curBoardPieces[row][j]).colour != activeColour)
    {
      lightUp(row, j);
    }
    else if (charToPiece(curBoardPieces[row][j]).colour == activeColour)
    {
      break;
    }
  }

  // Move down and right
  for (int i = row - 1; i >= 0; i--)
  {
    for (int j = 0; j <= 7; j++)
    {
      if (curBoardPieces[i][j] == '0' || charToPiece(curBoardPieces[i][j]).colour != activeColour)
      {
        lightUp(i, j);
      }
      else if (charToPiece(curBoardPieces[i][j]).colour == activeColour)
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
      if (curBoardPieces[i][j] == '0' || charToPiece(curBoardPieces[i][j]).colour != activeColour)
      {
        lightUp(i, j);
      }
      else if (charToPiece(curBoardPieces[i][j]).colour == activeColour)
      {
        break;
      }
    }
  }

  // Move up and right
  for (int i = row + 1; i <= 7 ; i++)
  {
    for (int j = 0; j <= 7; j++)
    {
      if (curBoardPieces[i][j] == '0' || charToPiece(curBoardPieces[i][j]).colour != activeColour)
      {
        lightUp(i, j);
      }
      else if (charToPiece(curBoardPieces[i][j]).colour == activeColour)
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
      if (curBoardPieces[i][j] == '0' || charToPiece(curBoardPieces[i][j]).colour != activeColour)
      {
        lightUp(i, j);
      }
      else if (charToPiece(curBoardPieces[i][j]).colour == activeColour)
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
    if (curBoardPieces[row - 1][col] == '0' || charToPiece(curBoardPieces[row - 1][col] ).colour != activeColour)
    {
      lightUp(row - 1, col);
    }
    if (col > 0 && curBoardPieces[row - 1][col - 1] == '0' || charToPiece(curBoardPieces[row - 1][col - 1] ).colour != activeColour)
    {
      lightUp(row - 1, col - 1);
    }
    if (col < 7 && curBoardPieces[row - 1][col + 1] == '0' || charToPiece(curBoardPieces[row - 1][col + 1] ).colour != activeColour)
    {
      lightUp(row - 1, col + 1);
    }
  }
  if (row < 7)
  {
    if (curBoardPieces[row + 1][col] == '0' || charToPiece(curBoardPieces[row + 1][col] ).colour != activeColour)
    {
      lightUp(row + 1, col);
    }
    if (col > 0 && curBoardPieces[row + 1][col - 1] == '0' || charToPiece(curBoardPieces[row + 1][col - 1] ).colour != activeColour)
    {
      lightUp(row + 1, col - 1);
    }
    if (col < 7 && curBoardPieces[row + 1][col + 1] == '0' || charToPiece(curBoardPieces[row + 1][col + 1] ).colour != activeColour)
    {
      lightUp(row + 1, col + 1);
    }
  }
  if (col > 0 && curBoardPieces[row][col - 1] == '0' || charToPiece(curBoardPieces[row][col - 1] ).colour != activeColour)
  {
    lightUp(row, col - 1);
  }
  if (col < 7 && curBoardPieces[row][col + 1] == '0' || charToPiece(curBoardPieces[row][col + 1] ).colour != activeColour)
  {
    lightUp(row, col + 1);
  }
}

void lightUp(int row, int col)
{
  digitalWrite(anodes[row], LOW);
  digitalWrite(anodes[row + 1], LOW);
  digitalWrite(cathodes[col], HIGH);
  digitalWrite(cathodes[col + 1], HIGH);
}

void flash()
{
  Piece fromPiece = charToPiece(liftedPiece);
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

  for (int i = 0; i < 9; i++)
  {
    pinMode(anodes[i], OUTPUT);
    pinMode(cathodes[i], OUTPUT);
    digitalWrite(anodes[i], HIGH);
  }

  // open serial port
  Serial.begin(9600);
}

int LED_NUM = 24;

void loop()
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

  identifyColors();

  Serial.print(gameState);
  Serial.print("\t");
  Serial.print(turns);
  Serial.print("\n");

  switch (gameState)
  {
  case 0:
    if (gameStartValid())
    { //&& gameStartPB) {
      assignPieces();
      gameState = 1;
    }
    break;
  case 1:
    sendFen();
    if (checkPick())
    {
      turns += 1;
      gameState = 2;
    }
    break;
  case 2:
    sendFen();
    if (checkPlace())
    {
      turns += 1;
      gameState = 1;
    }
    flash();
    break;
  default:
    break;
  }
  

  printHall();
  delay(delay_const);
  Serial.print("\n");
  Serial.print("\n");
  printColors();
  delay(delay_const);
  Serial.print("\n");
  Serial.print("\n");
  printBoard();
  delay(delay_const);
  Serial.print("\n");

  updateColors();
  delay(delay_const);
}
// void loop()
// {
//     loopHallSensors();

//     digitalWrite(LED_NUM, HIGH);

//     if(LED_NUM == 40){
//         digitalWrite(LED_NUM-2, LOW);
//         LED_NUM = 24;
//     }
//     else{
//         if (LED_NUM == 24)
//         {
//             digitalWrite(40, LOW);

//         }
//         digitalWrite(LED_NUM-2, LOW);
//         LED_NUM += 2;
//     }

//     delay(delay_const);
// }
