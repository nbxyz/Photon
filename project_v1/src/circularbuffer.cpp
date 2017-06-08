#include "circularbuffer.h"

CBC::CBC(int _size, int *_dataArrayPtr) {

  size = _size;
  dataArrayPtr = _dataArrayPtr;

  bufferController[size];

  for(int i = 0; i < size; i++) {

    bufferController[i].dataPtr = &dataArrayPtr[i];

  }

  for(int i = 0; i < size - 1; i++) {

    bufferController[i].nextPtr = &bufferController[i+1];

  }

  bufferController[size-1].nextPtr = &bufferController[0];

  head = &bufferController[0];
  tail = &bufferController[0];

}

int CBC::add(int *dataPtr) {

  if(full()) {
    status = "Buffer is full.";
    return false;
  }

  int **tempDataPtrPtr;


}

int CBC::full() {

  if(load == size) return true;
  else false;

}

int CBC::empty() {

  if(load == 0) return true;
  else false;

}
