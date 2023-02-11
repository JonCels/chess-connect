const int RX_PIN = 2; // RX pin of the SerialCommunication module
const int TX_PIN = 3; // TX pin of the SerialCommunication module

enum GameMode {
    BEGINNER_MODE,
    NORMAL_MODE,
    ENGINE_MODE
};

enum GameState {
    init,
    play,
    end,
    reset
};

enum UserAction
{
    wait_white,
    wait_black,
    piece_lifted,
    remove_piece,
    promoting,
    valid_move,
    invalid_move,
    draw,
    resign,
    reset,
    none
};
