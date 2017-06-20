#include "application.h"
#include "FatFs.h"
#include "sd.h"
#include "rgb.h"
#include "data.h"

SD::SD() {

  // Set working
  working = true;

  // Set all variables.
  sdAttached = false;
  sessionOpen = false;
  idOpen = false;
  latestID = false;

  // Set all initial messages.
  message = "Hello, world!";
  moduleMessage = "SD module not setup.";
  sdMessage = "No SD attached.";
  sessionMessage = "Session started, waiting for SD card.";
  idMessage = "ID not initialized.";
  dataMessage = "Data not initialized.";

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

  // Setup RGB
  rgb.setup(redPin,greenPin,bluePin,DIGITAL);
  rgb.yellow(); // Background color = Yellow (No SD attached)
  rgb.save();

  // Setup Buttom
  //btn.setup(btnPin);

  if(work()) return false;

  SD_CS = _SD_CS;
  FatSD.begin(SPI,SD_CS);
  FatSD.highSpeedClock(50000000);
  digitalWrite(SD_CS,LOW); // No card attached yet.

  return worksuccess();

} // The setup

int SD::update() {

  rgb.display();

  //control(&btn);

  return true;

} // Update User Interface (RGB & Button)

// --- SD ---

int SD::checkSD() {

  if(sdAttached) return false;

  if(attach()) return false;

  else {
    sdMessage = "SD is not attached, and couldn't attach";
    return true;
  }

}

int SD::attach() {

  if(work()) return false;

  if(sdAttached) {
    sdMessage = "SD already attached.";
    return workfail();
  }

  digitalWrite(SD_CS,HIGH);

  SDresult = FatFs::attach(FatSD, 0);

  if(error(SDresult, &sdMessage)) {
    digitalWrite(SD_CS,LOW);
    return workfail();
  }

  worksuccess();

  sdMessage = "SD Attached!";
  sdAttached = true;

  if(openSessionFile()) writeSession("Attached SD card.");

  if(loadIDFile());

  rgb.cyan(); // Set cyan as an indication of attached SD card.
  rgb.save();

  return true;

} // Attempt to attach the SD card

int SD::eject() {

  if(work()) return false;

  if(!sdAttached) {
    sdMessage = "No SD attached to eject.";
    return workfail();
  }

  if(sessionOpen) {
    writeSession("Ejecting SD.");
    closeSessionFile();
  }

  if(error(FILEresult,&sessionMessage)) return workfail();

  FatFs::detach(0);

  digitalWrite(SD_CS, LOW);

  rgb.yellow();
  rgb.save();

  sdMessage = "SD Ejected!";
  sdAttached = false;

  return worksuccess();
} // Attempt to eject attached SD card

int SD::forceeject() {

  if(sessionOpen) {
    writeSession("Ejecting SD with the Force.");
    closeSessionFile();
  }

  if(error(FILEresult, &sessionMessage));

  FatFs::detach(0);

  digitalWrite(SD_CS, LOW);

  rgb.yellow();
  rgb.save();

  sdMessage = "SD Forced to eject!";
  sdAttached = false;

  return worksuccess();
} // Force eject

// --- Status ---

String SD::getSDModuleStatus() {return moduleMessage;} // Get the module status
String SD::getSDCardStatus() {return sdMessage;} // Get the SD card status
String SD::getSessionStatus() {return sessionMessage;} // Get the Log file status
String SD::getIDStatus() {return idMessage;} // Get the ID file status
String SD::getDataStatus() {return dataMessage;} // Get the SD card status
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
    else if(parameter == "session") message = getSessionStatus();
    else if(parameter == "dataid") message = getIDStatus();
    else if(parameter == "data") message = getDataStatus();
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

  else if(command == "attach") return attach();

  else if(command == "eject") return eject();

  else if(command == "forceeject") return forceeject();

  else if(command == "getdata") {
    unsigned int id = parameter.toInt();
    if(!id) {
      message = String("Couldn't find the specified data at "+parameter+", "+id);
      return false;
    }

    //message = getData(id);
    return true;

  }

  else if(command == "writesession") writeSession(parameter);

  else message = "Didn't understand command.";

  return false;

} // Cloud CLI

// --- System ---

int SD::write() {
  if(working) {
    rgb.tmpPurple(200);
    return false;
  }
  rgb.save();
  rgb.white();
  rgb.tmpWhite(200);
  return true;
} // Attempt to write to a file.

int SD::writesuccess() {
  rgb.load();
  rgb.tmpGreen(400);
  writing = false;
  return true;
} // Successfully write to file.

