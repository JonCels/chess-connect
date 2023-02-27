int clk = A0;
int cs = A3;
int delay_const = 1000;

int anodes[9] = {29, 35, 25, 41, 27, 31, 37, 39, 33};
int cathodes[9] = {34, 28, 26, 30, 24, 40, 32, 36, 38};

int arx = A1;
int atx = A2;
int brx = A4;
int btx = A5;
int crx = A6;
int ctx = A7;
int drx = A8;
int dtx = A9;
int erx = 12;
int etx = 13;
int frx = 10;
int ftx = 11;
int grx = 8;
int gtx = 9;
int hrx = 6;
int htx = 7;

int rawStates[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0}};

int adjStates[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0},
                       {0, 0, 0, 0, 0, 0, 0, 0}};

int newBoardColors[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0}};

int oldBoardColors[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0}};

char curBoardPieces[8][8] = {{'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'}};

char defBoardPieces[8][8] = {{'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
                             {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'0', '0', '0', '0', '0', '0', '0', '0'},
                             {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
                             {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}};

int readHall(int adcnum, int rx, int tx)
{
    digitalWrite(cs, HIGH);
    digitalWrite(clk, LOW);
    digitalWrite(cs, LOW);
    delayMicroseconds(2);

    int commandout = adcnum;
    commandout |= 0x18;
    commandout <<= 3;

    for (int i = 0; i < 5; i++)
    {
        if (commandout & 0x80)
        {
            digitalWrite(tx, HIGH);
        }
        else
        {
            digitalWrite(tx, LOW);
        }
        delayMicroseconds(2);

        commandout <<= 1;
        digitalWrite(clk, HIGH);
        delayMicroseconds(2);
        digitalWrite(clk, LOW);
        delayMicroseconds(2);
    }

    int adcout = 0;

    for (int i = 0; i < 12; i++)
    {
        digitalWrite(clk, HIGH);
        delayMicroseconds(2);
        digitalWrite(clk, LOW);
        delayMicroseconds(2);

        adcout <<= 1;

        if (digitalRead(rx))
            adcout |= 0x1;
    }

    digitalWrite(cs, HIGH);

    adcout >>= 1;
    return adcout;
}

void readHallRow(int row, int rx, int tx)
{
    int i;

    for (i = 0; i < 8; i++)
    {
        rawStates[row][i] = readHall(i, rx, tx);
    }
}

void adjust()
{
    adjStates[7][0] = rawStates[2][3];
    adjStates[7][1] = rawStates[2][2];
    adjStates[7][2] = rawStates[2][1];
    adjStates[7][3] = rawStates[2][0];
    adjStates[7][4] = rawStates[1][7];
    adjStates[7][5] = rawStates[1][6];
    adjStates[7][6] = rawStates[1][5];
    adjStates[7][7] = rawStates[1][4];
    adjStates[6][0] = rawStates[2][7];
    adjStates[6][1] = rawStates[2][6];
    adjStates[6][2] = rawStates[2][5];
    adjStates[6][3] = rawStates[2][4];
    adjStates[6][4] = rawStates[1][3];
    adjStates[6][5] = rawStates[1][2];
    adjStates[6][6] = rawStates[1][1];
    adjStates[6][7] = rawStates[1][0];
    adjStates[5][0] = rawStates[3][3];
    adjStates[5][1] = rawStates[3][2];
    adjStates[5][2] = rawStates[3][1];
    adjStates[5][3] = rawStates[3][0];
    adjStates[5][4] = rawStates[0][7];
    adjStates[5][5] = rawStates[0][6];
    adjStates[5][6] = rawStates[0][5];
    adjStates[5][7] = rawStates[0][4];
    adjStates[4][0] = rawStates[3][7];
    adjStates[4][1] = rawStates[3][6];
    adjStates[4][2] = rawStates[3][5];
    adjStates[4][3] = rawStates[3][4];
    adjStates[4][4] = rawStates[0][3];
    adjStates[4][5] = rawStates[0][2];
    adjStates[4][6] = rawStates[0][1];
    adjStates[4][7] = rawStates[0][0];
    adjStates[3][0] = rawStates[7][0];
    adjStates[3][1] = rawStates[7][1];
    adjStates[3][2] = rawStates[7][2];
    adjStates[3][3] = rawStates[7][3];
    adjStates[3][4] = rawStates[5][0];
    adjStates[3][5] = rawStates[5][1];
    adjStates[3][6] = rawStates[5][2];
    adjStates[3][7] = rawStates[5][3];
    adjStates[2][0] = rawStates[7][4];
    adjStates[2][1] = rawStates[7][5];
    adjStates[2][2] = rawStates[7][6];
    adjStates[2][3] = rawStates[7][7];
    adjStates[2][4] = rawStates[5][7];
    adjStates[2][5] = rawStates[5][6];
    adjStates[2][6] = rawStates[5][5];
    adjStates[2][7] = rawStates[5][4];
    adjStates[1][0] = rawStates[6][0];
    adjStates[1][1] = rawStates[6][1];
    adjStates[1][2] = rawStates[6][2];
    adjStates[1][3] = rawStates[6][3];
    adjStates[1][4] = rawStates[4][3];
    adjStates[1][5] = rawStates[4][2];
    adjStates[1][6] = rawStates[4][1];
    adjStates[1][7] = rawStates[4][0];
    adjStates[0][0] = rawStates[6][4];
    adjStates[0][1] = rawStates[6][5];
    adjStates[0][2] = rawStates[6][6];
    adjStates[0][3] = rawStates[6][7];
    adjStates[0][4] = rawStates[4][7];
    adjStates[0][5] = rawStates[4][6];
    adjStates[0][6] = rawStates[4][5];
    adjStates[0][7] = rawStates[4][4];
}

void printHall()
{
    int i, j;
    // char columns[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};

    Serial.println("\ta\tb\tc\td\te\tf\tg\th");
    for (i = 0; i < 8; i++)
    {
        Serial.print(8 - i);
        Serial.print("\t");
        for (j = 0; j < 8; j++)
        {
            Serial.print(adjStates[i][j]);
            Serial.print("\t");
        }
        Serial.print("\n");
    }
}

void printColors()
{
    int i, j;
    // char columns[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};

    Serial.println("\ta\tb\tc\td\te\tf\tg\th");
    for (i = 0; i < 8; i++)
    {
        Serial.print(8 - i);
        Serial.print("\t");
        for (j = 0; j < 8; j++)
        {
            Serial.print(newBoardColors[i][j]);
            Serial.print("\t");
        }
        Serial.print("\n");
    }
}
void printBoard()
{
    int i, j;
    // char columns[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};

    Serial.println("\ta\tb\tc\td\te\tf\tg\th");
    for (i = 0; i < 8; i++)
    {
        Serial.print(8 - i);
        Serial.print("\t");
        for (j = 0; j < 8; j++)
        {
            Serial.print(curBoardPieces[i][j]);
            Serial.print("\t");
        }
        Serial.print("\n");
    }
}

void identifyColors()
{
    int i, j;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (adjStates[i][j] < 140)
                newBoardColors[i][j] = 0;
            else if (adjStates[i][j] > 310)
                newBoardColors[i][j] = 1;
            else
                newBoardColors[i][j] = 2;
        }
    }
}

