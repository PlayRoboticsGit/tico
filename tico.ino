 //****************************************************************//
// Tico - Tic-Tac-Toe playing robot
// Tico is an open source 3D printed robot designed by PlayRobotics
// Tico was designed in order to inspire kids to learn coding while teaching Tico to play Tic-Tac-Toe
// Full documentation can be found here: https://playrobotics.com/blog/tico-tic-tac-toe-arduino-robot-documentation

// Attribution: Parts of this code are based on the popular Plotclock by Joo (https://www.thingiverse.com/thing:248009)
//****************************************************************//

//Should playro draw the move made by the human, or the human will draw it himself?
#define DRAW_HUMAN_MOVE false

//If you don't have a remote control or IR receiver you can enable serial monitor instead
//When using serial monitor please choose 'No line ending' from the dropdown next to the boundrate instead of 'new line'
#define SERIAL_MONITOR_MODE false

// Include libraries
#include <Wire.h>
#include <Servo.h>
#include <IRremote.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

// Servo pins
const int LEFT_SERVO_PIN = 7;
const int RIGHT_SERVO_PIN = 6;
const int LIFT_SERO_PIN = 5;

Servo servo_lift;
Servo servo_left;
Servo servo_right;


//LCD Pins
#define TFT_CS        10
#define TFT_RST        8 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         9
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);


// Lift servo calibration

// *** If the pen is not touching the board, this is the value you should play with ***
const int Z_OFFSET = 525;  // Lower value will lift the pen higher

//*** Other servo configurations, usually you will not need to touch those
int servoLift = 1500;
const int LIFT0 = 1110 + Z_OFFSET;  // On drawing surface
const int LIFT1 = 925 + Z_OFFSET;   // Between numbers
const int LIFT2 = 735 + Z_OFFSET;   // Going towards sweeper
const int LIFT_SPEED = 1000;  // Speed of liftimg arm, lower number will increase speed.
// Side servos calibration
const int SERVO_LEFT_FACTOR = 690;
const int SERVO_RIGHT_FACTOR = 690;
// Zero-position
const int SERVO_LEFT_NULL = 1950;
const int SERVO_RIGHT_NULL = 815;
// Length of arms
const float L1 = 35;
const float L2 = 55.1;
const float L3 = 13.2;
const float L4 = 45;
// Origin points of left and right servos.
const int O1X = 24;
const int O1Y = -25;
const int O2X = 49;
const int O2Y = -25;
// Home coordinates, where the eraser is.
const volatile double ERASER_X = -11;
const volatile double ERASER_Y = 45.5;
volatile double lastX = ERASER_X;  // 75;
volatile double lastY = ERASER_Y;  // 47.5;

//We will be using an array that will hold the current state of all our game cells
// -1-> Empty cell 
// 0 -> 0
// 1 -> X

int board_values[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1};
int empty_places = 9;

int winner = -1;


