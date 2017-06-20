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
  reading_structure getData(unsigned int sequence_number);

  int write_session(String message);

private:

  RGBled rgb;

  int SD_CS, red, blue, green, btn;

  bool started, working, sd_attached, writing;

  int cli_in(String command);

  int start_try();

  int work_try();
  int work_success();
  int work_fail();

  int write_try();
  int write_fail();
  int write_success();

  //RGB rgb;

  String reading, cli_out;

  String count_String, sequence_number_String, now_String, is_send_String;

  void sd_update();
  void sequence_update();
  void data_update();
  void session_update();
  void module_update();

  String module_status, session_status, sd_status, data_status, sequence_status;

  unsigned long latest_sequence_number, start_timestamp, current_timestamp;

  void load_file_sequences(); // Reload the SEQUENCE file
  unsigned long datafile_sequence_number[256][2]; // Sequence_number [begin][end];
  String datafile_names[256];
  int datacount_infile[256];
  int datafile_count;
  int data_count;


  String timestamp_string, start_timestamp_string, pretty_timestamp;
  void update_time();
  String pretty_time(String ts);
  int current_data_hour;

  int current_time[6];
  // Year, Month, Day, Hour, Minute, Second

  String datafilename, session;
  FIL datafile, sessionfile, sequencefile;
  FatFsSD FatSD;
  String checkFilename(String filename);
  void new_datafile(unsigned long end_sequence, unsigned long begin_sequence);

  int error(FRESULT result, String *output);
  FRESULT sd_result, file_result;

};

#endif /* end of include guard: SD_H */
