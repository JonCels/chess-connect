class FEN
{

} FEN last_game_state;
FEN curr_game_state;
FEN error_code;

class LCD_screen
{

} LCD_screen error_screen;
LCD_screen mode_select_screen;
LCD_screen game_start_screen;
LCD_screen draw_select_screen;
LCD_screen game_active;

class mode
{
    beginner;
    regular;
    hard;
} mode curr_mode;

void setup()
{
    /*
     * Assign bluetooth transmit and receive pins
     * Assign mega transmit and receive pins
     * Assign LCD screen pins
     */
    mode_select();
    game_start();
}

void loop()
{
    rx_from_mega();
    if (wrongMove() == true)
    {
        LCD_display(error);
        wait;
        LCD_display(game_active);
    }
    else
    {
        tx_bluetooth();
        if (curr_mode == hard)
        {
            rx_bluetooth();
        }
    }
    draw_select();
}

void rx_from_mega()
{
    /*
     * Receive the FEN string from the arduino Mega
     * Assign the last game state to the past game state variable
     * Assign the new game state to the current game state
     */
}

void wrongMove()
{
    /*
     * Compare the past game state with the current game state for an illegal move
     * If the change in the game states required an illegal move, record the made into the error_code variable and then Return TRUE
     * Otherwise return FALSE
     */
}

void LCD_Display(LCD_screen screen)
{
    /*
     * Take the variable screen and send the appropriate signals to the LCD to display the screen variable
     */
}

void tx_bluetooth()
{
    /*
     * Send the current game state FEN to the server via bluetooth
     */
}

void rx_bluetooth()
{
    /*
     * Receive the FEN code for the best three moves recommended by the AI from the server
     */
}

void mode_select()
{
    LCD_display(mode_select_screen);
    /*
     * Display the screen and run a loop to wait for them to press a button
     */
}

void game_start()
{
    LCD_display(game_start_screen);
    /*
     * Display the start screen and wait for the players to select the start screen
     */
    LCD_display(game_active);
}

void draw_select()
{
    LCD_display(draw_select_screen);
    /*
     *  Check to see if draw button is pressed on game_active screen
     *  If pressed display the draw_select_screen and await the draw buttons of the two players
     */
    LCD_display(game_active);
}