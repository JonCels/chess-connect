

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
char pieceToChar(Piece piece);