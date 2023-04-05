#include "./ArduinoController.cpp"

using namespace std;

map<int, const char *> errors;
int exitCode = 0;

// Generate random values for hall sensors
random_device rd;  // Will be used to obtain a seed for the random number engine
mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
uniform_int_distribution<> dist(0, WHITE_HALL);
uniform_int_distribution<> distrib(BLACK_HALL, 512);
uniform_int_distribution<> distribute(WHITE_HALL, BLACK_HALL);
int organizedHallValues[8][8];
int rawHallValues[8][8];

// Compare data from the entire board
bool assert_equal(Square *actualBoard, Square *expectedBoard)
{
    bool equal = true;
    for (int i = 0; i < 64; i++)
    {
        equal = *(actualBoard + i) == *(expectedBoard + i);

        if (!equal)
        {
            cout << "\nnMismatched Piece values at row: " << i / 8 << " col: " << i % 8 << "\n";
            cout << "Expected:\tActual:\n";
            cout << pieceToChar((*(expectedBoard + i)).piece) << "\t\t" << pieceToChar((*(actualBoard + i)).piece) << "\n";
        }
    }
    return equal;
}

// Compare the values of the Square struct
bool assert_equal(Square actualBoard, Square expectedBoard)
{
    bool equal = true;
    equal = actualBoard == expectedBoard;
    if (!equal)
    {
        cout << "\nnMismatched Piece values \n";
        cout << "Expected:\tActual:\n";
        cout << pieceToChar(expectedBoard.piece) << "\t\t" << pieceToChar(actualBoard.piece) << "\n";
    }
    return equal;
}

// Compare the contents of int arrays (used primarily for hall sensors)
bool assert_equal(int *arr1, int *arr2)
{
    bool equal = true;
    for (int i = 0; i < 64; i++)
    {
        equal = *(arr1 + i) == *(arr2 + i);
        if (!equal)
        {
            cout << "\nMismatched int values at row: " << i / 8 << " col: " << i % 8 << "\n";
            cout << "Expected:\tActual:\n";
            cout << *(arr1 + i) << "\t\t" << *(arr2 + i) << "\n";
        }
    }
    return equal;
}

// Compare char values
bool assert_equal(char a, char b)
{
    bool result = a == b;
    if (!result)
    {
        cout << "\nMismatched char\n";
        cout << "Expected:\tActual:\n";
        cout << a << "\t\t" << b << "\n";
        return false;
    }
    return result;
}

// Compare numerical values
bool assert_equal(int a, int b)
{
    bool result = a == b;
    if (!result)
    {
        cout << "\nMismatched int\n";
        cout << "Expected:\tActual:\n";
        cout << a << "\t\t" << b << "\n";
        return false;
    }
    return result;
}

// Check if an error was raised
void check(bool b, const char *c, int l)
{
    if (!b)
        errors[l] = c;
}

// Run another iteration of the Arduino's loop function, start reading simulated pins from beginning.
void loopArduino()
{
    loop();
    PinSim.resetAllPinIterators();
}

// Generate random values for hall sensors
int randHall(Colour colour) {
    switch (colour)
    {
    case WHITE:
        return dist(gen);
    case BLACK:
        return distrib(gen);
    case NO_COLOUR:
        return distribute(gen);
    default:
        return 0;
    }
}

