#include "application.h"
#include "FatFs.h"
#include "sd.h"
#include "rgb.h"

SD::SD() {

  working = true;

  // Set all variables.

  updateTime();

  dataOpen = false;
  currentHour = false;
  newHour = false;
  logOpen = false;
  idOpen = false;
  sdAttached = false;
  sessionStarted = false;
  currentID = false;
  newEntries = 0;

  // Set all initial messages.
  message = "Hello, world!";
  moduleMessage = String("SD module not setup.");
  sdMessage = String("No SD attached.");
  masterMessage = String("Master not inialized, waiting for SD card.");
  sessionMessage = String("Session not started, waiting for SD card.");
  dataMessage = String("Data not initialized.");
  idMessage = String("ID not initialized.");

  // Publish cloud function.
  Particle.function("SD_Input", &SD::SDCloudInput, this);
  Particle.variable("SD_Output", message);

  // Generate random session ID
  randomSeed(analogRead(A0));

  session = String("S");

  for(int i = 0; i < 7; i++) {

    //char c = random(97,123); // Lowercase
    char c = random(65,91); // Uppercase

    session.concat(c);

  }

  // Start the session
  updateTime();
  startTimestamp = timestamp;

  working = false;

}

// --- SD Module Control ---

int SD::setup(int _SD_CS, int redPin, int greenPin, int bluePin, int button) {
  // Setup the module

  if(work()) return false;

  rgb.setup(redPin,greenPin,bluePin,DIGITAL);
  rgb.yellow(); // Background color = Yellow (No SD attached)
  rgb.save();
  // Buttom

  // rgb.tmpBlue(500); // Temp working color.

  SD_CS = _SD_CS;
  FatSD.begin(SPI,SD_CS);
  FatSD.highSpeedClock(50000000);
  digitalWrite(SD_CS,LOW); // No card attached yet.

  return success();

} // The setup

int SD::update() {

  rgb.display();

  return true;
} // Update User Interface (RGB & Button)

// --- SD ---

int SD::attach() {

  if(work()) return false;

  if(sdAttached) {
    sdMessage = "SD already attached.";
    return fail();
  }

  digitalWrite(SD_CS,HIGH);

  SDresult = FatFs::attach(FatSD, 0);

  if(error(SDresult, &sdMessage)) {
    digitalWrite(SD_CS,LOW);
    return fail();
  }

  sdMessage = "SD Attached!";
  sdAttached = true;

  if(openLogFile()) writeLog("Attached SD card.");

  rgb.cyan(); // Set cyan as an indication of attached SD card.
  rgb.save();

  return success();
} // Attempt to attach the SD card

int SD::eject() {

  if(work()) return false;

  if(!sdAttached) {
    sdMessage = "No SD attached to eject.";
    return fail();
  }

  if(dataOpen) closeDataFile();

  if(error(FILEresult,&dataMessage)) return fail();

  if(logOpen) {
    writeLog("Ejecting SD.");
    closeLogFile();
  }

  if(error(FILEresult,&sessionMessage)) return fail();

  FatFs::detach(0);

  digitalWrite(SD_CS, LOW);

  rgb.yellow();
  rgb.save();

  sdMessage = "SD Ejected!";
  sdAttached = false;

  return success();
} // Attempt to eject attached SD card

int SD::forceeject() {

  if(dataOpen) closeDataFile();

  if(error(FILEresult, &dataMessage));

  if(logOpen) {
    writeLog("Ejecting SD with the Force.");
    closeLogFile();
  }

  if(error(FILEresult, &sessionMessage));

  FatFs::detach(0);

  digitalWrite(SD_CS, LOW);

  rgb.yellow();
  rgb.save();

  sdMessage = "SD Forced to eject!";
  sdAttached = false;

  return success();
} // Force eject

// --- Public Data Manipulation ---

