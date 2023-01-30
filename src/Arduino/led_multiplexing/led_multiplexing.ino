int delay_time = 2;

int row_a = 21;
int row_b = 20;
int row_c = 15;
int row_d = 14;
int row_e = 0;
int row_f = 1;
int row_g = 2;
int row_h = 3;
int row_i = 4;

int col_1 = 5;
int col_2 = 6;
int col_3 = 7;
int col_4 = 8;
int col_5 = 9;
int col_6 = 10;
int col_7 = 11;
int col_8 = 12;
int col_9 = 13;

int leds[9][9] = {
{1, 1, 1, 1, 1, 1, 1, 1, 1},
{0, 0, 0, 0, 0, 0, 0, 0, 0},
{1, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0},
{1, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0}
};

void setup() {
  pinMode(row_a, OUTPUT);
  pinMode(row_b, OUTPUT);
  pinMode(row_c, OUTPUT);
  pinMode(row_d, OUTPUT);
  pinMode(row_e, OUTPUT);
  pinMode(row_f, OUTPUT);
  pinMode(row_g, OUTPUT);
  pinMode(row_h, OUTPUT);
  pinMode(row_i, OUTPUT);

  pinMode(col_1, OUTPUT);
  pinMode(col_2, OUTPUT);
  pinMode(col_3, OUTPUT);
  pinMode(col_4, OUTPUT);
  pinMode(col_5, OUTPUT);
  pinMode(col_6, OUTPUT);
  pinMode(col_7, OUTPUT);
  pinMode(col_8, OUTPUT);
  pinMode(col_9, OUTPUT);
}

