#ifndef SD_H
#define SD_H

#include "FatFs.h"
#include "rgb.h"

//class RGB;

class SD {

public:

  SD();

  // Physical SD

  int attach();
  int eject();
  int forceEject();

  // Update

  int update();
  String getSDStatus();
  String getFileStatus();

  // Check for error

  int error();

  // File handling

  int openData();
  int openLog();

  int closeData();
  int closeLog();

  int writeLog(String message);
  int writeData(String data);

  String checkFilename(String filename);

  // Cloud Control

  int control(String command);

private:

  String SDstatus, FILEstatus;

  int year, month, day, hour, minute, second;

  FatFsSD FatSD;

  String session, timestamp, startTimestamp, shortTimestamp;

  int dataOpen, logOpen, sdAttached;
  FIL logFile, dataFile;
  String logFilename, dataFilename;

  FRESULT SDresult, FILEresult;

  //RGB rgb;

};

#endif /* end of include guard: SD_H */
