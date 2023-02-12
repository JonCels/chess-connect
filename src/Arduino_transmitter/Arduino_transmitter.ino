char message[100] = "hello this is a test of sending one hundred characters sending between two different arduinos yeah ";

void setup() {
Serial.begin(9600);
}

void loop() {
Serial.write("hello this is a test of sending sixty four chars to arduinos ye\n"); //Write the serial data
  delay(5000);
}
