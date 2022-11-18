int sensor1 = 0;
int sensor2 = 0;
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
sensor1 = analogRead(0);
Serial.println(sensor1);
if(sensor1>0 and sensor1<=30) {
  Serial.println("Black Pawn");
}
else if(sensor1>30 and sensor1<=60) {
  Serial.println("Black Bishop");
}
else if(sensor1>60 and sensor1<=90) {
  Serial.println("Black Knight");
}
else if(sensor1>90 and sensor1<=120) {
  Serial.println("Black Rook");
}
else if(sensor1>120 and sensor1<=150) {
  Serial.println("Black Queen");
}
else if(sensor1>150 and sensor1<=180) {
  Serial.println("Black King");
}
else if(sensor1>220 and sensor1<=250) {
  Serial.println("White King");
}
else if(sensor1>250 and sensor1<=280) {
  Serial.println("White Queen");
}
else if(sensor1>280 and sensor1<=310) {
  Serial.println("White Rook");
}
else if(sensor1>310 and sensor1<=340) {
  Serial.println("White Knight");
}
else if(sensor1>340 and sensor1<=370) {
  Serial.println("White Bishop");
}
else if(sensor1>370 and sensor1<=400) {
  Serial.println("White Pawn");
}
else {
Serial.println("No Piece");
Serial.write(13);
}
//sensor2
sensor2 = analogRead(1);
Serial.println(sensor2);
if(sensor2>0 and sensor2<=30) {
  Serial.println("Black Pawn");
}
else if(sensor2>30 and sensor2<=60) {
  Serial.println("Black Bishop");
}
else if(sensor2>60 and sensor2<=90) {
  Serial.println("Black Knight");
}
else if(sensor2>90 and sensor2<=120) {
  Serial.println("Black Rook");
}
else if(sensor2>120 and sensor2<=150) {
  Serial.println("Black Queen");
}
else if(sensor2>150 and sensor2<=180) {
  Serial.println("Black King");
}
else if(sensor2>220 and sensor2<=250) {
  Serial.println("White Pawn");
}
else if(sensor2>250 and sensor2<=280) {
  Serial.println("White Bishop");
}
else if(sensor2>280 and sensor2<=310) {
  Serial.println("White Knight");
}
else if(sensor2>310 and sensor2<=340) {
  Serial.println("White Rook");
}
else if(sensor2>340 and sensor2<=370) {
  Serial.println("White Queen");
}
else if(sensor2>370 and sensor2<=400) {
  Serial.println("White King");
}
else {
Serial.println("No Piece");
}
}