void updateColors()
{
    int i, j;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            oldBoardColors[i][j] = newBoardColors[i][j];
        }
    }
}

// 0 is when the game is over and before the game is started.
// Enters state when powered on and when game over button is pressed
// Leaves state when all whites and blacks are in correct spot and the game start is pressed

// 1 Game is active and players are thinking (all pieces are on the board)
// Enters state when game start is pressed
// Leaves state when a piece is picked up or if resign or draw is pressed

// 2 Piece is picked up, LEDs are lit up accordingly
// Keeps track of what piece was picked up and what are the possible moves
// Enters state when piece is picked up
// Leaves state when the piece is placed again
int gameState = 0;

// 0 for white, 1 for black
int turns = 1;

char whoseTurn = 'w';

char liftedPiece = ' ';

int liftedSquare[2] = {0, 0};

int gameStartValid()
{
    int valid = 1;
    int i;
    for (i = 0; i < 8; i++)
    {
        if (newBoardColors[0][i] != 0)
            valid = 0;
        if (newBoardColors[1][i] != 0)
            valid = 0;

        if (newBoardColors[6][i] != 1)
            valid = 0;
        if (newBoardColors[7][i] != 1)
            valid = 0;
    }
    return valid;
}

void assignPieces()
{
    int i, j;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            curBoardPieces[i][j] = defBoardPieces[i][j];
        }
    }
}

