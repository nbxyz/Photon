#include "application.h"
#include "FatFs.h"
#include "sd.h"
#include "data.h"

SD::SD() {

  // Starting SD Module

  Particle.variable("sd_reading", reading);

  Particle.variable("sd_cli_out", cli_out);

  Particle.function("sd_cli_in", &SD::cli_in, this);

  cli_out = "Hello, World!";

  sd_status = "Unitialized";
  data_status = "Unitialized";
  sequence_status = "Unitialized";

  module_status = "Initialized";

  working = true;

  current_data_hour = false;
  sd_attached = false;
  started = false;

  latest_sequence_number = 0;
  start_timestamp = 0;
  current_timestamp = 0;

  // Session

  randomSeed(analogRead(A0));

  session = String("S");

  for(int i = 0; i < 7; i++) {
    char c = random(97,123); // Lowercase
    //char c = random(65,91); // Uppercase
    session.concat(c);
  }

  session = checkFilename(session);
  session_status = session;

  // Update Time

  update_time();

  start_timestamp_string = timestamp_string;

  // Sequence Number Location

  for(int i = 0; i < 256; i++) {

    datafile_sequence_number[i][0] = 0;
    datafile_sequence_number[i][1] = 0;
    datafile_names[i] = "";
    datacount_infile[i] = 0;

  }

  data_count = 0;
  datafile_count = 0;

  /* If there's already a data or sequence file, load these into memory

  else begin the data_sequenct_number[0][0] with 0.

  */

  // Done!

  working = false;

}

int SD::setup(int _SD_CS, int redPin, int greenPin, int bluePin, int _btn) {

  // Setup RGB

  rgb.setup(redPin,greenPin,bluePin,DIGITAL);

  rgb.yellow();
  rgb.save();
  rgb.tmpBlue(1000);

  if(!work_try()) return false;

  SD_CS = _SD_CS;
  FatSD.begin(SPI,SD_CS);
  FatSD.highSpeedClock(50000000);
  digitalWrite(SD_CS,LOW); // No card attached yet.

  rgb.tmpGreen(500);
  rgb.yellow();
  rgb.save();

  module_status = "Setup Done!";
  sd_status = "Initialized";

  start();

  return work_success();

}

void SD::update() {

  // Update;

  update_time();

}

void SD::draw() {

  // Draw

  rgb.display();

}

int SD::start() {
  if(started) {
    rgb.tmpRed(200);
    cli_out = "SD module not stopped.";
    module_status = "SD module not stopped.";
    return false;
  }

  if(sd_attached) rgb.cyan();
  else rgb.purple();

  rgb.save();
  started = true;

  return true;
}
int SD::stop() {

  if(!started) {
    rgb.tmpRed(200);
    cli_out = "SD module not started.";
    module_status = "SD module not started.";
    return false;
  }

  if(sd_attached) rgb.green();
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

  cli_out, sd_status = "SD Attached!";

  sd_attached = true;

  write_session("SD Attached!");

  rgb.cyan(); // Set cyan as an indication of attached SD card.
  rgb.save();

  return work_success();

}
int SD::eject() {

  if(!work_try()) return false;

  if(!sd_attached) {
    sd_status = "No SD attached to eject.";
    return work_fail();
  }

  write_session("Ejecting SD.");

  FatFs::detach(0);

  if(error(sd_result,&sd_status)) return work_fail();

  digitalWrite(SD_CS, LOW);

  sd_attached = false;

  rgb.purple();
  rgb.save();

  cli_out, sd_status = "SD Ejected!";

  return work_success();

}
void SD::force_eject() {

  write_session("Ejecting SD with the Force.");

  FatFs::detach(0);

  error(sd_result,&sd_status);

  digitalWrite(SD_CS, LOW);

  sd_attached = false;

  rgb.purple();
  rgb.save();

  sd_status = "SD Ejected with the Force!";
  cli_out = sd_status;

}

int SD::error(FRESULT result, String *output) {

  if(result != FR_OK) {

    *output = FatFs::fileResultMessage(result);

    cli_out = *output;

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
      cli_out, data_status = "No SD attached.";
      return write_fail();
    }
    writing = true;
  }

  count_String = String(data.count);
  sequence_number_String = String(data.sequence_number);
  now_String = String(data.timestamp,3); // 3 decimals
  is_send_String = String(data.is_send);

  reading = String(sequence_number_String+","+now_String+","+is_send_String+","+count_String+"\n");

  // Write to SD card

  update_time();

  if(current_data_hour != current_time[3]) {

    datafilename = String("D"+timestamp_string.substring(3,10));
    datafilename = checkFilename(datafilename);
    current_data_hour = current_time[3];
    //new_datafile(latest_sequence_number, data.sequence_number); // Not tested yet.

    data_count = 0;

  }

  latest_sequence_number = data.sequence_number;

  // Open datafile
  file_result = f_open(&datafile, datafilename+".txt", FA_OPEN_APPEND | FA_WRITE | FA_READ);

  if(error(file_result, &data_status)) return write_fail();

  // Write a new piece of data.
  if(!f_puts(reading, &datafile)) {
    cli_out, data_status = "Couldn't write to data file.";
    return write_fail();
  }

  // Close datafile
  file_result = f_close(&datafile);

  if(error(file_result, &data_status)) return write_fail();

  data_status = reading;
  cli_out = data_status;

  data_count++;

  return write_success();

}
int SD::write_session(String message) {
  if(!start_try()) return false;

  if(!sd_attached) {
    writing = false;
    if(!attach()) {
      cli_out, data_status = "No SD attached.";
      return false;
    }
    writing = true;
  }

  // Write to SD card

  update_time();

  message = String(pretty_timestamp+" "+message+"\n");

  // Open datafile
  file_result = f_open(&sessionfile, session+".txt", FA_OPEN_APPEND | FA_WRITE | FA_READ);

  if(error(file_result, &session_status)) return false;

  // Write a new piece of data.
  if(!f_puts(message, &sessionfile)) {
    session_status = "Couldn't write to data file.";
    cli_out = session_status;
    return false;
  }

  // Close datafile
  file_result = f_close(&sessionfile);

  if(error(file_result, &session_status)) return false;

  session_status = message;
  cli_out = message;

  return true;
}

// Experimental Functions with keeping sequence searching.
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

String SD::checkFilename(String filename) {

  if(filename.length() > 8) {

    String file_error = String(filename+" - Filename is too long.");
    write_session(file_error);
    cli_out = file_error;

    filename.remove(8); // Makes sure the filename is only 8 characters.

  }

  return filename;

}

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

int SD::sd_update() {

  if(sd_attached) {
    cli_out, sd_status = "SD is attached.";
  } else {
    cli_out, sd_status = "No SD attached.";
  }

  return true;

}
int SD::sequence_update() {

  // Experimental, not completed.

  String begin = String(datafile_sequence_number[datafile_count][0]);
  String latest = String(latest_sequence_number);
  String count = String(data_count);

  cli_out, sequence_status = String("File: "+datafilename+". Count: "+count+". Begin Seq: "+begin+". Latest: "+latest );

  return true;

}
int SD::data_update() {
  cli_out, data_status = String("Latest reading: "+reading);
  return true;
}
int SD::session_update() {
  cli_out, session_status = session;
  return true;
}
int SD::module_update() {
  if(started) cli_out, module_status = "Running.";
  else {
    module_status = "Not running.";
    cli_out = module_status;
  }
  return true;
}
