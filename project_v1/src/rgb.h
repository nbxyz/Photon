#ifndef RGB_H
#define RGB_H

#include "application.h"
#include "settings.h"

enum ledMode{
  DIGITAL, ANALOG
};

struct Color {
  int red;
  int green;
  int blue;
  String name;
};

class Led {

  public:

    Led();
    int setup(int _pin);
    int setMode(ledMode _mode);
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

    int setup(int redPin, int greenPin, int bluePin);
    int setMode(ledMode _mode);
    int setColor(Color _color);
    int setColor(int _r, int _g, int _b);
    int display();
    int red();
    int green();
    int blue();
    int purple();
    int yellow();
    int cyan();
    int off();

  private:

    Color color;
    int r, g, b;

    ledMode mode;

    Led RLed, GLed, BLed;

};

#endif /* end of include guard: RGB_H */