// Adjust for hardware pin configurations
void mapHallValuesToSensors()
{
    rawHallValues[2][3] = organizedHallValues[7][0];
    rawHallValues[2][2] = organizedHallValues[7][1];
    rawHallValues[2][1] = organizedHallValues[7][2];
    rawHallValues[2][0] = organizedHallValues[7][3];
    rawHallValues[1][7] = organizedHallValues[7][4];
    rawHallValues[1][6] = organizedHallValues[7][5];
    rawHallValues[1][5] = organizedHallValues[7][6];
    rawHallValues[1][4] = organizedHallValues[7][7];
    rawHallValues[2][7] = organizedHallValues[6][0];
    rawHallValues[2][6] = organizedHallValues[6][1];
    rawHallValues[2][5] = organizedHallValues[6][2];
    rawHallValues[2][4] = organizedHallValues[6][3];
    rawHallValues[1][3] = organizedHallValues[6][4];
    rawHallValues[1][2] = organizedHallValues[6][5];
    rawHallValues[1][1] = organizedHallValues[6][6];
    rawHallValues[1][0] = organizedHallValues[6][7];
    rawHallValues[3][3] = organizedHallValues[5][0];
    rawHallValues[3][2] = organizedHallValues[5][1];
    rawHallValues[3][1] = organizedHallValues[5][2];
    rawHallValues[3][0] = organizedHallValues[5][3];
    rawHallValues[0][7] = organizedHallValues[5][4];
    rawHallValues[0][6] = organizedHallValues[5][5];
    rawHallValues[0][5] = organizedHallValues[5][6];
    rawHallValues[0][4] = organizedHallValues[5][7];
    rawHallValues[3][7] = organizedHallValues[4][0];
    rawHallValues[3][6] = organizedHallValues[4][1];
    rawHallValues[3][5] = organizedHallValues[4][2];
    rawHallValues[3][4] = organizedHallValues[4][3];
    rawHallValues[0][3] = organizedHallValues[4][4];
    rawHallValues[0][2] = organizedHallValues[4][5];
    rawHallValues[0][1] = organizedHallValues[4][6];
    rawHallValues[0][0] = organizedHallValues[4][7];
    rawHallValues[7][0] = organizedHallValues[3][0];
    rawHallValues[7][1] = organizedHallValues[3][1];
    rawHallValues[7][2] = organizedHallValues[3][2];
    rawHallValues[7][3] = organizedHallValues[3][3];
    rawHallValues[5][0] = organizedHallValues[3][4];
    rawHallValues[5][1] = organizedHallValues[3][5];
    rawHallValues[5][2] = organizedHallValues[3][6];
    rawHallValues[5][3] = organizedHallValues[3][7];
    rawHallValues[7][4] = organizedHallValues[2][0];
    rawHallValues[7][5] = organizedHallValues[2][1];
    rawHallValues[7][6] = organizedHallValues[2][2];
    rawHallValues[7][7] = organizedHallValues[2][3];
    rawHallValues[5][7] = organizedHallValues[2][4];
    rawHallValues[5][6] = organizedHallValues[2][5];
    rawHallValues[5][5] = organizedHallValues[2][6];
    rawHallValues[5][4] = organizedHallValues[2][7];
    rawHallValues[6][0] = organizedHallValues[1][0];
    rawHallValues[6][1] = organizedHallValues[1][1];
    rawHallValues[6][2] = organizedHallValues[1][2];
    rawHallValues[6][3] = organizedHallValues[1][3];
    rawHallValues[4][3] = organizedHallValues[1][4];
    rawHallValues[4][2] = organizedHallValues[1][5];
    rawHallValues[4][1] = organizedHallValues[1][6];
    rawHallValues[4][0] = organizedHallValues[1][7];
    rawHallValues[6][4] = organizedHallValues[0][0];
    rawHallValues[6][5] = organizedHallValues[0][1];
    rawHallValues[6][6] = organizedHallValues[0][2];
    rawHallValues[6][7] = organizedHallValues[0][3];
    rawHallValues[4][7] = organizedHallValues[0][4];
    rawHallValues[4][6] = organizedHallValues[0][5];
    rawHallValues[4][5] = organizedHallValues[0][6];
    rawHallValues[4][4] = organizedHallValues[0][7];
}

// Store simulated sensor noise in the mock Arduino pins
void generateHallValues() {
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (i <= 1)
            {
                organizedHallValues[i][j] = randHall(BLACK);
            }
            else if (i >= 6)
            {
                organizedHallValues[i][j] = randHall(WHITE);
            }
            else
            {
                organizedHallValues[i][j] = randHall(NO_COLOUR);
            }
        }
    }
}

