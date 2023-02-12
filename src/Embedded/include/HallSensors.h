#pragma once
#define delay_const 2000

#define clk 97
#define cs 94

#define Row1LedAnode 35
#define Row2LedAnode 41
#define Row3LedAnode 39
#define Row4LedAnode 33
#define Row5LedAnode 29
#define Row6LedAnode 25
#define Row7LedAnode 27
#define Row8LedAnode 37
#define Row9LedAnode 39

#define aColLedCathode 34
#define bColLedCathode 28
#define cColLedCathode 26
#define dColLedCathode 30
#define eColLedCathode 24
#define fColLedCathode 40
#define gColLedCathode 32
#define hColLedCathode 36
#define iColLedCathode 38

#define arx 96
#define atx 95
#define brx 93
#define btx 92
#define crx 91
#define ctx 90
#define drx 89
#define dtx 88
#define erx 12
#define etx 13
#define frx 10
#define ftx 11
#define grx 8
#define gtx 9
#define hrx 6
#define htx 7

#include <CommonTypes.h>

int rawStates[8][8] = { {1, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0}};

int adjStates[8][8] = { {0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0}};

int baseReadings[8][8] = {{210, 210, 210, 210, 210, 210, 210, 210},
                        {210, 210, 210, 210, 210, 210, 210, 210},
                        {210, 210, 210, 210, 210, 210, 210, 210},
                        {210, 210, 210, 210, 210, 210, 210, 210},
                        {210, 210, 210, 210, 210, 210, 210, 210},
                        {210, 210, 210, 210, 210, 210, 210, 210},
                        {210, 210, 210, 210, 210, 210, 210, 210},
                        {210, 210, 210, 210, 210, 210, 210, 210}};