void setup()
{

  Serial.begin(9600);
  //LCD Setup and clear
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
  
  //Play sound on start
  tone(4,3000,250);
  delay(250);
  tone(4,400,250);
  delay(250);
  tone(4,3000,250);
  delay(250);

  //Draw blinking eyes animation
  tft.fillCircle(30, 50, 25, ST77XX_BLUE);
  tft.fillCircle(95, 50, 25, ST77XX_BLUE);
  delay(500);
  tft.fillCircle(30, 50, 15, ST77XX_BLACK);
  tft.fillCircle(95, 50, 15, ST77XX_BLACK);  
  delay(500);
  tft.fillCircle(30, 50, 15, ST77XX_BLUE);
  //tft.fillCircle(95, 50, 15, ST77XX_BLUE);  
  delay(250);
  tft.fillCircle(30, 50, 15, ST77XX_BLACK);
  //tft.fillCircle(95, 50, 15, ST77XX_BLACK);  
  delay(250);
  //tft.fillCircle(30, 50, 25, ST77XX_BLUE);
  tft.fillCircle(95, 50, 25, ST77XX_BLUE);
  delay(250);
  //tft.fillCircle(30, 50, 15, ST77XX_BLACK);
  tft.fillCircle(95, 50, 15, ST77XX_BLACK);  
  delay(250);
  tft.fillCircle(30, 50, 15, ST77XX_BLUE);
  tft.fillCircle(95, 50, 15, ST77XX_BLUE);  
  delay(250);
  tft.fillCircle(30, 50, 15, ST77XX_BLACK);
  tft.fillCircle(95, 50, 15, ST77XX_BLACK);     
  delay(1000);
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setTextWrap(true);
  tft.setCursor(30, 90);
  tft.setTextColor(ST77XX_BLUE );
  tft.setTextSize(3);
  tft.print("I'M");
  tft.setCursor(30, 120);
  tft.print("TICO");

  //Play sound when LCD says "I'M Tico"
  tone(4,400,250);
  delay(250);
  tone(4,3000,250);
  delay(250);
  delay(500);

  //Setup IR reciver on Pin 2
  IrReceiver.begin(2);

  //This is needed because we want different random number every time Arduino is reastrted
  randomSeed(analogRead(A2));
  pinMode(4, OUTPUT);

  pinMode(A0, INPUT_PULLUP);
  

}

void loop()
{ 
  // Draw "Click Start" message
  tft.setCursor(30, 100);  
  tft.fillRect(0,90,130,100,ST77XX_BLACK);
  tft.setTextColor(ST77XX_YELLOW );
  tft.setCursor(20, 90);
  tft.print("Click");
  tft.setCursor(20, 120);
  tft.print("Start");
  tone(4,3000,250); 
  delay(250);

  //The loop will run over and over again until the game is started
  //The game can be started using a button or serial monitor if you don't have a button
  //SERIAL-MONITOR-MODE setting can be changed in the begning of this code
  if (SERIAL_MONITOR_MODE)
  {
    //Serial monitor start
     //Print main menu
    Serial.println("--==MAIN MENU==--");
    Serial.println("==S== Start Game");
    Serial.println("==E== Erase");
    Serial.println("==F== Draw Frame");
    Serial.println("==H== Go Home");

    //We just wait until there is an input
    while (Serial.available() == 0) {}

    //Get the value user entered
    int user_input = Serial.read();

    
    //Ignore the hidden line end character the monitor is adding to the received character
    if(user_input != '\n')
    {
      //React to user input
      if ((user_input!='S')&&(user_input!='E')&&(user_input!='F')&&(user_input!='H')&&(user_input!='s')&&(user_input!='e')&&(user_input!='f')&&(user_input!='h'))
        Serial.println("====WRONG INPUT====");
      else
      {
       if ((user_input=='H')||(user_input=='h'))
            goHome();
       else
       {
          if ((user_input=='E')||(user_input=='e'))
          {
            erase();
            //goHome();
          }
          else
          {
            if ((user_input=='F')||(user_input=='f'))
              drawFrame();
            else
              startGame();
          }
        }
      }
    }  
  }
  else
  {
    //Button start
    //The following loop will just continue until the button is pressed
    int button_value = analogRead(A0);
    while(button_value<500)
    {
      delay(20);    
      //If you are using a regular button it will also require a pull up resistor
      //Alternatively you can use an  "Arduino button moudle" that already has a built in resistor
      //Another option will be to use Arduin's built in pull-up resistor
      button_value = analogRead(A0); 
    }
    startGame();
  }

  //Reset game variables before next game

  //Winner can be: 0 , 1(X) or 2(tie)
  winner = -1;
  empty_places = 9;

  //This array holds all the moves
  // 1->empty 0->0 1->X
  //Going over it to make it empty
  for (int i = 0; i < 9; i++)
    board_values[i] = -1;




}

