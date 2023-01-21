int delay_time = 250;

int sensor_a0 = 0;
int sensor_a1 = 0;
int sensor_a2 = 0;
int sensor_a3 = 0;
int sensor_a4 = 0;
int sensor_a5 = 0;
int sensor_a6 = 0;
int sensor_a7 = 0;

int sensor_b0 = 0;
int sensor_b1 = 0;
int sensor_b2 = 0;
int sensor_b3 = 0;
int sensor_b4 = 0;
int sensor_b5 = 0;
int sensor_b6 = 0;
int sensor_b7 = 0;

int sensor_c0 = 0;
int sensor_c1 = 0;
int sensor_c2 = 0;
int sensor_c3 = 0;
int sensor_c4 = 0;
int sensor_c5 = 0;
int sensor_c6 = 0;
int sensor_c7 = 0;

int sensor_d0 = 0;
int sensor_d1 = 0;
int sensor_d2 = 0;
int sensor_d3 = 0;
int sensor_d4 = 0;
int sensor_d5 = 0;
int sensor_d6 = 0;
int sensor_d7 = 0;

int sensor_e0 = 0;
int sensor_e1 = 0;
int sensor_e2 = 0;
int sensor_e3 = 0;
int sensor_e4 = 0;
int sensor_e5 = 0;
int sensor_e6 = 0;
int sensor_e7 = 0;

int sensor_f0 = 0;
int sensor_f1 = 0;
int sensor_f2 = 0;
int sensor_f3 = 0;
int sensor_f4 = 0;
int sensor_f5 = 0;
int sensor_f6 = 0;
int sensor_f7 = 0;

int sensor_g0 = 0;
int sensor_g1 = 0;
int sensor_g2 = 0;
int sensor_g3 = 0;
int sensor_g4 = 0;
int sensor_g5 = 0;
int sensor_g6 = 0;
int sensor_g7 = 0;

int sensor_h0 = 0;
int sensor_h1 = 0;
int sensor_h2 = 0;
int sensor_h3 = 0;
int sensor_h4 = 0;
int sensor_h5 = 0;
int sensor_h6 = 0;
int sensor_h7 = 0;

int row_a = 62; // A8
int row_b = 63; // A9
int row_c = 64; // A10
int row_d = 65; // A10
int row_d = 66; // A10
int row_d = 67; // A10
int row_d = 68; // A10
int row_d = 69; // A10

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
Serial.println(" ");
Serial.println("Welcome to ChessConnect");

pinMode(row_a, OUTPUT);
pinMode(row_b, OUTPUT);
pinMode(row_c, OUTPUT);
pinMode(row_d, OUTPUT);
pinMode(row_e, OUTPUT);
pinMode(row_f, OUTPUT);
pinMode(row_g, OUTPUT);
pinMode(row_h, OUTPUT);
}

