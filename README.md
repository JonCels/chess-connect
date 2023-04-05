# Chess Connect

The Chess Connect project allows two users to play a game of chess on a physical board with the information being transmitted to an online web application over Bluetooth. Our vision is to create a central platform that will provide flexibility and remove barriers for new players looking to learn the game.

*Date of project start: September 12, 2022*

## Authors

- [Alexander Van Kralingen](https://github.com/vankraa)
- [Arshdeep Aujla](https://github.com/aujlaa4)
- [Jonathan Cels](https://github.com/celsj)
- [Joshua Chapman](https://github.com/jtgc1)
- [Rupinder Nagra](https://github.com/RupinderN)


## Repository Structure

The folders and files for this project are as follows:

- **docs** - Documentation for the project
  - ProblemStatementAndGoals: [Problem Statement](docs/ProblemStatementAndGoals/ProblemStatement.pdf)
  - DevelopmentPlan: [Development Plan](docs/DevelopmentPlan/DevelopmentPlan.pdf)
  - SRS: [Software Requirements Specification](docs/SRS/SRS.pdf)
  - Design
    - SoftArchitrcture: [Module Guide](docs/Design/SoftArchitecture/MG.pdf)
    - SoftDetailedDes: [Module Interface Specification](docs/Design/SoftDetailedDes/MIS.pdf)
    - SystDesign: [System Design](docs/Design/SystDesign/SystDes.pdf)
  - HazardAnalysis: [Hazard Analysis](docs/HazardAnalysis/HazardAnalysis.pdf)
  - VnVPlan: [Verification and Validation Plan](docs/VnVPlan/VnVPlan.pdf)
  - VnVReport: [Verification and Validation Report](docs/VnVReport/VnVReport.pdf)
- **refs** - Reference material used for the documentation
- **src** - Source code
  - Arduino_transmitter: Part of the development stage and will be removed in future revisions.
  - arduinoUno/LCD: Source code to run the Arduino controller powering the chess board. Name change will be required for final production.
  - chess-app: Front-end of the web application
  - chess-app-backend: Back-end of the web application
  - Embedded: First revision of the embedded software for the chess board controller. To be removed in future revisions.
- **test** - Test cases
  - EmbeddedTest: Used in conjunction with the src/Embedded folder. Mock Arduino and test setup to be reworked to support the new src/arduinoUno/LCD source code.

## Getting Started

These instructions will get you a copy of the project up and running.

### Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software)
- [Node.js](https://nodejs.org/en/download)
- [React Library](https://react.dev/)

### Dependencies

**Arduino**

All of the following dependencies can be installed using the Arduino IDE Library Manager.

- [SoftwareSerial](https://docs.arduino.cc/learn/built-in-libraries/software-serial): This is a built-in Arduino library for serial communication on other pins than the hardware serial port.
- [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library): This is a graphics library for creating custom user interfaces on displays.
- [Adafruit_TFTLCD](https://github.com/adafruit/TFTLCD-Library): This is a hardware-specific library for controlling TFT LCD displays.
- [TouchScreen](https://github.com/adafruit/Adafruit_TouchScreen): This is a library for reading touch coordinates from a touchscreen.

**Node.js**

<!-- ADD WEB APP DEPENDENCIES HERE -->

### Installing
<!-- MODIFY INSTRUCTIONS FOR RUNNING WEB APP -->

1. Clone the repository
2. Open the Arduino IDE and load the `LCD.ino` sketch from the `src/arduinoUno/LCD` folder
3. Upload the sketch to the Arduino UNO
4. Open a terminal and navigate to the `chess-app` folder
5. Run `npm install` to install the required Node.js packages
6. Run `npm start` to start the React Web Application
7. Access the Web Application at `http://localhost:3000`


## License

This project is licensed under the [the Apache License, Version 2.0](./LICENSE).