void sendFen()
{
    int k, l;
    String fen = "";
    for (k = 0; k < 8; k++)
    {
        int itemp = 0;
        char ctemp[1];
        for (l = 0; l < 8; l++)
        {
            if (curBoardPieces[k][l] == '0')
            {
                itemp += 1;
            }
            else
            {
                if (itemp != 0)
                {
                    itoa(itemp, ctemp, 10);
                    fen += ctemp;
                }
                fen += curBoardPieces[k][l];
                itemp = 0;
            }
        }
        if (itemp != 0)
        {
            itoa(itemp, ctemp, 10);
            fen += ctemp;
            fen += '/';
        }
        else if (k != 7)
            fen += '/';
        else
            fen += ' ';
    }

    if (turns % 2 == 0)
        fen += "w ";
    else
        fen += "b ";

    // adjust later for castling
    fen += "KQkq - 0 ";

    char t[1];
    itoa(turns, t, 10);
    fen += t;
    fen += '\r';

    int i;
    for (i = 0; i < 64; i++)
    {
        Serial1.write(fen[i]);
    }
    for (i = 64; i < 128; i++)
    {
        Serial2.write(fen[i]);
    }
}

int checkPick()
{
    int change = 0;
    int i, j;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (newBoardColors[i][j] != oldBoardColors[i][j])
            {
                liftedPiece = curBoardPieces[i][j];
                liftedSquare[0] = i;
                liftedSquare[1] = j;
                change = 1;
            }
        }
    }
    return change;
}

int checkPlace()
{
    int change = 0;
    int i, j;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (newBoardColors[i][j] != oldBoardColors[i][j])
            {
                curBoardPieces[i][j] = liftedPiece;
                curBoardPieces[liftedSquare[0]][liftedSquare[1]] = '0';
                change = 1;
            }
        }
    }
    return change;
}

void lightUp(int i, int j)
{
    digitalWrite(anodes[i], LOW);
    digitalWrite(anodes[i + 1], LOW);
    digitalWrite(cathodes[j], HIGH);
    digitalWrite(cathodes[j + 1], HIGH);
}

void turnOff()
{
    int m;
    for (m = 0; m < 9; m++)
    {
        digitalWrite(anodes[m], HIGH);
        digitalWrite(cathodes[m], LOW);
    }
}

void highlightPawnMoves(int row, int col, int activeColor)
{
    int direction;
    int opp;
    if (activeColor == 0)
    {
        direction = 1;
        opp = 1;
    }
    else
    {
        direction = -1;
        opp = 0;
    }
    Serial.println(activeColor);
    if (row + direction >= 0 && row + direction <= 7)
    {
        // Pawn can move forward by one square
        if (curBoardPieces[row + direction][col] == '0')
        {

            lightUp(row + direction, col);
        }

        // Pawn can move forward by two squares
        if (curBoardPieces[row + 2 * direction][col] == '0' && ((activeColor == 1 && row == 6) || (activeColor == 0 && row == 1)))
        {
            lightUp(row + 2 * direction, col);
        }

        // Pawn can capture enemy piece diagonally
        if (col > 0 && newBoardColors[row + direction][col - 1] == opp)
        {
            lightUp(row + direction, col - 1);
        }
        if (col < 7 && newBoardColors[row + direction][col + 1] == opp)
        {
            lightUp(row + direction, col + 1);
        }
    }
}

