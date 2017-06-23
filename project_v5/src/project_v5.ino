/*
 * Project project_v4
 * Description:
 * Author:
 * Date:
 */

#include "data.h"
#include "sd.h"

//SYSTEM_MODE(MANUAL);

int light;

unsigned long timer;

unsigned long sequence;

reading_structure data;

SD sd;

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.

  //WiFi.setCredentials(SSID, PASSWORD);
  //WiFi.setCredentials("nbxyzmobile", "orangepanda");

  //Particle.connect();

  pinMode(A0, INPUT);

  light = analogRead(A0);

  Particle.variable("Light", light);

  timer = millis();

  sd.setup(A2,D0,D1,D2,D3);

  sequence = 1;

}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.

  //sd.update();

  sd.draw();

  if(millis() > timer) {

    timer += 4000;

    data.count = analogRead(A0);
    data.sequence_number = sequence;
    data.timestamp = millis(); //(double)Time.now() + (double)(millis()) / 1000;
    data.is_send = false;

    sd.write_data(data);

    sequence = sequence + 1;


  }

  /*

  if(millis() > connectTimer && Particle.connected() == false) {

    connectTimer += 60000;
    Particle.connect();

  }
  */

}