void startGame()
{
  Serial.println("====GAME IS ON====");
  tone(4,3000,250);
  
  //Clean text area
  tft.fillRect(0,90,130,100,ST77XX_BLACK);
  //Print
  tft.setCursor(20, 90);
  tft.print("GAME");
  tft.setCursor(20, 115);
  tft.print("=IS=");
  tft.setCursor(20, 140);
  tft.print("=ON= ");  

  delay(500);
  
  Serial.println("Erasing");
  erase();

  Serial.println("Drawing Frame");
  drawFrame();

  delay(1000);

  Serial.println("Tico is making the first move");
  drawMove(5);
  recordMove(5);

  //As long as the game is runing we need to repeate this loop
  while ((winner == -1) && (empty_places > 0))
  {

    Serial.println("Human, enter your move(1-9)");
    
    tft.setTextColor(ST77XX_RED);  
    //Clean text area
    tft.fillRect(0,90,130,100,ST77XX_BLACK);
    //Print
    tft.setCursor(20, 90);
    tft.print("YOUR");
    tft.setCursor(20, 115);
    tft.print("MOVE");   
    int moveTo;
    tft.fillCircle(30, 50, 15, ST77XX_RED);
    tft.fillCircle(95, 50, 15, ST77XX_BLACK);

    
    //This mode can be used to get input from user via monitor instead of remote control
    //You can change this setting at the beggining of this code

    if (SERIAL_MONITOR_MODE)
    {
        //Serial monitor mode
        //This loop will just wait until there is an input
        while (Serial.available() == 0) {}
        moveTo = Serial.readString().toInt(); //Reading the Input string and turning it to integer
    }
    else
    {
        //IR mode
        IrReceiver.enableIRIn();
        delay(100);
        int code_received = false;
        while (!code_received)
        {
          detachServos();
          if (IrReceiver.decode())
          {
            // Print a short summary of received data
            IrReceiver.printIRResultShort(&Serial);
            //Mapping of codes sent by the remote control we are using, 
            //if you are using a different remote control you will need to re-map this
            //0x4 ->1
            //0x5 ->2
            //0x6 ->3
            //0x8 ->4
            //0x9 ->5
            //0xA ->6
            //0xC ->7
            //0xD ->8
            //0xE ->9
            IrReceiver.resume(); // Enable receiving of the next value
            int message_rec = IrReceiver.decodedIRData.command;
            if ((message_rec == 0xC) || (message_rec == 0x18) || (message_rec == 0x5E)
                || (message_rec == 0x8) || (message_rec == 0x1C) || (message_rec == 0x5A)
                || (message_rec == 0x42) || (message_rec == 0x52) || (message_rec == 0x4A))
            {
              code_received = true;
              IrReceiver.disableIRIn();
    
              if (message_rec == 0xC)
                moveTo = 1;
              if (message_rec == 0x18)
                moveTo = 2;
              if (message_rec == 0x5E)
                moveTo = 3;
              if (message_rec == 0x8)
                moveTo = 4;
              if (message_rec == 0x1C)
                moveTo = 5;
              if (message_rec == 0x5A)
                moveTo = 6;
              if (message_rec == 0x42)
                moveTo = 7;
              if (message_rec == 0x52)
                moveTo = 8;
              if (message_rec == 0x4A)
                moveTo = 9;    
            }
    
          }
          delay(30);
        }
    }

    //***Regardless of the mode that was used(serial / IR)
    //we now have the user's move in moveTo variable
    
    //Check if the move value is valid
    if ((moveTo > 0) && (moveTo < 10))
    {
      //Check if this place is still empty
      if (board_values[moveTo - 1] == -1)
      {
        Serial.print("Moving to: ");
        Serial.println(moveTo);
  
        //Only draw the move made by the user if this setting is activated
        if (DRAW_HUMAN_MOVE)
          drawMove(moveTo + 10);

        //Even if Tico didn't draw the move he should document it anyway  
        recordMove(moveTo + 10);
  
        checkWinnerRow (1, 0);
        checkWinnerRow (2, 0);
        checkWinnerRow (3, 0);
  
        checkWinnerCol (1, 0);
        checkWinnerCol (2, 0);
        checkWinnerCol (3, 0);
  
        checkWinnerDiag (1, 0);
        checkWinnerDiag (2, 0);
  
        if ((winner == -1) && (empty_places > 0))
        {
  
          //Clean text area
          tft.fillRect(0,90,130,100,ST77XX_BLACK);
          tft.setTextColor(ST77XX_WHITE);  
          //Print
          tft.setCursor(20, 90);
          tft.print("TICO");
          tft.setCursor(20, 115);
          tft.print("MOVE");
          tft.fillCircle(30, 50, 15, ST77XX_BLACK);
          tft.fillCircle(95, 50, 15, ST77XX_YELLOW);
          replyMove();
  
        }
        checkWinnerRow (1, 1);
        checkWinnerRow (2, 1);
        checkWinnerRow (3, 1);
  
        checkWinnerCol (1, 1);
        checkWinnerCol (2, 1);
        checkWinnerCol (3, 1);
  
        checkWinnerDiag (1, 1);
        checkWinnerDiag (2, 1);
      }
      else
      {
        Serial.println("Already taken!!");
        //IrReceiver.disableIRIn();
        //delay(100);
  
        tft.setTextColor(ST77XX_RED);  
  
        //Clean text area
        tft.fillRect(0,90,130,100,ST77XX_BLACK);
        //Print
        tft.setCursor(20, 90);
        tft.print("PLACE");
        tft.setCursor(20, 115);
        tft.print("TAKEN");
  
        tone(4, 1000, 250);
        delay(250);  
      }
    }//End if -> Check if the move value is valid
  }//End if -> Check if this place is still empty
  goHome();
}
void checkWinnerCol (int col, int player) {
  //Row
  if ((board_values[(col - 1) * 3] == player) && (board_values[(col - 1) * 3 + 1] == player) && (board_values[(col - 1) * 3 + 2] == player))
  {
    attachServos();
    Serial.println("--== Winner COL==--");
    Serial.println(player);
    if (player==0){
        //tft.print("==YOU====WIN==");   
        tft.setTextColor(ST77XX_RED);  
        //Clean text area
        tft.fillRect(0,90,130,100,ST77XX_BLACK);
        //Print
        tft.setCursor(20, 90);
        tft.print("=YOU=");
        tft.setCursor(20, 115);
        tft.print("=WIN=");                 
    }
    else
    {
        //tft.print("==TICO===WINS==");        
        tft.setTextColor(ST77XX_BLUE);  
        //Clean text area
        tft.fillRect(0,90,130,100,ST77XX_BLACK);
        //Print
        tft.setCursor(20, 90);
        tft.print("TICO");
        tft.setCursor(20, 115);
        tft.print("WINS!");
    }
    drawTo(55 - 20 * (4 - col - 1), 10);
    //Draw
    lift(LIFT0);
    drawTo(55 - 20 * (4 - col - 1), 50);
    lift(LIFT2);

    winner = player;
  }
}
void checkWinnerRow (int row, int player) {

  //Row
  if ((board_values[row - 1] == player) && (board_values[row + 3 - 1] == player) && (board_values[row + 6 - 1] == player))
  {
    attachServos();
    Serial.println("--== Winner ROW==--");
    Serial.println(player);
    tft.setTextColor(ST77XX_RED);  
    tft.setCursor(0, 20);
    if (player==0){
        //tft.print("==YOU====WIN==");            
        tft.setTextColor(ST77XX_RED);  
        //Clean text area
        tft.fillRect(0,90,130,100,ST77XX_BLACK);
        //Print
        tft.setCursor(20, 90);
        tft.print("=YOU=");
        tft.setCursor(20, 115);
        tft.print("=WIN=");
    }
    else
    {
        //tft.print("==TICO===WINS==");        
         tft.setTextColor(ST77XX_BLUE);  
        //Clean text area
        tft.fillRect(0,90,130,100,ST77XX_BLACK);
        //Print
        tft.setCursor(20, 90);
        tft.print("TICO");
        tft.setCursor(20, 115);
        tft.print("WINS!");
    }
    drawTo(10, 43 - 14 * (row - 1));
    //Draw
    lift(LIFT0);
    drawTo(60, 43 - 14 * (row - 1));
    lift(LIFT2);

    winner = player;
  }

}

