// Need to define all of the constants and functions that the Arduino does implicitly on compilation 
// to accurately simulate the hardware.

#pragma once
#include <map>
#include <string>
#include <iostream>
#include <ctime>
#include <random>

#define A0 97
#define A3 94
#define A1 96
#define A2 95
#define A4 93
#define A5 92
#define A6 91
#define A7 90
#define A8 89
#define A9 88

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2
#define ADC_LEN 12

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352

#define CHANGE 1
#define FALLING 2
#define RISING 3


#ifndef min
    #define min(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef max
    #define max(a,b) ((a)>(b)?(a):(b))
#endif
#ifndef abs
    #define abs(x) ((x)>0?(x):-(x))
#endif
#ifndef constrain
    #define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif
#ifndef round
    #define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#endif
#ifndef radians
    #define radians(deg) ((deg)*DEG_TO_RAD)
#endif
#ifndef degrees
    #define degrees(rad) ((rad)*RAD_TO_DEG)
#endif
#ifndef sq
    #define sq(x) ((x)*(x))
#endif

using namespace std;

// Hold the "digital" readings for pins (1 or 0) in a sequence
struct PinValues {
    int val[1024];
    int iter;

    PinValues() : val{}, iter(0) {}
};

// Simulate reading and writing to a pin
class PinSimulation{
public:
    map<int, int> lastVal;
    map<int, PinValues> values;

    void writePin(int pin, int val) {
        if (values.find(pin) == values.end()) {
            lastVal[pin] = 0;
        }
        values[pin].val[lastVal[pin]] = val;

        // handle many read/writes to clk and cs pins while reading hall rows
        if (lastVal[pin] < 512)
        {
            lastVal[pin]++;
        }
        else
        {
            lastVal[pin] = 0;
        }
    }
    void reWritePin(int pin)
    {
        lastVal[pin] = 0;
    }
    void printPinValues(int pin)
    {
        cout << "______Pin " << pin << ":______\n";
        for (int i = 0; i < lastVal[pin]; i++)
        {
            cout << values[pin].val[i] << "\t";
        }
        cout << '\n';
    }
    void printAllPins() {
        for(map<int, PinValues>::iterator iter = values.begin(); iter != values.end(); ++iter)
        {
            printPinValues(iter->first);
        }
    }
    void resetPinIterator(int pin) {
        values[pin].iter = 0;
    }
    void resetAllPinIterators() {
        for (map<int, PinValues>::iterator iter = values.begin(); iter != values.end(); ++iter)
        {
            resetPinIterator(iter->first);
        }
    }

    // currently mapped pins on the Arduino Mega
    PinSimulation() {
        values = {
            {A0, PinValues()}, {A3, PinValues()}, {29, PinValues()}, {35, PinValues()}, {25, PinValues()}, {41, PinValues()},
            {27, PinValues()}, {31, PinValues()}, {37, PinValues()}, {39, PinValues()}, {33, PinValues()}, {34, PinValues()},
            {28, PinValues()}, {26, PinValues()}, {30, PinValues()}, {24, PinValues()}, {40, PinValues()}, {32, PinValues()},
            {36, PinValues()}, {38, PinValues()}, {A1, PinValues()}, {A2, PinValues()}, {A4, PinValues()}, {A5, PinValues()},
            {A6, PinValues()}, {A7, PinValues()}, {A8, PinValues()}, {A9, PinValues()}, {12, PinValues()}, {13, PinValues()},
            {10, PinValues()}, {11, PinValues()}, {8,  PinValues()}, {9,  PinValues()}, {6,  PinValues()}, {7,  PinValues()}
        };

        for(map<int, PinValues>::iterator iter = values.begin(); iter != values.end(); ++iter)
        {
            lastVal[iter->first] = 0;
        }
    }
    ~PinSimulation() {
        lastVal.clear();
        values.clear();
    }
};

// Simulate reading from and writing to the serial buffer when no serial connection is present for testing
class SerialStream {
public:
    string input;
    string output;
    int baudRate;

    void begin(int baud){
        baudRate = baud;
        cout << "Serial Communication initialized with baudrate: " << baud << "\n";
    }
    bool available(){
        return baudRate != 0;
    }
    int read()
    {
        return input.front();
    }

    // Handle multiple different data types when writing to Serial buffer
    size_t write(const char c)
    {
        size_t size = sizeof(char);
        output += c;
        return size;
    }
    size_t write(const char *str)
    {
        size_t size = sizeof(str);
        output += str;
        return size;
    }
    size_t print(const char c)
    {
        if (output.empty()) {
            output = string({c});
        }
        else {
            output.append(string({c}));
        }
        size_t size = sizeof(c);
        // cout << c;
        return size;
    }
    size_t print(const char *str)
    {
        if (output.empty()) {
            output = string(str);
        }
        else {
            output.append(string(str));
        }
        size_t size = sizeof(str);
        return size;
    }
    size_t print(int i)
    {
        if (output.empty()) {
            output = to_string(i);
        }
        else {
            output.append(to_string(i));
        }
        size_t size = sizeof(char);
        return size;
    }
    size_t println(const char *str)
    {
        if (output.empty())
        {
            output = string(str) + "\n";
        }
        else
        {
            output.append(string(str) + "\n");
        }
        size_t size = sizeof(str);
        return size;
    }
    size_t println(int i)
    {
        if (output.empty())
        {
            output = to_string(i) + "\n";
        }
        else
        {
            output.append(to_string(i) + "\n");
        }
        size_t size = sizeof(int) + sizeof("\n");
        return size;
    }

    // Internal functions to check the contents of the Serial bufer
    string checkOutput() {
        string temp = output;
        output.clear();
        return temp;
    }
    string checkInput() {
        string temp = input;
        input.clear();
        return temp;
    }
    void recieveData(string str) {
        input = str;
    }
    void flush() 
    {
        input.clear();
        output.clear();
    }

    SerialStream() {
        baudRate = 0;
    }
    ~SerialStream() {
        flush();
    }
};

PinSimulation PinSim = PinSimulation();
map<int, int> PinModes;

// Mode is defined in the Arduino setup() function
void pinMode(int pin, uint8_t mode) {
    PinModes[pin] = mode;
}

// Read the contents of a pin
int digitalRead(int pin) {
    return PinSim.values[pin].val[PinSim.values[pin].iter++];
}

// Write to a pin
int digitalWrite(int pin, int value) {
    PinSim.writePin(pin, value);
    return value;
}

// can toggle comment to display how long the Arduino sleeps during each loop iteration
void delay(int ms) {
    //cout << "Arduino slept for " << ms << " milliseconds.\n";
}

// Convert a decimal value into a binary string that will be how the actual hardware sends the data
void writeAdc(int pin, int decimal)
{
    int i = ADC_LEN;
    int binary[ADC_LEN];

    // first bit of adc reading is not useful
    binary[--i] = 0;

    while (decimal > 0)
    {
        // cout << "i: " << i << "\t%2: " << decimal % 2 << "\t/2: " << decimal / 2 << "\n";

        binary[--i] = decimal % 2;
        decimal /= 2;
    }
    while (i > 0)
    {
        binary[--i] = 0;
    }
    i=0;
    while(i < ADC_LEN){
        PinSim.writePin(pin, binary[i++]);
    }
}

// Write the sensor values for an entire row of hall sensors
void writeAdcRow(int rx, int* val)
{
    for (int i = 0; i < 8; i++)
    {
        writeAdc(rx, *(val+i));
    };
}
