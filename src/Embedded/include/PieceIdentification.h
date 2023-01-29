const int RX_PIN = 2; // RX pin of the SerialCommunication module
const int TX_PIN = 3; // TX pin of the SerialCommunication module

// Enum to represent the different types of chess pieces
enum PieceType
{
    NONE,
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
    BLACK
};

// Structure to represent a chess piece
// Members: PieceType, colour (0 for white, 1 for black)
struct Piece
{
    PieceType type;
    Colour colour; // 0 for white, 1 for black
};

Piece ReadSensors(int sensor = 0);