String SD::getData(unsigned int dataID) {

} // Get a piece of data
String SD::deleteData(unsigned int dataID) {

} // Delete selected data
int SD::modifyData(unsigned int dataID, String data) {

} // Modify selected data
int SD::writeData(String data) {
  if(write()) return writefail();

  if(!sdAttached) {
    dataMessage = "No SD attached.";
    return writefail();
  }

  if(!dataOpen) {
    dataMessage = "Data file not open.";
    return writefail();
  }

  updateTime(); // Update time.

  updateDataFile(); // Check to update the data file.

  // Write a new piece of data.
  if(!f_puts(data, &dataFile)) {
    dataMessage = "Couldn't write to data file.";
    return writefail();
  }

  return writesuccess();
} // Write data

// --- Status ---

String SD::getSDModuleStatus() {return moduleMessage;} // Get the module status
String SD::getSDCardStatus() {return sdMessage;} // Get the SD card status
String SD::getMasterFileStatus() {return masterMessage;} // Get the Master Session File status
String SD::getLogFileStatus() {return sessionMessage;} // Get the Log file status
String SD::getIDFileStatus() {return idMessage;} // Get the ID file status
String SD::getDataFileStatus() {return dataMessage;} // Get the SD card status
String SD::getTimestamp() {return timestamp;} // Get the lastest timestamp

// --- Cloud ---

int SD::SDCloudInput(String command) {

  // Published cloud commands

  // [Command]<space>[Parameter]

  String parameter;

  int space;

  if(space = command.indexOf(" ")) {
    parameter = command.substring(space+1);
    command.remove(space);
  }

  if(command == "status") {

    if(parameter == "module") message = getSDModuleStatus();
    else if(parameter == "sd") message = getSDCardStatus();
    else if(parameter == "master") message = getMasterFileStatus();
    else if(parameter == "session") message = getLogFileStatus();
    else if(parameter == "dataid") message = getIDFileStatus();
    else if(parameter == "data") message = getDataFileStatus();
    else if(parameter == "time" || parameter == "timestamp") {
      updateTime();
      message = prettyTimestamp;
    }
    else {
      message = "Status, but didn't understand parameter";
      return false;
    }
    return true;
  }

  //else if(command == "start") return start();

  //else if(command == "stop") return stop();

  else if(command == "attach") return attach();

  else if(command == "eject") return eject();

  else if(command == "forceeject") return forceeject();

  else if(command == "getdata") {
    int id = parameter.toInt();
    if(!id) {
      message = String("Couldn't find the specified data at "+parameter+", "+id);
      return false;
    }
    message = getData(id);
    return true;
  }

  else if(command == "writedata") writeData(parameter);

  else message = "Didn't understand command.";

  return false;
} // Cloud CLI

String SD::SDCloudOutput() {
  return message;
} // Return message

// --- System ---

int SD::write() {
  if(writing || working) {
    rgb.tmpPurple(200);
    return true;
  } else {
    rgb.save();
    rgb.white();
    rgb.tmpWhite(200);
    writing = true;
    return false;
  }
} // Attempt to write to a file.

int SD::writesuccess() {
  rgb.load();
  rgb.tmpGreen(100);
  writing = false;
  return true;
} // Successfully write to file.

int SD::writefail() {
  rgb.load();
  rgb.tmpRed(100);
  writing = false;
  return false;
} // Failed to write.

int SD::work() {
  if(working || writing) {
    rgb.tmpPurple(200);
    return true;
  } else {
    rgb.save();
    rgb.blue();
    rgb.tmpBlue(500);
    working = true;
    return false;
  }
} // Test for working, allows working or not.

int SD::success() {
  rgb.load();
  rgb.tmpGreen(200);
  working = false;
  return true;
} // Succeeded!

int SD::fail() {
  rgb.load();
  rgb.tmpRed(200);
  working = false;
  return false;
} // Failed the instruction.

