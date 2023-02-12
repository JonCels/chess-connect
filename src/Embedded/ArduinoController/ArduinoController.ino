#include "HallSensors.h"
#include "ChessBoard.h"
#include "PieceIdentification.h"

void setup()
{
    setupHallSensors();
    
    for (int i = 24; i <= 41; i++)
    {
        pinMode(i, OUTPUT);
    }

    // open serial port
    Serial.begin(9600);

    for (int i = 24; i <= 41; i+=2)
    {
        digitalWrite(i, LOW);
    }
    
}

int LED_NUM = 24;

void loop()
{
    loopHallSensors();

    digitalWrite(LED_NUM, HIGH);

    if(LED_NUM == 40){
        digitalWrite(LED_NUM-2, LOW);
        LED_NUM = 24;
    }
    else{
        if (LED_NUM == 24)
        {
            digitalWrite(40, LOW);
            
        }
        digitalWrite(LED_NUM-2, LOW);
        LED_NUM += 2;
    }

    delay(delay_const);
}