void checkWinnerDiag (int diag, int player) {
  attachServos();
  //Check which diagonal
  if (diag == 1)
  {
    if ((board_values[1 - 1] == player) && (board_values[5 - 1] == player) && (board_values[9 - 1] == player))
    {
      Serial.println("--== Winner DIAGONAL 1==--");
      Serial.println(player);
      tft.setTextColor(ST77XX_RED);  
      tft.setCursor(0, 20);
      if (player==0){
        //tft.print("==YOU====WIN==");            
        tft.setTextColor(ST77XX_RED);  
        //Clean text area
        tft.fillRect(0,90,130,100,ST77XX_BLACK);
        //Print
        tft.setCursor(20, 90);
        tft.print("=YOU=");
        tft.setCursor(20, 115);
        tft.print("=WIN=");           
      }
      else
      {
        //tft.print("==TICO===WIN==");        
         tft.setTextColor(ST77XX_BLUE);  
        //Clean text area
        tft.fillRect(0,90,130,100,ST77XX_BLACK);
        //Print
        tft.setCursor(20, 90);
        tft.print("TICO");
        tft.setCursor(20, 115);
        tft.print("WINS!");  
      }
      drawTo(60, 10);
      //Draw
      lift(LIFT0);
      drawTo(15, 45);
      lift(LIFT2);

      winner = player;
    }
  }
  else
  {
    if ((board_values[7 - 1] == player) && (board_values[5 - 1] == player) && (board_values[3 - 1] == player))
    {
      Serial.println("--== Winner DIAGONAL 2==--");
      Serial.println(player);
      tft.setTextColor(ST77XX_RED);  
      tft.setCursor(0, 20);
      if (player==0){
        //tft.print("==YOU====WIN==");            
        tft.setTextColor(ST77XX_RED);  
        //Clean text area
        tft.fillRect(0,90,130,100,ST77XX_BLACK);
        //Print
        tft.setCursor(20, 90);
        tft.print("=YOU=");
        tft.setCursor(20, 115);
        tft.print("=WIN=");          
      }
      else
      {
        //tft.print("==TICO===WIN==");        
         tft.setTextColor(ST77XX_BLUE);  
        //Clean text area
        tft.fillRect(0,90,130,100,ST77XX_BLACK);
        //Print
        tft.setCursor(20, 90);
        tft.print("TICO");
        tft.setCursor(20, 115);
        tft.print("WINS!");   
      }
      drawTo(10, 10);
      //Draw
      lift(LIFT0);
      drawTo(60, 50);
      lift(LIFT2);
      winner = player;
      // or draw the line from other side
      /*
        drawTo(60, 45);
        //Draw
        lift(LIFT0);
        drawTo(15, 10);
        lift(LIFT2);
      */
    }
  }
}




