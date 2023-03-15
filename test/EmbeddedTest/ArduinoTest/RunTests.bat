DUINO='..\..\..\src\Embedded\ArduinoController\ArduinoController.ino'
DUINOPP='.\ArduinoController.cpp'
xcopy %DUINO% %DUINOPP% /y

g++ ArduinoControllerTest.cpp -o ArduinoControlertest.exe

.\ArduinoControlertest.exe

exit %ERRORLEVEL%