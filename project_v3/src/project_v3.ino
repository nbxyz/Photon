/*
 * Project project_v3
 * Description:
 * Author:
 * Date:
 */

#include "sd.h"
#include "data.h"

SD sd;

int light;

long timekeeping;

Data d;

unsigned int counter;

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.

  // Test sensor
  pinMode(A0,INPUT);
  light = analogRead(A0);

  sd.setup(A2,D0,D1,D2,D3);
  // Card Select, Red, Green, Blue, Button

  Particle.variable("Light", light);

  timekeeping = millis();

  counter = 0;

}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.

  if(timekeeping > millis()) {

    timekeeping =+ 4000;

    d.id = counter;
    d.timestamp = 0;
    d.data = analogRead(A0);

    sd.writeData(d);

  }

  sd.update();

}
