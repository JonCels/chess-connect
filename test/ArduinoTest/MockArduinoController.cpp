#include <map>
#include <iostream>
#include <string>

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

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352

#define CHANGE 1
#define FALLING 2
#define RISING 3

#ifdef abs
#undef abs
#endif

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

using namespace std;

struct PinValues {
    short val[512];
    PinValues() : val({}) {}
};

class ReadWritePins{
public:
    map<int, int> index;
    map<int, PinValues> values;

    void writePin(int pin, int val) {
        if (values.find(pin) == values.end()) {
            index[pin] = 0;
        }
        
        values[pin].val[index[pin++]] = val;
    }
    void printPinValues(int pin) {
        cout << "______Pin " << pin << ":______\n";
        for (int i = 0; i < index[pin]; i++)
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

    ReadWritePins() {
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
            index[iter->first] = 0;
        }
    }
    ~ReadWritePins() {
        index.clear();
        values.clear();
    }
};

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
    char *read()
    {
        return input.data();
    }
    size_t write(char c)
    {
        size_t size = sizeof(char);
        output += c;
        return size;
    }
    size_t write(char *str)
    {
        size_t size = sizeof(str);
        output += str;
        return size;
    }
    size_t print(char *str)
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
    size_t println(char *str)
    {
        if (output.empty())
        {
            output = string(str);
        }
        else
        {
            output.append(string(str) + "\n");
        }
        size_t size = sizeof(str);
        return size;
    }
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

ReadWritePins RWPins = ReadWritePins();
map<int, int> PinModes;

void pinMode(int pin, uint8_t mode) {
    PinModes[pin] = mode;
}

int digitalRead(int pin) {
    return 0;
}

int digitalWrite(int pin, int value) {
    RWPins.writePin(pin, value);
    return value;
}

void delay(int ms) {
    cout << "Arduino slept for " << ms << " milliseconds.\n";
}