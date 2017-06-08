#ifndef EXTRGB_H
#define EXTRGB_H

class ExtRGB {

public:

  ExtRGB();

  int setup(int _redPin, int _greenPin, int _bluePin);

  void init();

  void run();

  void display();

  void set(int red, int green, int blue);

  // Colors

  void white();
  void yellow();
  void red();
  void green();
  void blue();
  void purple();
  void cyan();

private:

  int redPin;
  int greenPin;
  int bluePin;

  int r;
  int b;
  int g;

};

#endif
