#ifndef AQUBUFFER_H
#define AQUBUFFER_H

#define BUFFER_SIZE 128

#include "application.h"

struct Data {

  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;

  char *type = "Light";

  int value;

  int stored = 0;
  int uploaded = 0;

};

struct BufferNode {

  Data *dataPtr;
  BufferNode *nextPtr;

};

class AquBuffer {

public:

  AquBuffer();

  int add(Data data);
  int remove();

  int uploadToCloud();
  int storeToSD();

private:

  BufferNode buffer[BUFFER_SIZE];
  BufferNode *head;
  BufferNode *tail;

  Data dataArray[BUFFER_SIZE];

  int dataCount;
  long totalDataCount;

};

#endif