void replyMove() {
  //========= Reply move ======
  //We will generate a random number from 1 to the number of empty places
  //We will then go over the array and count the empty places we meet until we get to the needed place
  //If there are 3 empty places and the trandom number will be 2 , this means we will make a move at the second empty place we find

  int randEmptyPlace = random(empty_places) + 1;
  //Debugging
  /*
    Serial.println("============================");
    Serial.print("Empty Spaces:");
    Serial.println(empty_places);
    Serial.print("Replying to randEmptyPlace: ");
    Serial.println(randEmptyPlace);
    Serial.println("============================");
  */
  //Loop until we find an empty place
  int emptyPlacesFound = 0;

  for (int i = 0; i < 9; i++)
  {
    if (board_values[i] == -1)
    {
      //We found an empty place
      emptyPlacesFound++;
      if (emptyPlacesFound == randEmptyPlace)
      {
        drawMove(i + 1);
        recordMove(i + 1);
        Serial.print("Replying to: ");
        Serial.println(i + 1);
      }
    }
  }
}
void recordMove(int move)
{
  if ((move >= 1) && (move <= 9))
  {
    board_values[move - 1] = 1;
    empty_places--;
  }
  if ((move >= 11) && (move <= 19))
  {
    board_values[move - 11] = 0;
    empty_places--;
  }
}
void drawMove(int move)
{
  attachServos();
  switch (move) {
    case 0:
      drawFrame();
      break;
    case 1:
      drawX(15, 40);
      break;

    case 2:
      drawX(15, 25);
      break;

    case 3:
      drawX(15, 10);
      break;

    case 4:
      drawX(30, 40);
      break;

    case 5:
      drawX(30, 25);
      break;

    case 6:
      drawX(30, 15);
      break;

    case 7:
      drawX(50, 40);
      break;

    case 8:
      drawX(50, 25);
      break;

    case 9:
      drawX(50, 10);
      break;
    case 11:
      drawZero(15, 40);
      break;

    case 12:
      drawZero(15, 25);
      break;

    case 13:
      drawZero(15, 10);
      break;

    case 14:
      drawZero(30, 40);
      break;

    case 15:
      drawZero(30, 25);
      break;

    case 16:
      drawZero(30, 10);
      break;

    case 17:
      drawZero(50, 40);
      break;

    case 18:
      drawZero(50, 25);
      break;

    case 19:
      drawZero(50, 10);
      break;


    case 99:
      drawTo(5, 0);
      break;
  }
  //Get out of the way
  lift(LIFT2); 
  drawTo(10, 10);
  detachServos();
}





