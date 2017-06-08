#include "application.h"
#include "settings.h"
#include "app.h"
#include "sd.h"
#include "sensor.h"

App::App() {

  Particle.function("SDControl", &App::SDcontrol, this);

  status = String("Hello, World!");

  rgb.setup(RGBRED, RGBGREEN, RGBBLUE);

}

int App::setup() {

  sensor.setup(LIGHTSENSOR);

  //cloud.selectWiFi("nbxyzmobile");

}

String App::getStatus() {

  return status;

}

int App::display() {


}

int App::update() {

  sensor.read();

}

int App::SDcontrol(String command) {

  rgb.purple();

  delay(500);

  if(sd.control(command)) rgb.green();

  else rgb.red();

  delay(1000);

  rgb.off();

}
