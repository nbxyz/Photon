#ifndef APP_H
#define APP_H

#include "application.h"
#include "settings.h"
#include "sd.h"
#include "sensor.h"
#include "cloud.h"
#include "rgb.h"

class App {

public:

  App();

  int setup();
  int display();
  int update();

  String getStatus();

  SD sd;
  Sensor sensor;
  RGBled rgb;

  int SDcontrol(String command);

private:

  int mode;
  String status;

};

#endif /* end of include guard: APP_H */
