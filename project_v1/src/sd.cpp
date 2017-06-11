#include "application.h"
#include "settings.h"
#include "sd.h"
#include "FatFs.h"

SD::SD() {

  FatSD.begin(SPI,SD_CS);
  FatSD.highSpeedClock(50000000);

  digitalWrite(SD_CS,LOW);

  randomSeed(analogRead(A0));

  session = String("S");

  for(int i = 0; i < 7; i++) {

    char c = random(97,123); // Lowercase
    //char c = random(65,91); // Uppercase

    session.concat(c);

  }

  update();

  startTimestamp = timestamp;

  SDstatus = String("New session: "+session+", Time: "+timestamp);

  dataOpen = false;
  logOpen = false;
  sdAttached = false;

  Particle.variable("SD", SDstatus);

}

int SD::attach() {

  if(sdAttached) {
    SDstatus = "SD already attached.";
    return false;
  }

  digitalWrite(SD_CS,HIGH);

  SDresult = FatFs::attach(FatSD, 0);

  if(error()) {
    digitalWrite(SD_CS,LOW);
    return false;
  }

  SDstatus = "SD Attached!";
  sdAttached = true;

  if(openLog()) writeLog("Attached SD card. ");

  return true;

}

int SD::eject() {

  if(!sdAttached) {
    SDstatus = "No SD attached to eject.";
    return false;
  }

  if(dataOpen) closeData();

  if(error()) return false;

  if(logOpen) {

    writeLog("Ejecting SD.");
    closeLog();

  }

  if(error()) return false;

  FatFs::detach(0);

  digitalWrite(SD_CS, LOW);

  SDstatus = "SD Ejected!";
  sdAttached = false;

  return true;

}

int SD::forceEject() {

  if(dataOpen) closeData();

  if(error());

  if(logOpen) closeLog();

  if(error());

  FatFs::detach(0);

  digitalWrite(SD_CS, LOW);

  SDstatus = "SD Ejected!";

  sdAttached = false;

  return true;

}

int SD::update() {

  int newHour = Time.hour();

  if(newHour != hour && logOpen) {
    writeLog("Hour "+String(hour)+" ended, closing data file.");
    closeData();
    writeLog("New hour"+String(newHour)+" started, creating new data file.");
    openData();
  }

  year = Time.year();
  month = Time.month();
  day = Time.day();
  hour = newHour;
  minute = Time.minute();
  second = Time.second();

  String monthStr, dayStr, hourStr;

  if(month < 10) {
    monthStr = String("0");
    monthStr.concat(month);
  } else monthStr = String(month);

  if(day < 10) {
    dayStr = String("0");
    dayStr.concat(day);
  } else dayStr = String(day);

  if(hour < 10) {
    hourStr = String("0");
    hourStr.concat(month);
  } else hourStr = String(hour);

  shortTimestamp = String(monthStr+dayStr+"h"+hourStr);

  //shortTimestamp = String(String(year-2000)+String(month)+String(day)+"h"+String(hour));

  timestamp = String("y"+String(year)+"m"+String(month)+"d"+String(day)+"h"+String(hour)+"m"+String(minute)+"s"+String(second));

}

String SD::getSDStatus() {

  return SDstatus;

}

String SD::getFileStatus() {

  return FILEstatus;

}

int SD::error() {

  if(SDresult != FR_OK || FILEresult != FR_OK) {

    SDstatus = FatFs::fileResultMessage(SDresult);
    FILEstatus = FatFs::fileResultMessage(FILEresult);

    return true;

  }

  else return false;

}

int SD::openLog() {

  if(!sdAttached) if(!attach()) return false;

  if(logOpen) {
    FILEstatus = "Log already open.";
    return false;
  }

  update();

  sessionFilename = String(session);

  sessionFilename = checkFilename(sessionFilename);

  sessionFilename = String(logFilename+".txt");

  FILEresult = f_open(&logFile, sessionFilename, FA_OPEN_APPEND | FA_WRITE | FA_READ);

  if(error(FILEresult, &sessionMessage)) {
    return false;
  }

  logOpen = true;

  return true;

}

int SD::writeLog(String message) {

  if(!logOpen) {
    FILEstatus = "Log doesn't appear to be open.";
    return false;
  }

  update();

  if(!f_puts(String(timestamp+" "+message+"\n"), &logFile)) {
    FILEstatus = "Error writing to log...";
    return false;
  }

  return true;

}

int SD::closeLog() {

  if(!logOpen) openLog();

  writeLog("Goodbye!");

  FILEresult = f_close(&logFile);

  if(error()) return false;

  logOpen = false;

  return true;

}

int SD::openData() {

  if(dataOpen) {
    writeLog("Data file already open.");
    FILEstatus = "Data file already open.";
    return false;
  }

  update();

  dataFilename = String("D"+shortTimestamp);

  // [D](dd)(mm)[h](hh)

  writeLog(String("Opening data file: "+dataFilename));

  dataFilename = checkFilename(dataFilename);

  dataFilename = String(dataFilename+".txt");

  FILEresult = f_open(&dataFile, dataFilename, FA_OPEN_APPEND | FA_WRITE | FA_READ);

  if(error()) {
    String message = String("Failed to open datafile, "+dataFilename+" "+FILEstatus);
    writeLog(message);
    FILEstatus = String(message);
    return false;
  }

  dataOpen = true;

  return true;

}

int SD::writeData(String data) {

  if(!dataOpen) openData();

  data = String(data+"\n");

  if(!f_puts(data, &dataFile)) {
    writeLog("Failed to write data.");
    return false;
  }

  return true;

}

int SD::closeData() {

  if(dataOpen) {

    FILEresult = f_close(&dataFile);

    if(error()) {
      writeLog("Failed to close data file");
      return false;
    }

    dataOpen = false;

    return true;

  } else return false;

}

String SD::checkFilename(String filename) {

  if(filename.length() > 8) {

    String message = String(filename+" - Filename is too long.");
    writeLog(message);
    FILEstatus = message;

    filename.remove(8); // Makes sure the filename is only 8 characters.

  }

  return filename;

}

int SD::control(String command) {

  if(command == "attach") return attach();

  else if (command == "eject") return eject();

  else if (command == "forceeject") return forceEject();

  else if (command == "opendatafile") return openData();

  else if (command == "status") {

    SDstatus = FatFs::fileResultMessage(SDresult);
    FILEstatus = FatFs::fileResultMessage(FILEresult);

    return 7;

  }

  return true;

}
