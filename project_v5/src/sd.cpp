#include "application.h"
#include "FatFs.h"
#include "sd.h"
#include "data.h"

// Constructer
SD::SD() {

  // Starting SD Module

  Particle.variable("sd_reading", reading); // Latest reading
  Particle.variable("sd_cli_out", cli_out); // Command line interface out
  Particle.function("sd_cli_in", &SD::cli_in, this); // Command line interface in

  cli_out = "Hello, World!"; // AI

  sd_status = "Unitialized"; // Set all to unitialized for debugging
  data_status = "Unitialized";
  sequence_status = "Unitialized";

  module_status = "Initialized";

  working = true;

  current_data_hour = false; // Set all the false or zero
  sd_attached = false;
  started = false;
  dataopen = false;
  sessionopen = false;

  sequence_begin = false;
  latest_sequence_number = 0;
  start_timestamp = 0;
  current_timestamp = 0;
  datafile_count = 0;
  data_count = 0;

  // Session

  randomSeed(analogRead(A0)); // Generate new random seed from analog read

  session = String("S"); // Start the session ID with 'S'

  for(int i = 0; i < 7; i++) { // Generate 7 random characters to the session ID
    char c = random(97,123); // Lowercase
    //char c = random(65,91); // Uppercase
    session.concat(c);
  }

  session = checkFilename(session); // Secure the filename
  session_status = session;

  // Update Time

  update_time(); // Get the time of start

  start_timestamp_string = timestamp_string;

  // Sequence Number Location

  /*

  for(int i = 0; i < 256; i++) { // Experimental, initialize datafile sequence searching

    datafile_sequence_number[i][0] = 0;
    datafile_sequence_number[i][1] = 0;
    datafile_names[i] = "";
    datacount_infile[i] = 0;

  }

  */

  data_count = 0; // This session data count
  datafile_count = 0; // Not used

  /* If there's already a data or sequence file, load these into memory

  else begin the data_sequenct_number[0][0] with 0.

  */

  // Done!

  working = false;

}

// Setup, Update and Draw
int SD::setup(int _SD_CS, int redPin, int greenPin, int bluePin, int _btn) {

  // Setup RGB

  rgb.setup(redPin,greenPin,bluePin,DIGITAL);

  rgb.yellow(); // Not started, no SD.
  rgb.save();
  rgb.tmpBlue(1000); // Blue means working

  if(!work_try()) return false;

  // Setup SD inputs and clock
  SD_CS = _SD_CS;
  FatSD.begin(SPI,SD_CS);
  FatSD.highSpeedClock(50000000);
  digitalWrite(SD_CS,LOW); // No card attached yet.

  // Succesfully setup
  rgb.tmpGreen(500);

  module_status = "Setup Done!";
  sd_status = "Initialized";

  start();

  return work_success();

}
void SD::update() {

  // Update

  // update_time();

}
void SD::draw() {

  // Draw

  rgb.display();

}

// Session start try, start and stop
int SD::start() {

  if(started) { // Test for started.
    rgb.tmpRed(200);
    module_status = "SD module not stopped.";
    return false;
  }

  if(sessionopen) {
    write_session("Starting.");
  }

  if(sd_attached) rgb.cyan(); // Test for sd attached.
  else rgb.purple();
  rgb.save();

  started = true;
  sequence_begin = false;
  data_count = 0;
  datafile_count = 0;

  return true;

}
int SD::stop() {

  if(!started) { // Test for started.
    rgb.tmpRed(200);
    module_status = "SD module not started.";
    return false;
  }

  if(dataopen) {
    file_result = f_close(&datafile);
    error(file_result, &data_status);
    dataopen = false;
  }

  if(sessionopen) {
    write_session("Stopping.");
    file_result = f_close(&sessionfile);
    error(file_result, &session_status);
    dataopen = false;
  }

  if(sd_attached) rgb.green(); // Test for sd attached.
  else rgb.yellow();
  rgb.save();

  started = false;

  return true;
}
int SD::start_try() {

  if(started) {
    rgb.tmpBlue(200);
    return true;
  }
  else {
    rgb.tmpRed(200);
    return false;
  }

}

