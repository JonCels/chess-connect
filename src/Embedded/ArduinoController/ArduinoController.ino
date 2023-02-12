
int clk = A0;
int cs = A3; 
int delay_const = 2000;

int Row1LedAnode = 35;
int Row2LedAnode = 41;
int Row3LedAnode = 39;
int Row4LedAnode = 33;
int Row5LedAnode = 29;
int Row6LedAnode = 25;
int Row7LedAnode = 27;
int Row8LedAnode = 37;
int Row9LedAnode = 39;

int aColLedCathode = 34;
int bColLedCathode = 28;
int cColLedCathode = 26;
int dColLedCathode = 30;
int eColLedCathode = 24;
int fColLedCathode = 40;
int gColLedCathode = 32;
int hColLedCathode = 36;
int iColLedCathode = 38;

int arx = A1; 
int atx = A2; 
int brx = A4; 
int btx = A5; 
int crx = A6; 
int ctx = A7; 
int drx = A8; 
int dtx = A9; 
int erx = 12; 
int etx = 13; 
int frx = 10; 
int ftx = 11; 
int grx = 8; 
int gtx = 9; 
int hrx = 6; 
int htx = 7; 


int rawStates[8][8] = {{1, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0}};

int adjStates[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0}};

int readHall(int adcnum, int rx, int tx)
{
    digitalWrite(cs, HIGH);
    digitalWrite(clk, LOW);
    digitalWrite(cs, LOW);
    //delayMicroseconds(2);

    int commandout = adcnum;
    commandout |= 0x18;
    commandout <<= 3;

    for (int i = 0; i < 5; i++)
    {
        if (commandout & 0x80)
        {
            digitalWrite(tx, HIGH);
        }
        else
        {
            digitalWrite(tx, LOW);
        }
        //delayMicroseconds(2);

        commandout <<= 1;
        digitalWrite(clk, HIGH);
        //delayMicroseconds(2);
        digitalWrite(clk, LOW);
        //delayMicroseconds(2);
    }

    int adcout = 0;

    for (int i = 0; i < 12; i++)
    {
        digitalWrite(clk, HIGH);
        //delayMicroseconds(2);
        digitalWrite(clk, LOW);
        //delayMicroseconds(2);

        adcout <<= 1;

        if (digitalRead(rx))
            adcout |= 0x1;
    }

    digitalWrite(cs, HIGH);

    adcout >>= 1;
    return adcout;
}

void readRow(int row, int rx, int tx)
{
    int i;

    for (i = 0; i < 8; i++)
    {
        rawStates[row][i] = readHall(i, rx, tx);
    }
}

void adjust()
{
    adjStates[7][0] = rawStates[2][3];
    adjStates[7][1] = rawStates[2][2];
    adjStates[7][2] = rawStates[2][1];
    adjStates[7][3] = rawStates[2][0];
    adjStates[7][4] = rawStates[1][7];
    adjStates[7][5] = rawStates[1][6];
    adjStates[7][6] = rawStates[1][5];
    adjStates[7][7] = rawStates[1][4];
    adjStates[6][0] = rawStates[2][7];
    adjStates[6][1] = rawStates[2][6];
    adjStates[6][2] = rawStates[2][5];
    adjStates[6][3] = rawStates[2][4];
    adjStates[6][4] = rawStates[1][3];
    adjStates[6][5] = rawStates[1][2];
    adjStates[6][6] = rawStates[1][1];
    adjStates[6][7] = rawStates[1][0];
    adjStates[5][0] = rawStates[3][3];
    adjStates[5][1] = rawStates[3][2];
    adjStates[5][2] = rawStates[3][1];
    adjStates[5][3] = rawStates[3][0];
    adjStates[5][4] = rawStates[0][7];
    adjStates[5][5] = rawStates[0][6];
    adjStates[5][6] = rawStates[0][5];
    adjStates[5][7] = rawStates[0][4];
    adjStates[4][0] = rawStates[3][7];
    adjStates[4][1] = rawStates[3][6];
    adjStates[4][2] = rawStates[3][5];
    adjStates[4][3] = rawStates[3][4];
    adjStates[4][4] = rawStates[0][3];
    adjStates[4][5] = rawStates[0][2];
    adjStates[4][6] = rawStates[0][1];
    adjStates[4][7] = rawStates[0][0];
    adjStates[3][0] = rawStates[7][0];
    adjStates[3][1] = rawStates[7][1];
    adjStates[3][2] = rawStates[7][2];
    adjStates[3][3] = rawStates[7][3];
    adjStates[3][4] = rawStates[5][0];
    adjStates[3][5] = rawStates[5][1];
    adjStates[3][6] = rawStates[5][2];
    adjStates[3][7] = rawStates[5][3];
    adjStates[2][0] = rawStates[7][4];
    adjStates[2][1] = rawStates[7][5];
    adjStates[2][2] = rawStates[7][6];
    adjStates[2][3] = rawStates[7][7];
    adjStates[2][4] = rawStates[5][7];
    adjStates[2][5] = rawStates[5][6];
    adjStates[2][6] = rawStates[5][5];
    adjStates[2][7] = rawStates[5][4];
    adjStates[1][0] = rawStates[6][0];
    adjStates[1][1] = rawStates[6][1];
    adjStates[1][2] = rawStates[6][2];
    adjStates[1][3] = rawStates[6][3];
    adjStates[1][4] = rawStates[4][3];
    adjStates[1][5] = rawStates[4][2];
    adjStates[1][6] = rawStates[4][1];
    adjStates[1][7] = rawStates[4][0];
    adjStates[0][0] = rawStates[6][4];
    adjStates[0][1] = rawStates[6][5];
    adjStates[0][2] = rawStates[6][6];
    adjStates[0][3] = rawStates[6][7];
    adjStates[0][4] = rawStates[4][7];
    adjStates[0][5] = rawStates[4][6];
    adjStates[0][6] = rawStates[4][5];
    adjStates[0][7] = rawStates[4][4];
}

void printHall()
{
    int i, j;
    // char columns[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};

    Serial.println("\ta\tb\tc\td\te\tf\tg\th");
    for (i = 0; i < 8; i++)
    {
        Serial.print(8 - i);
        Serial.print("\t");
        for (j = 0; j < 8; j++)
        {
            Serial.print(adjStates[i][j]);
            Serial.print("\t");
        }
        Serial.print("\n");
        delay(1);
    }
}

void setup()
{
    pinMode(cs, OUTPUT);
    pinMode(arx, INPUT);
    pinMode(atx, OUTPUT);
    pinMode(brx, INPUT);
    pinMode(btx, OUTPUT);
    pinMode(crx, INPUT);
    pinMode(ctx, OUTPUT);
    pinMode(drx, INPUT);
    pinMode(dtx, OUTPUT);
    pinMode(erx, INPUT);
    pinMode(etx, OUTPUT);
    pinMode(frx, INPUT);
    pinMode(ftx, OUTPUT);
    pinMode(grx, INPUT);
    pinMode(gtx, OUTPUT);
    pinMode(hrx, INPUT);
    pinMode(htx, OUTPUT);
    pinMode(clk, OUTPUT);

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
    readRow(0, arx, atx);
    readRow(1, brx, btx);
    readRow(2, crx, ctx);
    readRow(3, drx, dtx);
    readRow(4, erx, etx);
    readRow(5, frx, ftx);
    readRow(6, grx, gtx);
    readRow(7, hrx, htx);

    adjust();

    printHall();
    Serial.print("\n");

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
