char message[100];
String str;

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    // Serial.readBytes(message,100); //Read the serial data and store in var
    str = Serial.readStringUntil('\n');
    Serial.print("message1: ");
    Serial.println(str); //Print data on Serial Monitor
    delay(5000);
    str = "";
  }
}