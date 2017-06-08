#include "application.h"
#include "sensor.h"

Sensor::Sensor() {

}

int Sensor::setup(int _pin) {

  pin = _pin;

  pinMode(pin,INPUT);

}

int Sensor::read() {

  value = analogRead(pin);

  return value;

}
