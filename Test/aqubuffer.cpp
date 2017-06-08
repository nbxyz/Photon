#ifndef AQUBUFFER_CPP
#define AQUBUFFER_CPP

#include "application.h"
#include "aqubuffer.h"

AquBuffer::AquBuffer() {

  for(int i = 0; i < BUFFER_SIZE; i++) {

    // First layer is the data pointer
    buffer[i].dataPtr = &dataArray[i];

  }

  for(int i = 0; i < BUFFER_SIZE - 1; i++) {

    // Point to next data one in the buffer
    buffer[i].nextPtr = &buffer[i+1];

  }

  // Complete the circular buffer
  buffer[BUFFER_SIZE-1].nextPtr = &buffer[0];

  // Setup head and tail.
  head = &buffer[0];
  tail = &buffer[0];

}

int AquBuffer::add(Data data) {

  if(dataCount < BUFFER_SIZE) {

    // Make sure the new data doesn't appear stored or uploaded.
    data.stored = 0;
    data.uploaded = 0;

    // Stupid work around using a pointer pointer to change the data.
    Data *tmpDataPtr = head->dataPtr;
    *tmpDataPtr = data;

    // Update head pointer
    head = head->nextPtr;

    // Increment counters
    dataCount++;
    totalDataCount++;

  } else {

    // Buffer is full.

    return -1;

  }

  return 1;

}

int AquBuffer::remove() {

  if(dataCount) {

    tail = tail->nextPtr;

    dataCount--;

  } else {

    // Buffer is empty.

    return -1;

  }

  return 1;

}

int AquBuffer::uploadToCloud() {

}

int AquBuffer::storeToSD() {



}

#endif
