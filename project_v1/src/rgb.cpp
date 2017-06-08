#include "application.h"
#include "settings.h"
#include "rgb.h"

Led::Led() {

}

int Led::setup(int _pin) {
  pin = _pin;
  pinMode(pin,OUTPUT);
  digitalWrite(pin,LOW);
}

int Led::setMode(ledMode _mode) {
  mode = _mode;
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

}

int RGBled::setup(int redPin, int greenPin, int bluePin) {
  RLed.setup(redPin);
  GLed.setup(greenPin);
  BLed.setup(bluePin);
}

int RGBled::setMode(ledMode _mode) {
  mode = _mode;
}

int RGBled::setColor(Color _color) {
  color = _color;
  r = color.red;
  g = color.green;
  b = color.blue;

  RLed.set(r);
  GLed.set(g);
  BLed.set(b);
}

int RGBled::setColor(int _r, int _g, int _b) {
  r = _r;
  g = _g;
  b = _b;

  RLed.set(r);
  GLed.set(g);
  BLed.set(b);
}

int RGBled::display() {
  RLed.display();
  GLed.display();
  BLed.display();
}

int RGBled::red() {setColor(255,0,0); display();}
int RGBled::green() {setColor(0,255,0); display();}
int RGBled::blue() {setColor(0,0,255); display();}
int RGBled::yellow() {setColor(255,255,0); display();}
int RGBled::purple() {setColor(255,0,255); display();}
int RGBled::cyan() {setColor(0,255,255); display();}
int RGBled::off() {setColor(0,0,0); display();}
