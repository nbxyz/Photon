#ifndef EXPRGB_CPP
#define EXPRGB_CPP

#include "application.h"
#include "extrgb.h"

ExtRGB::ExtRGB() {

}

int ExtRGB::setup(int _redPin, int _greenPin, int _bluePin) {

  redPin = _redPin;
  greenPin = _greenPin;
  bluePin = _bluePin;

}

void ExtRGB::init() {

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

}

void ExtRGB::set(int _r, int _g, int _b) {

  r = _r;
  g = _g;
  b = _b;

}

void ExtRGB::white()  {  set(HIGH,HIGH,HIGH);   display();  }
void ExtRGB::yellow() {  set(HIGH,HIGH,LOW);    display();  }
void ExtRGB::red()    {  set(HIGH,LOW,LOW);     display();  }
void ExtRGB::green()  {  set(LOW,HIGH,LOW);     display();  }
void ExtRGB::cyan()   {  set(LOW,HIGH,HIGH);    display();  }
void ExtRGB::blue()   {  set(LOW,LOW,HIGH);     display();  }
void ExtRGB::purple() {  set(HIGH,LOW,HIGH);    display();  }


void ExtRGB::run() {

}

void ExtRGB::display() {

  digitalWrite(redPin,r);
  digitalWrite(greenPin,g);
  digitalWrite(bluePin,b);

}

#endif
