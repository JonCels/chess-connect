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

enum GameMode {
    BEGINNER_MODE,
    NORMAL_MODE,
    ENGINE_MODE
};

enum GameState {
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
struct Position {
    int row;
    int col;
};

// Structure to represent a chess piece
// Members: PieceType, colour (0 for white, 1 for black
struct Piece
{
    PieceType type = NO_PIECE;
    Colour colour = NO_COLOUR;
};