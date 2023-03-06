#include "./ArduinoController.cpp"
#include <iostream>
#include <ctime>

// using namespace std;

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

    lightUp(1, 2);
    RWPins.printPinValues(anodes[1]);
    RWPins.printPinValues(cathodes[2]);
    return 0;
}