# Chess Connect

The Chess Connect project allows two users to play a game of chess on a physical board with the information being 
transmitted to an online web application over Bluetooth. Currently, there is no way for players to seamlessly switch 
between playing on a physical board and playing online, but Chess Connect intends to change this by creating a central 
platform that will provide flexibility and remove barriers for new players looking to learn the game.

Date of project start: September 12, 2022

## Authors

- [Alexander Van Kralingen](https://github.com/vankraa)
- [Arshdeep Aujla](https://github.com/aujlaa4)
- [Jonathan Cels](https://github.com/celsj)
- [Joshua Chapman](https://github.com/jtgc1)
- [Rupinder Nagra](https://github.com/RupinderN)



The folders and files for this project are as follows:

docs - Documentation for the project
refs - Reference material used for the project, including papers
src - Source code
test - Test cases
etc.

# Project Title

Short description of the project.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine.

### Prerequisites

- Arduino IDE
- Node.js

### Dependencies

#### Arduino

All of the following dependencies can be installed using the Arduino IDE Library Manager.

- [SoftwareSerial](https://docs.arduino.cc/learn/built-in-libraries/software-serial): This is a built-in Arduino library for serial communication on other pins than the hardware serial port.
- [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library): This is a graphics library for creating custom user interfaces on displays.
- [Adafruit_TFTLCD](https://github.com/adafruit/TFTLCD-Library): This is a hardware-specific library for controlling TFT LCD displays.
- [TouchScreen](https://github.com/adafruit/Adafruit_TouchScreen): This is a library for reading touch coordinates from a touchscreen.

### Installing

1. Clone the repository
2. Open the Arduino IDE and load the `LCD.ino` sketch from the `src/arduinoUno/LCD` folder
3. Upload the sketch to the Arduino UNO
4. Open a terminal and navigate to the `web-app` folder
5. Run `npm install` to install the required Node.js packages
6. Run `npm start` to start the React Web Application
7. Access the Web Application at `http://localhost:3000`

## Built With

- Arduino IDE
- React
- Node.js

## License

This project is licensed under the [the Apache License, Version 2.0](./LICENSE).
