/*
 * Project project_v1
 * Description:
 * Author:
 * Date:
 */

#include "settings.h"
#include "app.h"

//SYSTEM_MODE(SEMI_AUTOMATIC);

App app;

// --- Particle Cloud ---

// Cloud variables
String appStatus = "Starting...";
String SDStatus = "Not Attached.";
String FileStatus = "n/a.";
int sensorValue = 0;


// Cloud functions
//int controlSD(String command);

void setup() {

  app.rgb.purple();

  delay(1000);

  // Cloud variables
  Particle.variable("App", appStatus);
  //Particle.variable("SD", SDStatus);
  Particle.variable("File", FileStatus);
  Particle.variable("Light", sensorValue);

  // Cloud Functions
  //Particle.function("Control SD", controlSD);

  app.setup();

  app.rgb.blue();

  delay(2000);

  app.rgb.off();

}



void loop() {

  app.update();

  sensorValue = app.sensor.read();
  appStatus = app.getStatus();
  SDStatus = app.sd.getSDStatus();
  FileStatus = app.sd.getFileStatus();

}