void highlightKnightMoves(int row, int col, int activeColor)
{
    int moves[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
    Serial.println("Hey");
    if (activeColor == 0)
        int opp = 1;
    else
        int opp = 0;
    for (int i = 0; i < 8; i++)
    {
        int newRow = row + moves[i][0];
        int newCol = col + moves[i][1];
        if (newRow >= 0 && newRow <= 7 && newCol >= 0 && newCol <= 7)
        {
            // char targetPiece = curBoardPieces[newRow][newCol];
            if (newBoardColors[newRow][newCol] != activeColor)
            {
                lightUp(newRow, newCol);
            }
        }
    }
}

void highlightBishopMoves(int row, int col, int activeColor)
{
    // Check for moves in the up-right direction
    for (int i = row + 1, j = col + 1; i < 8 && j < 8; i++, j++)
    {
        if (curBoardPieces[i][j] == '0' || newBoardColors[i][j] != activeColor)
        {
            lightUp(i, j);
        }
        else
        {
            break;
        }
    }

    // Check for moves in the up-left direction
    for (int i = row + 1, j = col - 1; i < 8 && j >= 0; i++, j--)
    {
        if (curBoardPieces[i][j] == '0' || newBoardColors[i][j] != activeColor)
        {
            lightUp(i, j);
        }
        else
        {
            break;
        }
    }

    // Check for moves in the down-right direction
    for (int i = row - 1, j = col + 1; i >= 0 && j < 8; i--, j++)
    {
        if (curBoardPieces[i][j] == '0' || newBoardColors[i][j] != activeColor)
        {
            lightUp(i, j);
        }
        else
        {
            break;
        }
    }

    // Check for moves in the down-left direction
    for (int i = row - 1, j = col - 1; i >= 0 && j >= 0; i--, j--)
    {
        if (curBoardPieces[i][j] == '0' || newBoardColors[i][j] != activeColor)
        {
            lightUp(i, j);
        }
        else
        {
            break;
        }
    }
}

void highlightRookMoves(int row, int col, int activeColor)
{
    int i, j;

    if (col + 1 < 8)
    {
        // Check moves to the right
        for (i = col + 1; i < 8; i++)
        {
            if (curBoardPieces[row][i] == '0' || newBoardColors[row][i] != activeColor)
            {
                lightUp(row, i);
            }
            else
            {
                if (newBoardColors[row][i] == activeColor)
                {
                    break;
                }
            }
        }
    }

    if (col - 1 >= 0)
    {
        // Check moves to the left
        for (i = col - 1; i >= 0; i--)
        {
            if (curBoardPieces[row][i] == '0' || newBoardColors[row][i] != activeColor)
            {
                lightUp(row, i);
            }
            else
            {
                if (newBoardColors[row][i] == activeColor)
                {
                    break;
                }
            }
        }
    }

    if (row - 1 >= 0)
    {
        // Check moves down
        for (j = row - 1; j >= 0; j--)
        {
            if (curBoardPieces[j][col] == '0' || newBoardColors[j][col] != activeColor)
            {
                lightUp(j, col);
            }
            else
            {
                if (newBoardColors[j][col] == activeColor)
                {
                    break;
                }
            }
        }
    }

    if (row + 1 < 8)
    {
        // Check moves up
        for (j = row + 1; j < 8; j++)
        {
            if (curBoardPieces[j][col] == '0' || newBoardColors[j][col] != activeColor)
            {
                lightUp(j, col);
            }
            else
            {
                if (newBoardColors[j][col] == activeColor)
                {
                    break;
                }
            }
        }
    }
}

void highlightQueenMoves(int row, int col, int activeColor)
{
    // Move up
    for (int i = row - 1; i >= 0; i--)
    {
        if (curBoardPieces[i][col] == '0' || newBoardColors[i][col] != activeColor)
        {
            lightUp(i, col);
        }
        else if (newBoardColors[i][col] == activeColor)
        {
            break;
        }
    }

    // Move down
    for (int i = row + 1; i <= 7; i++)
    {
        if (curBoardPieces[i][col] == '0' || newBoardColors[i][col] != activeColor)
        {
            lightUp(i, col);
        }
        else if (newBoardColors[i][col] == activeColor)
        {
            break;
        }
    }

    // Move left
    for (int j = col - 1; j >= 0; j--)
    {
        if (curBoardPieces[row][j] == '0' || newBoardColors[row][j] != activeColor)
        {
            lightUp(row, j);
        }
        else if (newBoardColors[row][j] == activeColor)
        {
            break;
        }
    }

    // Move right
    for (int j = col + 1; j <= 7; j++)
    {
        if (curBoardPieces[row][j] == '0' || newBoardColors[row][j] != activeColor)
        {
            lightUp(row, j);
        }
        else if (newBoardColors[row][j] == activeColor)
        {
            break;
        }
    }

    // Move down and right
    for (int i = row - 1; i >= 0; i--)
    {
        for (int j = 0; j <= 7; j++)
        {
            if (curBoardPieces[i][j] == '0' || newBoardColors[i][j] != activeColor)
            {
                lightUp(i, j);
            }
            else if (newBoardColors[i][j] == activeColor)
            {
                break;
            }
        }
    }

    // Move down and left
    for (int i = row - 1; i >= 0; i--)
    {
        for (int j = col - 1; j >= 0; j--)
        {
            if (curBoardPieces[i][j] == '0' || newBoardColors[i][j] != activeColor)
            {
                lightUp(i, j);
            }
            else if (newBoardColors[i][j] == activeColor)
            {
                break;
            }
        }
    }

    // Move up and right
    for (int i = row + 1; i <= 7; i++)
    {
        for (int j = 0; j <= 7; j++)
        {
            if (curBoardPieces[i][j] == '0' || newBoardColors[i][j] != activeColor)
            {
                lightUp(i, j);
            }
            else if (newBoardColors[i][j] == activeColor)
            {
                break;
            }
        }
    }

    // Move up and left
    for (int i = row + 1; i <= 7; i++)
    {
        for (int j = col - 1; j >= 0; j--)
        {
            if (curBoardPieces[i][j] == '0' || newBoardColors[i][j] != activeColor)
            {
                lightUp(i, j);
            }
            else if (newBoardColors[i][j] == activeColor)
            {
                break;
            }
        }
    }
}

void highlightKingMoves(int row, int col, int activeColor)
{
    // Check the squares directly adjacent to the king
    if (row > 0)
    {
        if (curBoardPieces[row - 1][col] == '0' || newBoardColors[row - 1][col] != activeColor)
        {
            lightUp(row - 1, col);
        }
        if (col > 0 && curBoardPieces[row - 1][col - 1] == '0' || newBoardColors[row - 1][col - 1] != activeColor)
        {
            lightUp(row - 1, col - 1);
        }
        if (col < 7 && curBoardPieces[row - 1][col + 1] == '0' || newBoardColors[row - 1][col + 1] != activeColor)
        {
            lightUp(row - 1, col + 1);
        }
    }
    if (row < 7)
    {
        if (curBoardPieces[row + 1][col] == '0' || newBoardColors[row + 1][col] != activeColor)
        {
            lightUp(row + 1, col);
        }
        if (col > 0 && curBoardPieces[row + 1][col - 1] == '0' || newBoardColors[row + 1][col - 1] != activeColor)
        {
            lightUp(row + 1, col - 1);
        }
        if (col < 7 && curBoardPieces[row + 1][col + 1] == '0' || newBoardColors[row + 1][col + 1] != activeColor)
        {
            lightUp(row + 1, col + 1);
        }
    }
    if (col > 0 && curBoardPieces[row][col - 1] == '0' || newBoardColors[row][col - 1] != activeColor)
    {
        lightUp(row, col - 1);
    }
    if (col < 7 && curBoardPieces[row][col + 1] == '0' || newBoardColors[row][col + 1] != activeColor)
    {
        lightUp(row, col + 1);
    }
}

void flash()
{
    int fromRow = liftedSquare[0];
    int fromCol = liftedSquare[1];

    switch (liftedPiece)
    {
    case 'p':
        highlightPawnMoves(fromRow, fromCol, 0);
        return;
    case 'P':
        highlightPawnMoves(fromRow, fromCol, 1);
        return;
    case 'n':
        highlightKnightMoves(fromRow, fromCol, 0);
        return;
    case 'N':
        highlightKnightMoves(fromRow, fromCol, 1);
        return;
    case 'r':
        highlightRookMoves(fromRow, fromCol, 0);
        return;
    case 'R':
        highlightRookMoves(fromRow, fromCol, 1);
        return;
    case 'b':
        highlightBishopMoves(fromRow, fromCol, 0);
        return;
    case 'B':
        highlightBishopMoves(fromRow, fromCol, 1);
        return;
    case 'q':
        highlightQueenMoves(fromRow, fromCol, 0);
        return;
    case 'Q':
        highlightQueenMoves(fromRow, fromCol, 1);
        return;
    case 'k':
        highlightKingMoves(fromRow, fromCol, 0);
        return;
    case 'K':
        highlightKingMoves(fromRow, fromCol, 1);
        return;
    }
}

void setup()
{
    pinMode(cs, OUTPUT);
    pinMode(arx, INPUT);
    pinMode(atx, OUTPUT);
    pinMode(brx, INPUT);
    pinMode(btx, OUTPUT);
    pinMode(crx, INPUT);
    pinMode(ctx, OUTPUT);
    pinMode(drx, INPUT);
    pinMode(dtx, OUTPUT);
    pinMode(erx, INPUT);
    pinMode(etx, OUTPUT);
    pinMode(frx, INPUT);
    pinMode(ftx, OUTPUT);
    pinMode(grx, INPUT);
    pinMode(gtx, OUTPUT);
    pinMode(hrx, INPUT);
    pinMode(htx, OUTPUT);
    pinMode(clk, OUTPUT);
    int i = 0;
    for (i = 0; i < 9; i++)
    {
        pinMode(anodes[i], OUTPUT);
        pinMode(cathodes[i], OUTPUT);
        digitalWrite(anodes[i], HIGH);
        digitalWrite(cathodes[i], LOW);
    }
    Serial.begin(9600);
    Serial1.begin(9600);
}

char gameCommand;

void loop()
{
    if (Serial1.available() > 0)
    {
        gameCommand = Serial1.read();
    }

    readHallRow(0, arx, atx);
    readHallRow(1, brx, btx);
    readHallRow(2, crx, ctx);
    readHallRow(3, drx, dtx);
    readHallRow(4, erx, etx);
    readHallRow(5, frx, ftx);
    readHallRow(6, grx, gtx);
    readHallRow(7, hrx, htx);
    adjust();

    identifyColors();

    Serial.print(gameState);
    Serial.print("\t");
    Serial.print(turns);
    Serial.print("\t");
    // Serial.println(gameCommand);
    Serial.print("\n");


    if (gameState == 0)
    {
        if (gameStartValid())
        { //&& gameCommand == 's') {
            assignPieces();
            gameState = 1;
        }
    }

    else if (gameState == 1)
    {
        // sendFen();
        if (checkPick())
        {
            gameState = 2;
        }
        else if (gameCommand == 'e')
            gameState = 0;
    }

    else if (gameState == 2)
    {
        // sendFen();
        flash();
        if (checkPlace())
        {
            if (whoseTurn == 'w')
                whoseTurn = 'b';
            else
            {
                whoseTurn = 'w';
                turns += 1;
            }
            gameState = 1;
            turnOff();
        }
        else if (gameCommand == 'e')
            gameState = 0;
    }

    // printHall();
    // Serial.print("\n");
    // printColors();
    // Serial.print("\n");
    printBoard();
    // delay(delay_const);
    Serial.print("\n");

    updateColors();
    delay(delay_const);
}