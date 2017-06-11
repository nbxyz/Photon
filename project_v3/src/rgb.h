#ifndef RGB_H
#define RGB_H

#include "application.h"

enum ledMode{
  DIGITAL, ANALOG
};

class Led {

  public:

    Led();
    int setup(int _pin, ledMode _mode);
    int set(int _value);
    int display();

  private:

    ledMode mode;
    int value;
    int pin;

};

class RGBled {

  public:

    RGBled();

    // Setup
    int setup(int redPin, int greenPin, int bluePin, ledMode _mode);

    // Display
    int display();

    // Set Colors
    int tmpColor(int r, int g, int b, int millis);
    int color(int r, int g, int b);

    int save();
    int load();

    // Premade Colors
    int red();
    int green();
    int blue();
    int yellow();
    int purple();
    int cyan();
    int white();
    int off();

    int tmpRed(int millis);
    int tmpGreen(int millis);
    int tmpBlue(int millis);
    int tmpYellow(int millis);
    int tmpPurple(int millis);
    int tmpCyan(int millis);
    int tmpWhite(int millis);
    int tmpOff(int millis);

  private:

    int swapColor();

    int R, G, B;
    int tmpR, tmpG, tmpB;

    ledMode mode;

    Led RLed, GLed, BLed;

    const static int size = 64;
    int buffer[size][4]; // Red, Green, Blue, timer
    int head = 0;
    int tail = 0;
    int count = 0;

    long swapTime;

};

#endif /* end of include guard: RGB_H */
