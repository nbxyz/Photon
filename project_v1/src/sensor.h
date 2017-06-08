#ifndef SENSOR_H
#define SENSOR_H

#include "application.h"
#include "settings.h"

class Sensor {

public:

  Sensor();

  int setup(int pin);

  int read();

  int value;

private:

  int pin;

};

#endif /* end of include guard: SENSOR_H */
