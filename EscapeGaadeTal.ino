#include <M5StickCPlus.h>

int code = 2206; //Koden der skal intastes
int inputone = 0;
int inputtwo = 1;
int inputthree = 2;

// screen size is 135x240

/* After M5StickC is started or reset
  the program in the setUp () function will be run, and this part will only be run once.
  After M5StickCPlus is started or reset, the program in the setup() function will be executed, and this part will only be executed once. */
void setup(){
  // Initialize the M5StickCPlus object. Initialize the M5StickCPlus object
  M5.begin();

  // LCD display. LCd display
  //M5.Lcd.print("Hello World");

  M5.Lcd.setRotation(1); //Rotate the screen 90 degrees clockwise (1*90)

  M5.Lcd.setTextSize(5);
  M5.Lcd.drawChar(inputone+65, 23, 25, 2);
  M5.Lcd.drawChar(inputtwo+65, 100, 25, 2);
  M5.Lcd.drawChar(inputthree+65, 175, 25, 2);

  M5.Lcd.drawLine(10, 95, 70, 95, BLUE);
  M5.Lcd.drawLine(80, 95, 150, 95, GREEN);
  M5.Lcd.drawLine(160, 95, 230, 95, RED);
}

/* After the program in setup() runs, it runs the program in loop()
The loop() function is an infinite loop in which the program runs repeatedly
After the program in the setup() function is executed, the program in the loop() function will be executed
The loop() function is an endless loop, in which the program will continue to run repeatedly */
void loop() {
}
