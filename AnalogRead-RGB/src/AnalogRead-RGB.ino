/*
 * Project learning-photon-sd-v1
 * Description:
 * Author: Niklas Buhl
 * Date:
 */

#define PHOTORESISTOR A0

#define SD_CS A2
#define SD_SCK A3
#define SD_MISO A4
#define SD_MOSI A5
//#define SD_CD D6 // Card Detected, grounded

#include "extrgb.h"
#include "FatFs.h"

int light = 0;
int counter = 0;

//AquBuffer buffer;

const int btnA = D3;
const int btnB = D4;

ExtRGB rgb;

FatFsSD SD;

String SDstatus;
FRESULT SDresult;

static FIL file;

//Timer diskActivityIndicatorTimer(1, updateDiskActivityIndicator);

void setup() {

  rgb.setup(D0,D1,D2);
  rgb.init();

  // Publish Variables
  Particle.variable("Light", light); // Analog read lightmeter.
  Particle.variable("SD Status", SDstatus); // Publish SD status.

  // Setup Lightmeter
  pinMode(PHOTORESISTOR, INPUT);

  // Setup SD Card
  rgb.purple();

  delay(500);

  SD.begin(SPI,SD_CS);
  //SD.highSpeedClock(25000000);
  SD.highSpeedClock(50000000);

  SDresult = FatFs::attach(SD, 0);
  SDstatus = FatFs::fileResultMessage(SDresult);

  if(SDresult != FR_OK) {
    rgb.red(); // Error opening SD card
      delay(1000);
  } else {
    rgb.green(); // Ready!
    delay(500);

  }



  // Try write to SD card

  rgb.purple();

  delay(500);

  SDresult = f_open(&file, "sd_test.txt", FA_OPEN_APPEND | FA_WRITE | FA_READ);

  SDstatus = FatFs::fileResultMessage(SDresult);

  if(SDresult != FR_OK) {
    rgb.red(); // Error writing to file
    delay(1000);
  } else {
    rgb.green();
    delay(500);
  }

  String hello ="Hello, World!";

  f_puts(hello, &file);

  SDresult = f_close(&file);

  if(SDresult != FR_OK) {
    rgb.red(); // Error closing file
    delay(1000);
  }

  delay(500);

  rgb.blue();

  delay(2000);
}

void loop() {

  rgb.purple();

  delay(500);

  light = analogRead(A0);

  SDresult = f_open(&file, "sd_test.txt", FA_OPEN_APPEND | FA_WRITE | FA_READ);

  SDstatus = FatFs::fileResultMessage(SDresult);

  if(SDresult != FR_OK) {
    rgb.red(); // Error opening file
    delay(1000);
  }

  rgb.green();

  String lightStr = String(light, DEC);
  String counterStr = String(counter, DEC);

  String writeStr = ("Index: "+lightStr+". Light: "+lightStr+".\n");

  f_puts(writeStr, &file);

  SDresult = f_close(&file);

  if(SDresult != FR_OK) {
    rgb.red(); // Error closing file
    delay(1000);
  }

  counter++;

}