int SD::cli_in(String command) {

  rgb.tmpBlue(200);

  // Published cloud commands

  // [Command]<space>[Parameter]

  String parameter;

  int space;

  if(space = command.indexOf(" ")) {
    parameter = command.substring(space+1);
    command.remove(space);
  }

  if(command == "status") {

    if(parameter == "module") cli_out = module_status;
    else if(parameter == "sd") cli_out = sd_status;
    else if(parameter == "session") cli_out = session_status;
    else if(parameter == "sequence") cli_out = sequence_status;
    else if(parameter == "data") cli_out = data_status;
    else if(parameter == "time" || parameter == "timestamp") {
      update_time();
      cli_out = timestamp_string;
    }
    else {
      cli_out = "Status, but didn't understand parameter";
      return false;
    }
    return true;
  }
  else if(command == "update") {
    if (parameter == "module") return module_update();
    else if(parameter == "sd") return sd_update();
    else if(parameter == "data") return data_update();
    else if(parameter == "session") return session_update();
    else if(parameter == "sequence") return sequence_update();
  }
  else if(command == "attach") return attach();
  else if(command == "eject") return eject();
  else if(command == "forceeject") {
    force_eject();
    return true;
  }
  else if(command == "start") return start();
  else if(command == "stop") return stop();
  else if(command == "getdata") {
    unsigned int id = parameter.toInt();
    if(!id) {
      cli_out = String("Couldn't find the specified data at "+parameter+", "+id);
      return false;
    }

    //message = getData(id);
    return true;

  }
  else if(command == "writesession") return write_session(parameter);
  else cli_out = "Didn't understand command.";

  return false;

} // Cloud CLI

int SD::attach() {

  if(!work_try()) return false;

  if(sd_attached) {
    sd_status = "SD already attached.";
    return work_fail();
  }

  digitalWrite(SD_CS,HIGH);

  sd_result = FatFs::attach(FatSD, 0);

  if(error(sd_result, &sd_status)) {
    digitalWrite(SD_CS,LOW);
    return work_fail();
  }

  sd_status = "SD Attached!";

  sd_attached = true;

  dataopen = false;
  sessionopen = false;

  write_session("SD Attached!");

  if(started) rgb.cyan(); // Set cyan as an indication of attached SD card.
  else rgb.green();

  rgb.save();

  return work_success();

}
int SD::eject() {

  if(!work_try()) return false; // If it's writing or working you can't eject.

  if(started) return work_fail(); // You have to stop before ejecting.

  if(!sd_attached) {
    sd_status = "No SD attached to eject.";
    return work_fail();
  }

  write_session("Ejecting SD.");

  if(dataopen) {
    file_result = f_close(&datafile);
    error(file_result, &data_status);
    dataopen = false;
  }

  if(sessionopen) {
    file_result = f_close(&sessionfile);
    error(file_result, &session_status);
    dataopen = false;
  }

  FatFs::detach(0);

  if(error(sd_result,&sd_status)) return work_fail();

  digitalWrite(SD_CS, LOW);

  sd_attached = false;

  if(started) rgb.purple();
  else rgb.yellow();
  rgb.save();

  sd_status = "SD Ejected!";

  return work_success();

}
void SD::force_eject() {

  write_session("Ejecting SD with the Force.");

  file_result = f_close(&datafile);
  error(file_result, &data_status);
  dataopen = false;

  file_result = f_close(&sessionfile);
  error(file_result, &session_status);
  sessionopen = false;

  FatFs::detach(0);

  error(sd_result,&sd_status);

  digitalWrite(SD_CS, LOW);

  sd_attached = false;

  rgb.purple();
  rgb.save();

  sd_status = "SD Ejected with the Force!";

}

int SD::error(FRESULT result, String *output) {

  if(result != FR_OK) {

    *output = FatFs::fileResultMessage(result);

    write_session(*output);

    return true;

  } else return false;

} // Check the FRESULT and return the string.

