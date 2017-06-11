/*
 * Project project_v3
 * Description:
 * Author:
 * Date:
 */

#include "sd.h"

SD sd;

int light;

long writeTime;

String timestamp = "timestamp";

unsigned int dataID;

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.

  // Test sensor
  pinMode(A0,INPUT);
  light = analogRead(A0);

  sd.setup(A2,D0,D1,D2,D3);
  // Card Select, Red, Green, Blue, Button

  Particle.variable("Light", light);

  writeTime = 0;
  dataID = 0;

}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.

  //message = "Hello, other world!";

  if(millis() > writeTime) {

    writeTime += 4000;
    light = analogRead(A0);
    String data = String(dataID+","+timestamp+","+light);
    sd.writeData(data);

    dataID++;

  }

  sd.update();

}
