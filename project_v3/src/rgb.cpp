#include "application.h"
#include "rgb.h"

Led::Led() {

}

int Led::setup(int _pin, ledMode _mode) {
  pin = _pin;
  mode = _mode;
  pinMode(pin,OUTPUT);
  digitalWrite(pin,LOW);
}

int Led::set(int _value) {
  value = _value;
}

int Led::display() {

  if(mode == ANALOG) analogWrite(pin, value);
  else if(value > 0) digitalWrite(pin, HIGH);
  else digitalWrite(pin, LOW);

}

RGBled::RGBled() {

  Particle.variable("Red", R);
  Particle.variable("Green", G);
  Particle.variable("Blue", B);

  Particle.variable("SavedRed", savedR);
  Particle.variable("SavedGreen", savedG);
  Particle.variable("SavedBlue", savedB);

  R, G, B, tmpR, tmpG, tmpB = 0;

}

int RGBled::setup(int redPin, int greenPin, int bluePin, ledMode _mode) {
  RLed.setup(redPin, _mode);
  GLed.setup(greenPin, _mode);
  BLed.setup(bluePin, _mode);

}

int RGBled::display() {

  if(millis() < swapTime) return false;

  if(head != tail || count == size) { // Not empty

    swapColor();
    return true;

  } else {

    color(R,G,B);
    return true;

  }
}

int RGBled::tmpColor(int r, int g, int b, int millis) {

  if(count >= size) return false; // Full

  buffer[head][0] = r;
  buffer[head][1] = g;
  buffer[head][2] = b;
  buffer[head][3] = millis;

  head = (head + 1) % size;
  count++;

  return true;

}

int RGBled::color(int r, int g, int b) {

  R = r;
  G = g;
  B = b;

  RLed.set(R);
  GLed.set(G);
  BLed.set(B);

  RLed.display();
  GLed.display();
  BLed.display();

  return true;

}

int RGBled::swapColor() {

  swapTime = millis() + buffer[tail][3];

  RLed.set(buffer[tail][0]);
  GLed.set(buffer[tail][1]);
  BLed.set(buffer[tail][2]);

  RLed.display();
  GLed.display();
  BLed.display();

  tail = (tail + 1) % size;
  count--;

  return true;

}

void RGBled::save() {

  savedR = R;
  savedG = G;
  savedB = B;

}

void RGBled::load() {

  R = savedR;
  G = savedG;
  B = savedB;

}

// --- Premade Colors ---

int RGBled::red()     {return color(255,   0,   0);}
int RGBled::green()   {return color(  0, 255,   0);}
int RGBled::blue()    {return color(  0,   0, 255);}
int RGBled::yellow()  {return color(255,255,0);}
int RGBled::purple()  {return color(255,0,255);}
int RGBled::cyan()    {return color(0,255,255);}
int RGBled::white()   {return color(255,255,255);}
int RGBled::off()     {return color(0,0,0);}

int RGBled::tmpRed(int millis) {return tmpColor(255,0,0,millis);}
int RGBled::tmpGreen(int millis) {return tmpColor(0,255,0,millis);}
int RGBled::tmpBlue(int millis) {return tmpColor(0,0,255,millis);}
int RGBled::tmpYellow(int millis) {return tmpColor(255,255,0,millis);}
int RGBled::tmpPurple(int millis) {return tmpColor(255,0,255,millis);}
int RGBled::tmpCyan(int millis) {return tmpColor(0,255,255,millis);}
int RGBled::tmpWhite(int millis) {return tmpColor(255,255,255,millis);}
int RGBled::tmpOff(int millis) {return tmpColor(0,0,0,millis);}
