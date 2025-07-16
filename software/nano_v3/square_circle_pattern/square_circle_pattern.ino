/*
  Scrolling alphabet demo, displays characters one at a time into a scrolling box.
 */

#include <SPI.h>
#include <DMD2.h>

// Set Width to the number of displays wide you have
const int WIDTH = 2;

SoftDMD dmd(WIDTH, 1);  // DMD controls the entire display
DMD_TextBox box(dmd);  // "box" provides a text box to automatically write to/scroll the display

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
  dmd.setBrightness(255);
  dmd.begin();
}

// the loop routine runs over and over again forever:
void loop() {
  for(int i = 0; i < 100; i++) {
    dmd.drawBox(0, 0, 30, 10);
    dmd.drawCircle(50, 7, 5);
    delay(200);
    dmd.clearScreen();
    dmd.drawBox(1, 1, 29, 9);
    dmd.drawCircle(50, 7, 4);
    delay(200);
    dmd.clearScreen();
    dmd.drawBox(2, 2, 28, 8);
    dmd.drawCircle(50, 7, 3);
    delay(200);
    dmd.clearScreen();
    dmd.drawBox(3, 3, 27, 7);
    dmd.drawCircle(50, 7, 2);
    delay(200);
    dmd.clearScreen();
    dmd.drawBox(4, 4, 26, 6);
    dmd.drawCircle(50, 7, 1);
    delay(200);
    dmd.clearScreen();
    dmd.drawBox(5, 5, 25, 5);
    dmd.drawCircle(50, 7, 6);
    delay(200);
    dmd.clearScreen();
  }
}
