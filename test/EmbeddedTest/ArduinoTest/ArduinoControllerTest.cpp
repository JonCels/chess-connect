#include "./ArduinoController.cpp"
#include <iostream>
#include <ctime>

using namespace std;

// def assert_equal(Square *testBoard, Square *expectedBoard)
// {
//     if testBoard == exp
// }

// def assert_true(bool obj){
//     throw 
// }

// def highlighBlackPawnCapturesPieceTest(){
//     movePiece()
// }

int main(void) {

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

    int arxrow[8] = {342, 322, 353, 344, 318, 329, 347, 333};
    writeAdcRow(arx, arxrow);
    writeAdcRow(brx, arxrow);
    writeAdcRow(crx, arxrow);
    writeAdcRow(drx, arxrow);
    writeAdcRow(erx, arxrow);
    writeAdcRow(frx, arxrow);
    writeAdcRow(grx, arxrow);
    writeAdcRow(hrx, arxrow);

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