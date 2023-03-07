#include "./ArduinoController.cpp"

using namespace std;



random_device rd;  // Will be used to obtain a seed for the random number engine
mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
uniform_int_distribution<> dist(0, WHITE_HALL);
uniform_int_distribution<> distrib(BLACK_HALL, 512);
uniform_int_distribution<> distribute(WHITE_HALL, BLACK_HALL);
int organizedHallValues[8][8];
int rawHallValues[8][8];

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

bool assert_equal(Square *testBoard, Square *expectedBoard)
{
    bool equal = true;
    for (int i = 0; i < 64; i++)
    {
        equal = *(testBoard + i) == *(expectedBoard + i);
    }
    
    return equal;
}
bool assert_equal(int *arr1, int *arr2)
{
    bool equal = true;
    for (int i = 0; i < 64; i++)
    {
        equal = *(arr1 + i) == *(arr2 + i);
    }

    return equal;
}
bool assert_equal(char a, char b)
{
    return a == b;
}
bool assert_equal(int a, int b)
{
    return a == b;
}


// def assert_true(bool obj){
//     throw 
// }

// def highlighBlackPawnCapturesPieceTest(){
//     movePiece()
// }

int main(void) {
    resetChessBoard();
    assert_equal(*currentBoard, *oldBoard);

    // generateHallValues();
    // mapHallValuesToSensors();

    // lightUp(1, 2);
    // RWPins.printPinValues(anodes[1]);
    // RWPins.printPinValues(cathodes[2]);

    // // loop();
    // resetChessBoard();
    // cout << "\n\n" << (currentBoard[0][1] == oldBoard[0][1]) << "\n\n";
    // currentBoard[0][1] = Square(0, 1);
    // cout << "\n\n" << (currentBoard[0][1] == oldBoard[0][1]) << "\n\n";
    // printBoard();
    // cout << Serial.output << "\n\n";

    // int arxrow[8] = {342, 322, 353, 344, 318, 329, 347, 333};
    writeAdcRow(arx, rawHallValues[0]);
    writeAdcRow(brx, rawHallValues[1]);
    writeAdcRow(crx, rawHallValues[2]);
    writeAdcRow(drx, rawHallValues[3]);
    writeAdcRow(erx, rawHallValues[4]);
    writeAdcRow(frx, rawHallValues[5]);
    writeAdcRow(grx, rawHallValues[6]);
    writeAdcRow(hrx, rawHallValues[7]);

    // RWPins.printPinValues(arx);
    // RWPins.printPinValues(brx);
    // RWPins.printPinValues(crx);
    // RWPins.printPinValues(drx);
    // RWPins.printPinValues(erx);
    // RWPins.printPinValues(frx);
    // RWPins.printPinValues(grx);
    // RWPins.printPinValues(hrx);
    readHallSensors();
    cout << "\n\n" + Serial.output;

    return 0;
}