void loop() {
 
 // Read row A
  digitalWrite(row_a, HIGH);
  digitalWrite(row_b, LOW);
  digitalWrite(row_c, LOW);
  digitalWrite(row_d, LOW);
  digitalWrite(row_e, LOW);
  digitalWrite(row_f, LOW);
  digitalWrite(row_g, LOW);
  digitalWrite(row_h, LOW);
  delay(delay_time);

  sensor_a0 = analogRead(0);
  sensor_a1 = analogRead(1);
  sensor_a2 = analogRead(2);
  sensor_a3 = analogRead(3);
  sensor_a4 = analogRead(4);
  sensor_a5 = analogRead(5);
  sensor_a6 = analogRead(6);
  sensor_a7 = analogRead(7);

  //Read row B
  digitalWrite(row_a, LOW);
  digitalWrite(row_b, HIGH);
  digitalWrite(row_c, LOW);
  digitalWrite(row_d, LOW);
  digitalWrite(row_e, LOW);
  digitalWrite(row_f, LOW);
  digitalWrite(row_g, LOW);
  digitalWrite(row_h, LOW);
  delay(delay_time);

  sensor_b0 = analogRead(0);
  // sensor_b1 = analogRead(1);
  // sensor_b2 = analogRead(2);
  // sensor_b3 = analogRead(3);
  // sensor_b4 = analogRead(4);
  // sensor_b5 = analogRead(5);
  // sensor_b6 = analogRead(6);
  // sensor_b7 = analogRead(7);

  // Read row C
  digitalWrite(row_a, LOW);
  digitalWrite(row_b, LOW);
  digitalWrite(row_c, HIGH);
  digitalWrite(row_d, LOW);
  digitalWrite(row_e, LOW);
  digitalWrite(row_f, LOW);
  digitalWrite(row_g, LOW);
  digitalWrite(row_h, LOW);
  delay(delay_time);

  sensor_c0 = analogRead(0);
  // sensor_c1 = analogRead(1);
  // sensor_c2 = analogRead(2);
  // sensor_c3 = analogRead(3);
  // sensor_c4 = analogRead(4);
  // sensor_c5 = analogRead(5);
  // sensor_c6 = analogRead(6);
  // sensor_c7 = analogRead(7);

  // Read row D
  digitalWrite(row_a, LOW);
  digitalWrite(row_b, LOW);
  digitalWrite(row_c, LOW);
  digitalWrite(row_d, HIGH);
  digitalWrite(row_e, LOW);
  digitalWrite(row_f, LOW);
  digitalWrite(row_g, LOW);
  digitalWrite(row_h, LOW);
  delay(delay_time);

  sensor_d0 = analogRead(0);
  // sensor_d1 = analogRead(1);
  // sensor_d2 = analogRead(2);
  // sensor_d3 = analogRead(3);
  // sensor_d4 = analogRead(4);
  // sensor_d5 = analogRead(5);
  // sensor_d6 = analogRead(6);
  // sensor_d7 = analogRead(7);

   // Read row E
  digitalWrite(row_a, LOW);
  digitalWrite(row_b, LOW);
  digitalWrite(row_c, LOW);
  digitalWrite(row_d, LOW);
  digitalWrite(row_e, HIGH);
  digitalWrite(row_f, LOW);
  digitalWrite(row_g, LOW);
  digitalWrite(row_h, LOW);
  delay(delay_time);

  sensor_e0 = analogRead(0);
  // sensor_e1 = analogRead(1);
  // sensor_e2 = analogRead(2);
  // sensor_e3 = analogRead(3);
  // sensor_e4 = analogRead(4);
  // sensor_e5 = analogRead(5);
  // sensor_e6 = analogRead(6);
  // sensor_e7 = analogRead(7);

   // Read row F
  digitalWrite(row_a, LOW);
  digitalWrite(row_b, LOW);
  digitalWrite(row_c, LOW);
  digitalWrite(row_d, LOW);
  digitalWrite(row_e, LOW);
  digitalWrite(row_f, HIGH);
  digitalWrite(row_g, LOW);
  digitalWrite(row_h, LOW);
  delay(delay_time);

  sensor_f0 = analogRead(0);
  // sensor_f1 = analogRead(1);
  // sensor_f2 = analogRead(2);
  // sensor_f3 = analogRead(3);
  // sensor_f4 = analogRead(4);
  // sensor_f5 = analogRead(5);
  // sensor_f6 = analogRead(6);
  // sensor_f7 = analogRead(7);

   // Read row G
  digitalWrite(row_a, LOW);
  digitalWrite(row_b, LOW);
  digitalWrite(row_c, LOW);
  digitalWrite(row_d, LOW);
  digitalWrite(row_e, LOW);
  digitalWrite(row_f, LOW);
  digitalWrite(row_g, HIGH);
  digitalWrite(row_h, LOW);
  delay(delay_time);

  sensor_g0 = analogRead(0);
  // sensor_g1 = analogRead(1);
  // sensor_g2 = analogRead(2);
  // sensor_g3 = analogRead(3);
  // sensor_g4 = analogRead(4);
  // sensor_g5 = analogRead(5);
  // sensor_g6 = analogRead(6);
  // sensor_g7 = analogRead(7);

   // Read row H
  digitalWrite(row_a, LOW);
  digitalWrite(row_b, LOW);
  digitalWrite(row_c, LOW);
  digitalWrite(row_d, LOW);
  digitalWrite(row_e, LOW);
  digitalWrite(row_f, LOW);
  digitalWrite(row_g, LOW);
  digitalWrite(row_h, HIGH);
  delay(delay_time);

  sensor_h0 = analogRead(0);
  // sensor_h1 = analogRead(1);
  // sensor_h2 = analogRead(2);
  // sensor_h3 = analogRead(3);
  // sensor_h4 = analogRead(4);
  // sensor_h5 = analogRead(5);
  // sensor_h6 = analogRead(6);
  // sensor_h7 = analogRead(7);

  // Print all sensor data
  Serial.print(sensor_a0);
  Serial.print(" ");
  Serial.print(sensor_a1);
  Serial.print(" ");
  Serial.print(sensor_a2);
  Serial.print(" ");
  Serial.print(sensor_a3);
  Serial.print(" ");
  Serial.print(sensor_a4);
  Serial.print(" ");
  Serial.print(sensor_a5);
  Serial.print(" ");
  Serial.print(sensor_a6);
  Serial.print(" ");
  Serial.print(sensor_a7);
  Serial.println(" ");

  Serial.print(sensor_b0);
  Serial.print(" ");
  Serial.print(sensor_b1);
  Serial.print(" ");
  Serial.print(sensor_b2);
  Serial.print(" ");
  Serial.print(sensor_b3);
  Serial.print(" ");
  Serial.print(sensor_b4);
  Serial.print(" ");
  Serial.print(sensor_b5);
  Serial.print(" ");
  Serial.print(sensor_b6);
  Serial.print(" ");
  Serial.print(sensor_b7);
  Serial.println(" ");

  Serial.print(sensor_c0);
  Serial.print(" ");
  Serial.print(sensor_c1);
  Serial.print(" ");
  Serial.print(sensor_c2);
  Serial.print(" ");
  Serial.print(sensor_c3);
  Serial.print(" ");
  Serial.print(sensor_c4);
  Serial.print(" ");
  Serial.print(sensor_c5);
  Serial.print(" ");
  Serial.print(sensor_c6);
  Serial.print(" ");
  Serial.print(sensor_c7);
  Serial.println(" ");

  Serial.print(sensor_d0);
  Serial.print(" ");
  Serial.print(sensor_d1);
  Serial.print(" ");
  Serial.print(sensor_d2);
  Serial.print(" ");
  Serial.print(sensor_d3);
  Serial.print(" ");
  Serial.print(sensor_d4);
  Serial.print(" ");
  Serial.print(sensor_d5);
  Serial.print(" ");
  Serial.print(sensor_d6);
  Serial.print(" ");
  Serial.print(sensor_d7);
  Serial.println(" ");

  Serial.print(sensor_e0);
  Serial.print(" ");
  Serial.print(sensor_e1);
  Serial.print(" ");
  Serial.print(sensor_e2);
  Serial.print(" ");
  Serial.print(sensor_e3);
  Serial.print(" ");
  Serial.print(sensor_e4);
  Serial.print(" ");
  Serial.print(sensor_e5);
  Serial.print(" ");
  Serial.print(sensor_e6);
  Serial.print(" ");
  Serial.print(sensor_e7);
  Serial.println(" ");

  Serial.print(sensor_f0);
  Serial.print(" ");
  Serial.print(sensor_f1);
  Serial.print(" ");
  Serial.print(sensor_f2);
  Serial.print(" ");
  Serial.print(sensor_f3);
  Serial.print(" ");
  Serial.print(sensor_f4);
  Serial.print(" ");
  Serial.print(sensor_f5);
  Serial.print(" ");
  Serial.print(sensor_f6);
  Serial.print(" ");
  Serial.print(sensor_f7);
  Serial.println(" ");

  Serial.print(sensor_g0);
  Serial.print(" ");
  Serial.print(sensor_g1);
  Serial.print(" ");
  Serial.print(sensor_g2);
  Serial.print(" ");
  Serial.print(sensor_g3);
  Serial.print(" ");
  Serial.print(sensor_g4);
  Serial.print(" ");
  Serial.print(sensor_g5);
  Serial.print(" ");
  Serial.print(sensor_g6);
  Serial.print(" ");
  Serial.print(sensor_g7);
  Serial.println(" ");

  Serial.print(sensor_h0);
  Serial.print(" ");
  Serial.print(sensor_h1);
  Serial.print(" ");
  Serial.print(sensor_h2);
  Serial.print(" ");
  Serial.print(sensor_h3);
  Serial.print(" ");
  Serial.print(sensor_h4);
  Serial.print(" ");
  Serial.print(sensor_h5);
  Serial.print(" ");
  Serial.print(sensor_h6);
  Serial.print(" ");
  Serial.print(sensor_h7);
  Serial.println(" ");

  Serial.println("-----------------------------------------------");  
}