void erase() {
  goHome();
  attachServos();
  lift(LIFT0);  // Go down, just before doing the erase movements.
  drawTo(70, ERASER_Y);
  drawTo(5, ERASER_Y);


  drawTo(70, 34);
  drawTo(0, 34);
  drawTo(70, 34);


  drawTo(0, 26);
  drawTo(70, 20);


  drawTo(0, 20);
  drawTo(70, 5);

  drawTo(10, 15);
  drawTo(40, 30);

  drawTo(ERASER_X, ERASER_Y);
  lift(LIFT2 - 100);

  detachServos();
}

void drawX(float bx, float by) {
  bx = bx - 1;
  by = by + 1;
  //Go
  drawTo(bx, by+1);
  //Draw
  lift(LIFT0);
  drawTo(bx + 10, by + 10);
  //=====
  //Go
  lift(LIFT2);
  drawTo(bx + 10, by);
  //Draw
  lift(LIFT0);
  drawTo(bx, by + 10);
  lift(LIFT1);
}

void drawZero(float bx, float by) {
  drawTo(bx + 6, by + 3);
  lift(LIFT0);
  bogenGZS(bx + 3.5, by + 5, 5, -0.8, 6.7, 0.5);
  lift(LIFT1);
}


void lift(int lift) {
  if (servoLift >= lift) {
    while (servoLift >= lift) {
      servoLift--;
      servo_lift.writeMicroseconds(servoLift);
      delayMicroseconds(LIFT_SPEED);
    }
  }
  else {
    while (servoLift <= lift) {
      servoLift++;
      servo_lift.writeMicroseconds(servoLift);
      delayMicroseconds(LIFT_SPEED);
    }
  }
}


void bogenUZS(float bx, float by, float radius, int start, int ende, float sqee) {
  float inkr = -0.05;
  float count = 0;

  do {
    drawTo(sqee * radius * cos(start + count) + bx,
           radius * sin(start + count) + by);
    count += inkr;
  }
  while ((start + count) > ende);
}


void bogenGZS(float bx, float by, float radius, int start, int ende, float sqee) {
  float inkr = 0.05;
  float count = 0;

  do {
    drawTo(sqee * radius * cos(start + count) + bx,
           radius * sin(start + count) + by);
    count += inkr;
  }
  while ((start + count) <= ende);
}