int SD::write_data(reading_structure data) {

  if(!start_try()) return false;

  if(!write_try()) return false;

  if(!sd_attached) {
    writing = false;
    if(!attach()) {
      rgb.purple();
      rgb.save();
      data_status = "No SD attached.";
      return write_fail();
    }
    writing = true;
  }

  if(!sequence_begin) sequence_begin = data.sequence_number;

  count_String = String(data.count);
  sequence_number_String = String(data.sequence_number);
  now_String = String(data.timestamp,3); // 3 decimals
  is_send_String = String(data.is_send);

  reading = String("Data: "+sequence_number_String+","+now_String+","+is_send_String+","+count_String+"\n");

  // Write to SD card

  update_time();

  if(current_data_hour != current_time[3]) { // New hour, new datafile

    datafilename = String("D"+timestamp_string.substring(3,10));
    datafilename = checkFilename(datafilename);
    current_data_hour = current_time[3];
    //new_datafile(latest_sequence_number, data.sequence_number); // Not tested yet.
    dataopen = false;
    data_count = 0;
    datafile_count++;

  }

  latest_sequence_number = data.sequence_number;

  // Open datafile
  if(!dataopen) file_result = f_open(&datafile, datafilename+".txt", FA_OPEN_APPEND | FA_WRITE | FA_READ);

  // Check for errors
  if(error(file_result, &data_status)) return write_fail();

  // Write a new piece of data.
  if(!f_puts(reading, &datafile)) {
    data_status = "Couldn't write to data file.";
    return write_fail();
  }

  // Close datafile
  file_result = f_sync(&datafile);

  // Check for errors
  if(error(file_result, &data_status)) return write_fail();

  // All succesful!
  data_status = reading;

  data_count++;

  return write_success();

}
int SD::write_session(String message) {

  if(!sd_attached) {
    writing = false;
    if(!attach()) {
      if(started) rgb.cyan();
      else rgb.purple();
      rgb.save();
      data_status = "No SD attached.";
      return false;
    }
    writing = true;
  }

  // Write to SD card

  update_time();

  message = String(pretty_timestamp+" "+message+"\n");

  // Open datafile
  if(!sessionopen) file_result = f_open(&sessionfile, session+".txt", FA_OPEN_APPEND | FA_WRITE | FA_READ);

  if(error(file_result, &session_status)) return false;

  // Write a new piece of data.
  if(!f_puts(message, &sessionfile)) {
    session_status = "Couldn't write to data file.";
    return false;
  }

  // Close datafile

  file_result = f_sync(&sessionfile);

  if(error(file_result, &session_status)) return false;

  session_status = message;

  return true;
}

/* Experimental functions with keeping sequence searching, needs more work before implementation
void SD::new_datafile(unsigned long end_sequence, unsigned long begin_sequence) {

  // Not tested, experimental.

  // Finish current sequence information
  datafile_sequence_number[datafile_count][1] = end_sequence;
  datacount_infile[datafile_count] = data_count;

  // Convert all information to string
  String beginSeqStr = String(datafile_sequence_number[datafile_count][0]);
  String endSeqStr = String(datafile_sequence_number[datafile_count][1]);
  String filSeqStr = String(datafile_names[datafile_count]);
  String countStr = String(datacount_infile[datafile_count]);

  // Complete sequence information
  String completeSequence = String(beginSeqStr+","+endSeqStr+","+filSeqStr+","+countStr);

  write_session("Finishing hourly datafile: "+completeSequence);

  // Open sequence file.
  file_result = f_open(&sequencefile, "SEQUENCE.txt", FA_OPEN_APPEND | FA_WRITE | FA_READ);
  error(file_result, &sequence_status);

  // Write sequence information
  if(!f_puts(completeSequence, &sequencefile)) cli_out, sequence_status = "Couldn't write sequence information.";

  // Close sequence file
  file_result = f_close(&sequencefile);
  error(file_result, &data_status);

  // Update new sequence
  datafile_count++;
  datafile_sequence_number[datafile_count][0] = begin_sequence;
  datafile_names[datafile_count] = datafilename;

  write_session("Starting new data hour!");

  sequence_status = completeSequence;
  cli_out = completeSequence;

}
void SD::load_file_sequences() {

  // Not tested, experimental.

  file_result = f_open(&sequencefile, "SEQUENCE.txt", FA_READ);

  if(error(file_result, &sequence_status)) return;

  String seqInfo;

  char buffer[256];

  unsigned long begin;
  unsigned long end;
  String filename;
  int count;
  int comma;

  while(!f_eof(&sequencefile)) {

    f_gets(buffer, sizeof buffer, &sequencefile);

    seqInfo = String(buffer);

    // Begin sequence

    comma = seqInfo.indexOf(",");

    datafile_sequence_number[datafile_count][0] = seqInfo.substring(comma+1).toInt();

    seqInfo.remove(comma);


    // End sequence

    comma = seqInfo.indexOf(",");

    datafile_sequence_number[datafile_count][1] = seqInfo.substring(comma+1).toInt();

    seqInfo.remove(comma);

    // Filename

    comma = seqInfo.indexOf(",");

    datafile_names[datafile_count] = seqInfo.substring(comma+1);

    seqInfo.remove(comma);

    // Datacount

    datacount_infile[datafile_count] = seqInfo.substring(comma+1).toInt();

    // Line finished.

    // Move line pointer???

    datafile_count++;

  }

}
*/

