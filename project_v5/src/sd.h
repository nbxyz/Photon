#ifndef SD_H
#define SD_H

#include "application.h"
#include "rgb.h"
#include "data.h"
#include "FatFs.h"

class SD {

public:

  SD();

  int setup(int SD_CS, int red, int green, int blue, int btn);

  void update();
  void draw();

  int start();
  int stop();

  int attach();
  int eject();
  void force_eject();

  int write_data(reading_structure data);

  //reading_structure getData(unsigned int sequence_number); // Experimental

  int write_session(String message);

private:

  // RGB
  RGBled rgb;

  // SD
  int SD_CS;
  FatFsSD FatSD;
  // A3, A4, A5

  // Flags
  bool started, working, sd_attached, writing;

  // Command line interface in
  int cli_in(String command);
  String cli_out;

  // Session started try
  int start_try();

  // Working
  int work_try();
  int work_success();
  int work_fail();

  // Writing
  int write_try();
  int write_fail();
  int write_success();

  // Reading Structure
  String reading, count_String, sequence_number_String, now_String, is_send_String;

  // Updates and status
  int sd_update();
  int sequence_update();
  int data_update();
  int session_update();
  int module_update();

  String module_status, session_status, sd_status, data_status, sequence_status;

  // Sequence and epoch timestamps
  unsigned long latest_sequence_number, sequence_begin, start_timestamp, current_timestamp;

  /* Experimetal and development

  //void load_file_sequences(); // Reload the SEQUENCE file
  //unsigned long datafile_sequence_number[256][2]; // Sequence_number [begin][end];
  //String datafile_names[256];
  //int datacount_infile[256];

  */
  int datafile_count; // Count of datafiles since session start
  int data_count; // Count of readings since session start

  // Timestamp
  String timestamp_string, start_timestamp_string, pretty_timestamp;
  void update_time();
  String pretty_time(String ts);
  int current_data_hour;

  int current_time[6];
  // Year, Month, Day, Hour, Minute, Second

  // Files and filenames
  String datafilename, session;
  bool dataopen, sessionopen;
  FIL datafile, sessionfile, sequencefile;
  String checkFilename(String filename);

  //void new_datafile(unsigned long end_sequence, unsigned long begin_sequence); // Experimental

  // File and sd error
  int error(FRESULT result, String *output);
  FRESULT sd_result, file_result;

};

#endif /* end of include guard: SD_H */