// Initialize mock Arduino pins
void setupMockHallSensors()
{
    generateHallValues();
    mapHallValuesToSensors();
    for (int i = 0; i < 8; i++)
    {
        writeAdcRow(hallRx[i], rawHallValues[i]);
    }
}

// Reset board into initial game play mode (white and black pieces all in correct starting positions)
void setupBoard()
{
    setupMockHallSensors();
    loopArduino(); // enter PLAY_GAME state

    check(assert_equal('p', gameState), __FUNCTION__, __LINE__);
    check(gameStartValid(), __FUNCTION__, __LINE__);
    check(assert_equal(*currentBoard, *oldBoard), __FUNCTION__, __LINE__);
    check(assert_equal(*organizedHallValues, *adjStates), __FUNCTION__, __LINE__);

    loopArduino();// enter WAIT_PICK state
    check(assert_equal('w', gameState), __FUNCTION__, __LINE__);
}

// Test to read if a piece picked up will be recorded, and change the game state
void testReadPiece()
{
    setupBoard();

    // Simulate picking a piece
    organizedHallValues[0][1] = randHall(NO_COLOUR);
    mapHallValuesToSensors();
    PinSim.reWritePin(hallRx[0]);
    writeAdcRow(hallRx[0], rawHallValues[0]);

    // No Piece, No colour
    Square expectedSquare = Square(0, 1);

    // Checkpick() function inside Arduino's loop should catch this and update the pieces on the board
    loopArduino();

    // Make sure the state changes to PIECE_LIFTED ('l')
    check(assert_equal('l', gameState), __FUNCTION__, __LINE__);

    // Make sure the square in the board array is updated successfully
    check(assert_equal(expectedSquare, currentBoard[0][1]), __FUNCTION__, __LINE__);
}

// To be re-implemented for the final version of source code 
void testHighlightPawnValidMoves();
void testHighlightKnightValidMoves();
void testHighlightBishopValidMoves();
void testHighlightRookValidMoves();
void testHighlightQueenValidMoves();
void testHighlightKingValidMoves();

// Display errors after all tests complete
void printErrors()
{
    if (errors.empty()) {
        cout << "\n\nSUCCESS: All tests pass\n\n";
    }
    else {
        exitCode = (int)errors.size();
        cout << "\n\n_______ERRORS:_______\n\n";
        for (const auto &elem : errors)
        {
            cout << "\tAssertion failed in: " << elem.second << "() at line: " << elem.first << "\n";
        }
    }
}

// Main test running entry point
int main(void) {
    try
    {
        testReadPiece();
    
    // generateHallValues();
    // mapHallValuesToSensors();

    // lightUp(1, 2);
    // PinSim.printPinValues(anodes[1]);
    // PinSim.printPinValues(cathodes[2]);

    // // loop();
    // resetChessBoard();
    // cout << "\n\n" << (currentBoard[0][1] == oldBoard[0][1]) << "\n\n";
    // currentBoard[0][1] = Square(0, 1);
    // cout << "\n\n" << (currentBoard[0][1] == oldBoard[0][1]) << "\n\n";
    // printBoard();
    // cout << Serial.output << "\n\n";

    // writeAdcRow(arx, rawHallValues[0]);
    // writeAdcRow(brx, rawHallValues[1]);
    // writeAdcRow(crx, rawHallValues[2]);
    // writeAdcRow(drx, rawHallValues[3]);
    // writeAdcRow(erx, rawHallValues[4]);
    // writeAdcRow(frx, rawHallValues[5]);
    // writeAdcRow(grx, rawHallValues[6]);
    // writeAdcRow(hrx, rawHallValues[7]);

    // // PinSim.printPinValues(arx);
    // // PinSim.printPinValues(brx);
    // // PinSim.printPinValues(crx);
    // // PinSim.printPinValues(drx);
    // // PinSim.printPinValues(erx);
    // // PinSim.printPinValues(frx);
    // // PinSim.printPinValues(grx);
    // // PinSim.printPinValues(hrx);
    // readHallSensors();
    // cout << "\n\n" + Serial.output;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    printErrors();
    return 0;
}