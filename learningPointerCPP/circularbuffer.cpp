#include "circularbuffer.h"
#include <stdio.h>

CBC::CBC(int _size, int *_dataArrayPtr, CBNode *_circle) {

  size = _size;
  dataArrayPtr = _dataArrayPtr;
  circle = _circle;

  for(int i = 0; i < size; i++) {

    circle[i].dataPtr = &dataArrayPtr[i];

  }


  for(int i = 0; i < size - 1; i++) {

    circle[i].nextPtr = &circle[i];

  }

  circle[size].nextPtr = &circle[0];

  head = &circle[0];
  tail = &circle[0];

}

int CBC::add(int *dataPtr) {

  head->dataPtr = dataPtr;

  return 1;

}

int* CBC::get() {

  return tail->dataPtr;

}