String SD::updateTime() {

  // Get new time

  time[0] = Time.year();
  time[1] = Time.month();
  time[2] = Time.day();
  time[3] = Time.hour();
  time[4] = Time.minute();
  time[5] = Time.second();

  timestamp = "";

  for(int i = 0; i < 6; i++) {
    if(time[i] < 10) {
      timestamp.concat('0');
      timestamp.concat(String(time[i]));
    }
    else timestamp.concat(String(time[i]));
  }

  prettyTimestamp = getPrettyTimestamp(timestamp);

  return timestamp;
} // Update timestamp and prettyTimestamp, create new data file if hour is changed.

String SD::getPrettyTimestamp(String ts) {

  String prettyts = "";

  prettyts.concat("y");
  prettyts.concat(ts.substring(0,4));

  prettyts.concat("m");
  prettyts.concat(ts.substring(4,6));

  prettyts.concat("d");
  prettyts.concat(ts.substring(6,8));

  prettyts.concat("h");
  prettyts.concat(ts.substring(8,10));

  prettyts.concat("m");
  prettyts.concat(ts.substring(10,12));

  prettyts.concat("s");
  prettyts.concat(ts.substring(12,14));

  return prettyts;
} // Return a pretty timestamp yYYYYmYMM... from YYYYMMDD...

// --- Error and trouble shooting ---

int SD::error(FRESULT result, String *output) {

  if(result != FR_OK) {

    *output = FatFs::fileResultMessage(result);

    return true;

  } else return false;
} // Check the FRESULT and return the string.

String SD::checkFilename(String filename) {

  if(filename.length() > 8) {

    String message = String(filename+" - Filename is too long.");
    writeLog(message);

    filename.remove(8); // Makes sure the filename is only 8 characters.

  }

  return filename;

}

// --- SD ---

int SD::checkSD() {

  if(sdAttached) return false;

  if(attach()) return false;

  else {
    sdMessage = "SD is not attached, and couldn't attach";
    return true;
  }

}

// --- Master Session Log ---

int SD::checkMasterfile() {
  
}

int SD::openMasterFile() {

}

int SD::closeMasterFile() {

}

int SD::writeMasterFile() {

}

// Session Log

int SD::checkLogFile() {

  if(logOpen) return false;

  else if(openLogFile()) return false;

  else {
    sessionMessage = "Log is not open and couldn't open.";
    return true;
  }

}

int SD::openLogFile() {

  if(checkSD()) return false;

  if(logOpen) {
    sessionMessage = "Log already open.";
    return false;
  }

  update();

  sessionFilename = String(session);

  sessionFilename = checkFilename(sessionFilename);

  sessionFilename = String(sessionFilename+".txt");

  FILEresult = f_open(&logFile, sessionFilename, FA_OPEN_APPEND | FA_WRITE | FA_READ);

  if(error(FILEresult, &sessionMessage)) return false;

  logOpen = true;

  writeLog("Hello!");

  return true;

}

int SD::closeLogFile() {

  if(checkSD()) return false;

  if(checkLogFile()) return false;

  writeLog("Goodbye!");

  FILEresult = f_close(&logFile);

  if(error(FILEresult, &sessionMessage)) return false;

  logOpen = false;

  return true;

}

int SD::writeLog(String input) {

  if(checkSD()) return false;

  if(checkLogFile()) return false;

  update();

  if(!f_puts(String(prettyTimestamp+" "+input+"\n"), &logFile)) {
    sessionMessage = "Error writing to log...";
    return false;
  }

  return true;

}

// Data ID

int SD::openIDFile() {

}

int SD::closeIDFile() {

}

int SD::writeIDFile() {

}

int SD::loadIDFile() {
  // Loads the saved ID file into memory
}

// --- Data File ---

String SD::getDataFilename() {

  return String("D"+timestamp.substring(3,10));

} // D+[YYY]YMMDDHH

int SD::updateDataFile() {

  currentHour = newHour;

  newHour = time[3]; // Hour

  if(newHour != currentHour) {
    openDataFile();
    dataMessage = "New hour!";
    return true;
  }

  else return false;

} // Check if there is and hour change and need for a new datafile.

int SD::openDataFile() {

  updateTime();

}

int SD::closeDataFile() {

  updateTime();

}
