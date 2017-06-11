#ifndef SD_H
#define SD_H

#include "application.h"
#include "FatFs.h"
#include "rgb.h"

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

  String getData(unsigned int dataID); // Get a piece of data
  String deleteData(unsigned int dataID); // Delete selected data
  int modifyData(unsigned int dataID, String data); // Modify selected data
  int writeData(String data); // Write a new piece of data
  unsigned int getLatestDataID();

  // Status

  String getSDModuleStatus(); // Get the module status
  String getSDCardStatus(); // Get the SD card status
  String getMasterFileStatus(); // Get the Log file status
  String getLogFileStatus(); // Get the Log file status
  String getIDFileStatus(); // Get the ID file status
  String getDataFileStatus(); // Get Data file status
  String getTimestamp(); // Get Data file status

  // Cloud

  int SDCloudInput(String command); // Published cloud commands
  String SDCloudOutput();
  String message;

private:

  // I/O

  RGBled rgb;
  // Button

  // Module

  int write(); // Test for write access.
  int writesuccess();
  int writefail();
  int work(); // Test for working.
  int success(); // Success.
  int fail(); // Fail.

  int working, writing, reading;
  int time[6]; // year, month, day, hour, minute, second;
  String timestamp, prettyTimestamp, masterTimestamp, startTimestamp;
  // YYYYMMDDHHMMSS
  // YYYYMMDDHHMMSS
  // YYYYMMDDHHMMSS
  // YMMDDHH - Short time stamp

  String moduleMessage;

  String updateTime(); // Update time, return true if new data file is created, else false.
  String getPrettyTimestamp(String ts);

  // Error and trouble shooting

  int error(FRESULT result, String *output);
  FRESULT SDresult, FILEresult;
  String checkFilename(String filename);

  // SD

  FatFsSD FatSD;
  int sdAttached, SD_CS;
  String sdMessage;

  // Master & Session Log

  int openLogFile();
  int closeLogFile();
  int writeLog(String input);
  FIL masterFile, logFile;
  int masterOpen, logOpen, sessionStarted;
  String session;
  String masterMessage, sessionMessage;

  // Data ID

  int openIDFile();
  int closeIDFile();
  int writeIDFile();
  int loadIDFile(); // Loads the saved ID file into memory
  FIL IDFile;
  int idOpen, currentID, newEntries;
  String idMessage;

  unsigned int dataID[256]; // Beginning ID
  String dataFileNames[256]; // Data name (could be reduced to char [8]?)

  // Data File

  String dataFilename();
  int openDataFile();
  int updateDataFile();
  int closeDataFile();
  FIL dataFile;
  int dataOpen, newHour, currentHour;
  String dataMessage;

};

#endif /* end of include guard: SD_H */