void loop() {
  // Row A
  digitalWrite(row_a, LOW);
  digitalWrite(row_b, HIGH);
  digitalWrite(row_c, HIGH);
  digitalWrite(row_d, HIGH);
  digitalWrite(row_e, HIGH);
  digitalWrite(row_f, HIGH);
  digitalWrite(row_g, HIGH);
  digitalWrite(row_h, HIGH);
  digitalWrite(row_i, HIGH);

  digitalWrite(col_1, leds[0][0]);
  digitalWrite(col_2, leds[0][1]);
  digitalWrite(col_3, leds[0][2]);
  digitalWrite(col_4, leds[0][3]);
  digitalWrite(col_5, leds[0][4]);
  digitalWrite(col_6, leds[0][5]);
  digitalWrite(col_7, leds[0][6]);
  digitalWrite(col_8, leds[0][7]);
  digitalWrite(col_9, leds[0][8]);
  
  delay(delay_time);

  // Row B
  digitalWrite(row_a, HIGH);
  digitalWrite(row_b, LOW);
  digitalWrite(row_c, HIGH);
  digitalWrite(row_d, HIGH);
  digitalWrite(row_e, HIGH);
  digitalWrite(row_f, HIGH);
  digitalWrite(row_g, HIGH);
  digitalWrite(row_h, HIGH);
  digitalWrite(row_i, HIGH);

  digitalWrite(col_1, leds[1][0]);
  digitalWrite(col_2, leds[1][1]);
  digitalWrite(col_3, leds[1][2]);
  digitalWrite(col_4, leds[1][3]);
  digitalWrite(col_5, leds[1][4]);
  digitalWrite(col_6, leds[1][5]);
  digitalWrite(col_7, leds[1][6]);
  digitalWrite(col_8, leds[1][7]);
  digitalWrite(col_9, leds[1][8]);
  
  delay(delay_time);
  
  // Row C
  digitalWrite(row_a, HIGH);
  digitalWrite(row_b, HIGH);
  digitalWrite(row_c, LOW);
  digitalWrite(row_d, HIGH);
  digitalWrite(row_e, HIGH);
  digitalWrite(row_f, HIGH);
  digitalWrite(row_g, HIGH);
  digitalWrite(row_h, HIGH);
  digitalWrite(row_i, HIGH);

  digitalWrite(col_1, leds[2][0]);
  digitalWrite(col_2, leds[2][1]);
  digitalWrite(col_3, leds[2][2]);
  digitalWrite(col_4, leds[2][3]);
  digitalWrite(col_5, leds[2][4]);
  digitalWrite(col_6, leds[2][5]);
  digitalWrite(col_7, leds[2][6]);
  digitalWrite(col_8, leds[2][7]);
  digitalWrite(col_9, leds[2][8]);

  delay(delay_time);

  // Row D
  digitalWrite(row_a, HIGH);
  digitalWrite(row_b, HIGH);
  digitalWrite(row_c, HIGH);
  digitalWrite(row_d, LOW);
  digitalWrite(row_e, HIGH);
  digitalWrite(row_f, HIGH);
  digitalWrite(row_g, HIGH);
  digitalWrite(row_h, HIGH);
  digitalWrite(row_i, HIGH);

  digitalWrite(col_1, leds[3][0]);
  digitalWrite(col_2, leds[3][1]);
  digitalWrite(col_3, leds[3][2]);
  digitalWrite(col_4, leds[3][3]);
  digitalWrite(col_5, leds[3][4]);
  digitalWrite(col_6, leds[3][5]);
  digitalWrite(col_7, leds[3][6]);
  digitalWrite(col_8, leds[3][7]);
  digitalWrite(col_9, leds[3][8]);

  delay(delay_time);

  // Row E
  digitalWrite(row_a, HIGH);
  digitalWrite(row_b, HIGH);
  digitalWrite(row_c, HIGH);
  digitalWrite(row_d, HIGH);
  digitalWrite(row_e, LOW);
  digitalWrite(row_f, HIGH);
  digitalWrite(row_g, HIGH);
  digitalWrite(row_h, HIGH);
  digitalWrite(row_i, HIGH);

  digitalWrite(col_1, leds[4][0]);
  digitalWrite(col_2, leds[4][1]);
  digitalWrite(col_3, leds[4][2]);
  digitalWrite(col_4, leds[4][3]);
  digitalWrite(col_5, leds[4][4]);
  digitalWrite(col_6, leds[4][5]);
  digitalWrite(col_7, leds[4][6]);
  digitalWrite(col_8, leds[4][7]);
  digitalWrite(col_9, leds[4][8]);

  delay(delay_time);

  // Row F
  digitalWrite(row_a, HIGH);
  digitalWrite(row_b, HIGH);
  digitalWrite(row_c, HIGH);
  digitalWrite(row_d, HIGH);
  digitalWrite(row_e, HIGH);
  digitalWrite(row_f, LOW);
  digitalWrite(row_g, HIGH);
  digitalWrite(row_h, HIGH);
  digitalWrite(row_i, HIGH);

  digitalWrite(col_1, leds[5][0]);
  digitalWrite(col_2, leds[5][1]);
  digitalWrite(col_3, leds[5][2]);
  digitalWrite(col_4, leds[5][3]);
  digitalWrite(col_5, leds[5][4]);
  digitalWrite(col_6, leds[5][5]);
  digitalWrite(col_7, leds[5][6]);
  digitalWrite(col_8, leds[5][7]);
  digitalWrite(col_9, leds[5][8]);

  delay(delay_time);

  // Row G
  digitalWrite(row_a, HIGH);
  digitalWrite(row_b, HIGH);
  digitalWrite(row_c, HIGH);
  digitalWrite(row_d, HIGH);
  digitalWrite(row_e, HIGH);
  digitalWrite(row_f, HIGH);
  digitalWrite(row_g, LOW);
  digitalWrite(row_h, HIGH);
  digitalWrite(row_i, HIGH);

  digitalWrite(col_1, leds[6][0]);
  digitalWrite(col_2, leds[6][1]);
  digitalWrite(col_3, leds[6][2]);
  digitalWrite(col_4, leds[6][3]);
  digitalWrite(col_5, leds[6][4]);
  digitalWrite(col_6, leds[6][5]);
  digitalWrite(col_7, leds[6][6]);
  digitalWrite(col_8, leds[6][7]);
  digitalWrite(col_9, leds[6][8]);
  
  delay(delay_time);

  // Row H
  digitalWrite(row_a, HIGH);
  digitalWrite(row_b, HIGH);
  digitalWrite(row_c, HIGH);
  digitalWrite(row_d, HIGH);
  digitalWrite(row_e, HIGH);
  digitalWrite(row_f, HIGH);
  digitalWrite(row_g, HIGH);
  digitalWrite(row_h, LOW);
  digitalWrite(row_i, HIGH);

  digitalWrite(col_1, leds[7][0]);
  digitalWrite(col_2, leds[7][1]);
  digitalWrite(col_3, leds[7][2]);
  digitalWrite(col_4, leds[7][3]);
  digitalWrite(col_5, leds[7][4]);
  digitalWrite(col_6, leds[7][5]);
  digitalWrite(col_7, leds[7][6]);
  digitalWrite(col_8, leds[7][7]);
  digitalWrite(col_9, leds[7][8]);

  delay(delay_time);

  // Row I
  digitalWrite(row_a, HIGH);
  digitalWrite(row_b, HIGH);
  digitalWrite(row_c, HIGH);
  digitalWrite(row_d, HIGH);
  digitalWrite(row_e, HIGH);
  digitalWrite(row_f, HIGH);
  digitalWrite(row_g, HIGH);
  digitalWrite(row_h, HIGH);
  digitalWrite(row_i, LOW);

  digitalWrite(col_1, leds[8][0]);
  digitalWrite(col_2, leds[8][1]);
  digitalWrite(col_3, leds[8][2]);
  digitalWrite(col_4, leds[8][3]);
  digitalWrite(col_5, leds[8][4]);
  digitalWrite(col_6, leds[8][5]);
  digitalWrite(col_7, leds[8][6]);
  digitalWrite(col_8, leds[8][7]);
  digitalWrite(col_9, leds[8][8]);  

  delay(delay_time);
}
