DUINO='../../../src/Embedded/ArduinoController/ArduinoController.ino'
DUINOPP='ArduinoController.cpp'
cp -f -T $DUINO $DUINOPP

g++ ArduinoControllerTest.cpp -o ArduinoControlertest.exe

./ArduinoControlertest.exe