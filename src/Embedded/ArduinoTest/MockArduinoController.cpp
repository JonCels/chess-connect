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
#define LOW 0
#define HIGH 1

using namespace std;

class ReadWriteValues{
public:
    map<int, int> index;
    map<int, int[512]> values;
    void add(int pin, int val){
        index[pin] = 0;
        values[pin][0] = {val};
    }
};

int abs(int a) {
    return ((a > 0) ? a : -a);
}

int min(int a, int b) {
    return ((a < b) ? a : b);
}

int digitalRead(int pin) {
    return 0;
}

int digitalWrite(int pin, int value) {
    WriteValues[pin];
    return value;
}

class SerialStream {
    string input;
    string output;

    char *read(char *str)
    {
        input = string(str);
        return str;
    }
    size_t write(char *str)
    {
        size_t size = sizeof(str);
        output = string(str);
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
    void flush() 
    {
        input.clear();
        output.clear();
    }
};