int SD::writefail() {
  rgb.load();
  rgb.tmpRed(400);
  return false;
} // Failed to write.

int SD::work() {
  if(working) {
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

int SD::worksuccess() {
  rgb.load();
  rgb.tmpGreen(200);
  working = false;
  return true;
} // Succeeded!

int SD::workfail() {
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

  return pretty_timestamp;
} // Return a pretty timestamp yYYYYmYMM... from YYYYMMDD...

// --- Error and trouble shooting ---

int SD::error(FRESULT result, String *output) {

  if(result != FR_OK) {

    *output = FatFs::fileResultMessage(result);

    if(sessionOpen) writeSession(*output);

    return true;

  } else return false;
} // Check the FRESULT and return the string.

String SD::checkFilename(String filename) {

  if(filename.length() > 8) {

    String fileError = String(filename+" - Filename is too long.");
    if(sessionOpen) writeSession(fileError);

    filename.remove(8); // Makes sure the filename is only 8 characters.

  }

  return filename;

}

// Session Log

int SD::checkSessionFile() {

  if(sessionOpen) return false;

  else if(openSessionFile()) return false;

  else {
    sessionMessage = "Log is not open and couldn't open.";
    return true;
  }

}

int SD::openSessionFile() {

  if(checkSD()) return false;

  if(sessionOpen) {
    sessionMessage = "Log already open.";
    return false;
  }

  updateTime();

  sessionFilename = String(session);
  sessionFilename = checkFilename(sessionFilename);
  sessionFilename = String(sessionFilename+".txt");

  FILEresult = f_open(&sessionFile, sessionFilename, FA_OPEN_APPEND | FA_WRITE | FA_READ);

  if(error(FILEresult, &sessionMessage)) return false;

  sessionOpen = true;

  writeSession("Hello!");

  return true;

}

int SD::closeSessionFile() {

  if(checkSD()) return false;
  if(checkSessionFile()) return false;

  writeSession("Goodbye!");

  FILEresult = f_close(&sessionFile);

  if(error(FILEresult, &sessionMessage)) return false;

  sessionOpen = false;

  return true;

}

int SD::writeSession(String input) {

  if(checkSD()) return false;
  if(checkSessionFile()) return false;

  updateTime();

  if(!f_puts(String(prettyTimestamp+" "+input+"\n"), &sessionFile)) {
    sessionMessage = "Error writing to log...";
    return false;
  }

  return true;

}

// Data ID

int SD::writeID(unsigned int id, String datafile) {

  // Open

  if(sdAttached) return false;

  FILEresult = f_open(&IDFile, "IDFILE.txt", FA_OPEN_APPEND | FA_WRITE | FA_READ);

  if(error(FILEresult, &idMessage)) return false;

  writeSession("ID file opened.");

  // Write

  if(!f_puts(String(id+","+datafile), &IDFile)) {
    sessionMessage = "Error writing to ID...";
    return false;
  }

  dataIDs[IDcount] = id;
  dataFilenames[IDcount] = datafile;

  IDcount++;

  // Close

  if(sessionOpen) writeSession("Closing ID file.");

  FILEresult = f_close(&IDFile);

  if(error(FILEresult, &idMessage)) return false;

  idOpen = false;

  return true;

}

int SD::loadIDFile() {
  ///
}

// --- Data File ---

int SD::writeData(Data data) {

  if(!write()) return writefail();

  if(!sdAttached) {
    dataMessage = "No SD attached.";
    return writefail();
  }

  updateTime();

  latestID = data.id;

  String tmpDataFilename = dataFilename;

  dataFilename = String("D"+timestamp.substring(3,10));
  dataFilename = checkFilename(dataFilename);
  dataFilename = String(dataFilename+".txt");

  // Open datafile
  FILEresult = f_open(&dataFile, dataFilename, FA_OPEN_APPEND | FA_WRITE | FA_READ);

  if(error(FILEresult, &dataMessage)) return writefail();

  String dataString = String(data.id+","+timestamp+","+data.data+"\n");

  // Write a new piece of data.
  if(!f_puts(dataString, &dataFile)) {
    dataMessage = "Couldn't write to data file.";
    return writefail();
  }

  // Write ID and DataFilename to ID file
  if(tmpDataFilename != dataFilename) {

    if(sessionOpen) writeSession("Opening new data file.");
    writeID(latestID, dataFilename);

  }

  // Close datafile
  FILEresult = f_close(&dataFile);

  if(error(FILEresult, &dataMessage)) return writefail();

  return writesuccess();

} // Write data