void drawTo(double pX, double pY) {
  double dx, dy, c;
  int i;

  // dx dy of new point
  dx = pX - lastX;
  dy = pY - lastY;
  //path lenght in mm, times 4 equals 4 steps per mm
  c = floor(7 * sqrt(dx * dx + dy * dy));

  if (c < 1) c = 1;

  for (i = 0; i <= c; i++) {
    // draw line point by point
    set_XY(lastX + (i * dx / c), lastY + (i * dy / c));
  }

  lastX = pX;
  lastY = pY;

}


double return_angle(double a, double b, double c) {
  // cosine rule for angle between c and a
  return acos((a * a + c * c - b * b) / (2 * a * c));
}


void set_XY(double Tx, double Ty) {
  delay(1);
  double dx, dy, c, a1, a2, Hx, Hy;

  // calculate triangle between pen, servoLeft and arm joint
  // cartesian dx/dy
  dx = Tx - O1X;
  dy = Ty - O1Y;

  // polar lemgth (c) and angle (a1)
  c = sqrt(dx * dx + dy * dy); //
  a1 = atan2(dy, dx); //
  a2 = return_angle(L1, L2, c);
  //Serial.print("servo_left:");
  //Serial.println(empty_places);
  servo_left.writeMicroseconds(floor(((a2 + a1 - M_PI) * SERVO_LEFT_FACTOR) + SERVO_LEFT_NULL));

  // calculate joinr arm point for triangle of the right servo arm
  a2 = return_angle(L2, L1, c);
  Hx = Tx + L3 * cos((a1 - a2 + 0.621) + M_PI); //36,5°
  Hy = Ty + L3 * sin((a1 - a2 + 0.621) + M_PI);

  // calculate triangle between pen joint, servoRight and arm joint
  dx = Hx - O2X;
  dy = Hy - O2Y;

  c = sqrt(dx * dx + dy * dy);
  a1 = atan2(dy, dx);
  a2 = return_angle(L1, L4, c);
  //Serial.print("servo_right:");
  //Serial.println(floor(((a1 - a2) * SERVO_RIGHT_FACTOR) + SERVO_RIGHT_NULL));
  servo_right.writeMicroseconds(floor(((a1 - a2) * SERVO_RIGHT_FACTOR) + SERVO_RIGHT_NULL));
}


void drawFrame() {

  attachServos();
  lift(LIFT2);
  
  //===VERTICAL

  //Go
  drawTo(30, 10);
  delay(500);
  //Draw
  lift(LIFT0);
  drawTo(25, 50);
  lift(LIFT2);

  //Go
  drawTo(47, 10);
  delay(500);
  //Draw
  lift(LIFT0);
  drawTo(45, 50);
  lift(LIFT2);


  //===HORIZONTAL

  //Go
  drawTo(10, 23);

  //Draw
  lift(LIFT0);
  drawTo(60, 23);
  lift(LIFT2);


  //Go
  drawTo(10, 35);


  //Draw
  lift(LIFT0);
  drawTo(60, 35);
  lift(LIFT2);

  detachServos();
}


void goHome() {
  //initial servo location

  servo_lift.writeMicroseconds(800);
  servo_left.writeMicroseconds(1633);
  servo_right.writeMicroseconds(2289);
  servo_lift.attach(LIFT_SERO_PIN);
  servo_left.attach(LEFT_SERVO_PIN);
  servo_right.attach(RIGHT_SERVO_PIN);

  lift(LIFT2 - 100); // Lift all the way up.
  drawTo(ERASER_X, ERASER_Y);
  lift(LIFT0);
  delay(500);
  //lift(LIFT2);
  detachServos();
}

void detachServos() {
  servo_lift.detach();
  servo_left.detach();
  servo_right.detach();
}
void attachServos() {
  servo_lift.attach(LIFT_SERO_PIN);
  servo_left.attach(LEFT_SERVO_PIN);
  servo_right.attach(RIGHT_SERVO_PIN);
}

