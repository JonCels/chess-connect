#include <SoftwareSerial.h>
//Use 4800 baud on serial monitor, with Carriage Return (or carriage return and newline)
SoftwareSerial BTserial(10, 11); // TX | RX
void setup() {
	Serial.begin(4800);
	BTserial.begin(9600);  //Default Baud for comm, it may be different for your Module. 
	Serial.println("The bluetooth gates are open.\n Connect to HC-05 from any other bluetooth device with 1234 as pairing key!.");

}

void loop() {
  	// Feed any data from bluetooth to Terminal.
	if (BTserial.available()) {
		//Serial.print(BTserial.read());
		Serial.write(BTserial.read());
	}
	// Feed all data from termial to bluetooth
	if (Serial.available()){
		BTserial.write(Serial.read());
	}
}