#ifndef SD_H
#define SD_H

#include "application.h"
#include "FatFs.h"
#include "rgb.h"
#include "data.h"

class SD {

public:

  SD();

  // SD Module Control

  int setup(int SD_CS, int redPin, int greenPin, int bluePin, int button); // Setup the module
  int update(); // Mainly user interface. (RGB and button)

  // SD Card Control

  int attach(); // Attempt to attach the SD card
  int eject(); // Attempt to eject attached SD card
  int forceeject(); // Force eject

  // Data Manipulation

  String getData(unsigned int id); // Get a piece of data
  int writeData(Data data); // Write a new piece of data

  // Status

  String getSDModuleStatus(); // Get the module status
  String getSDCardStatus(); // Get the SD card status
  String getSessionStatus(); // Get the session status
  String getIDStatus(); // Get the ID status
  String getDataStatus(); // Get the data status
  String getTimestamp(); // Get the timestamp

  // Cloud

  int SDCloudInput(String command); // Published cloud commands

private:

  // I/O

  RGBled rgb;
  // Button btn;
  // void control(&btn);

  // Module

  String message, moduleMessage;

  int write(); // Test for write access (not working).
  int writesuccess();
  int writefail();
  int work(); // Test for working.
  int worksuccess(); // Success.
  int workfail(); // Fail.
  int working, writing;

  // Time

  int time[6]; // year, month, day, hour, minute, second.
  String timestamp, prettyTimestamp, masterTimestamp, startTimestamp;
  // YYYYMMDDHHMMSS, YMMDDHH - Short time stamp
  String updateTime(); // Update time, return true if new data file is created, else false.
  String getPrettyTimestamp(String ts);

  // Error and trouble shooting

  int error(FRESULT result, String *output);
  FRESULT SDresult, FILEresult;
  String checkFilename(String filename);
  int troubleShootSD();

  // SD

  int checkSD();
  FatFsSD FatSD;
  int sdAttached, SD_CS;
  String sdMessage;

  // Session Log

  int checkSessionFile();
  int openSessionFile();
  int closeSessionFile();
  int writeSession(String log);
  FIL sessionFile;
  int sessionOpen, sessionStarted;
  String session, sessionMessage, sessionFilename;

  // Data ID

  int writeID(unsigned int id, String datafile);
  int loadIDFile(); // Loads the saved ID file into memory
  FIL IDFile;
  int idOpen;
  String idMessage, idFilename;

  unsigned int latestID;
  unsigned int dataIDs[256]; // Beginning ID
  String dataFilenames[256]; // Data name (could be reduced to char [8]?
  int IDcount = 0;

  // Data File

  FIL dataFile;
  String dataMessage, dataFilename;

};

#endif /* end of include guard: SD_H */