String SD::checkFilename(String filename) {

  if(filename.length() > 8) {

    String file_error = String(filename+" - Filename is too long.");
    write_session(file_error);

    filename.remove(8); // Makes sure the filename is only 8 characters.

  }

  return filename;

}

// Work try, success and fail
int SD::work_try() {

  rgb.save();
  rgb.blue();
  rgb.tmpBlue(500);

  if(working || writing) {
    return false;
  } else {
    working = true;
  }
  return true;

}
int SD::work_success() {

  rgb.load();
  rgb.tmpGreen(500);

  working = false;
  return true;
}
int SD::work_fail() {

  rgb.load();
  rgb.tmpRed(500);

  working = false;
  return false;
}

// Write try, success and fail
int SD::write_try() {

  rgb.save();
  rgb.white();
  rgb.tmpWhite(500);

  if(writing || working) return false;
  else {
    writing = true;
  }
  return true;

}
int SD::write_success() {

  rgb.load();
  rgb.tmpGreen(500);
  writing = false;
  return true;

}
int SD::write_fail() {

  rgb.load();
  rgb.tmpRed(500);
  writing = false;
  return false;

}

// Timestamp
void SD::update_time() {

  // Get new time
  current_time[0] = Time.year();
  current_time[1] = Time.month();
  current_time[2] = Time.day();
  current_time[3] = Time.hour();
  current_time[4] = Time.minute();
  current_time[5] = Time.second();

  String timestamp = "";

  for(int i = 0; i < 6; i++) {
    if(current_time[i] < 10) {
      timestamp.concat('0');
      timestamp.concat(String(current_time[i]));
    }
    else timestamp.concat(String(current_time[i]));
  }

  timestamp_string = timestamp;

  pretty_timestamp = pretty_time(timestamp);

}
String SD::pretty_time(String pts) {

  String result = "";

  result.concat("y");
  result.concat(pts.substring(0,4));

  result.concat("m");
  result.concat(pts.substring(4,6));

  result.concat("d");
  result.concat(pts.substring(6,8));

  result.concat("h");
  result.concat(pts.substring(8,10));

  result.concat("m");
  result.concat(pts.substring(10,12));

  result.concat("s");
  result.concat(pts.substring(12,14));

  return result;

}

// Updates
int SD::sd_update() {

  if(sd_attached) {
    sd_status = "SD is attached.";
  } else {
    sd_status = "No SD attached.";
  }

  return true;

}
int SD::sequence_update() {

  String begin = String(sequence_begin);
  String latest = String(latest_sequence_number);
  String count = String(data_count);

  sequence_status = String("File: "+datafilename+". Count: "+count+". Begin Seq: "+begin+". Latest: "+latest );

  return true;

}
int SD::data_update() {
  data_status = String("Latest reading: "+reading);
  return true;
}
int SD::session_update() {
  session_status = session;
  return true;
}
int SD::module_update() {
  if(started) module_status = "Running.";
  else {
    module_status = "Not running.";
  }
  return true;
}
