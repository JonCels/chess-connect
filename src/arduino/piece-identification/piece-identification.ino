int sensor = 0;
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
sensor = analogRead(0);
Serial.println(sensor);
if(sensor>0 and sensor<=30) {
  Serial.println("Black Pawn");
}
else if(sensor>30 and sensor<=60) {
  Serial.println("Black Bishop");
}
else if(sensor>60 and sensor<=90) {
  Serial.println("Black Knight");
}
else if(sensor>90 and sensor<=120) {
  Serial.println("Black Rook");
}
else if(sensor>120 and sensor<=150) {
  Serial.println("Black Queen");
}
else if(sensor>150 and sensor<=180) {
  Serial.println("Black King");
}
else if(sensor>220 and sensor<=250) {
  Serial.println("White Pawn");
}
else if(sensor>250 and sensor<=280) {
  Serial.println("White Bishop");
}
else if(sensor>280 and sensor<=310) {
  Serial.println("White Knight");
}
else if(sensor>310 and sensor<=340) {
  Serial.println("White Rook");
}
else if(sensor>340 and sensor<=370) {
  Serial.println("White Queen");
}
else if(sensor>370 and sensor<=400) {
  Serial.println("White King");
}
else {
Serial.println("No Piece");
}
}
