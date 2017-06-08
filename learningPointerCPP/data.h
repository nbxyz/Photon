#ifndef DATA_H
#define DATA_H

#include <stdio.h>

class Data {

public:

  Data(int *dataArrayPtr);

  int add(int *data, int n);

private:

  int *dataArrayPtr;

};

Data::Data(int *_dataArrayPtr) {

  dataArrayPtr = _dataArrayPtr;

}

int Data::add(int *data, int n) {

  //int input = *data;

  printf("Input data: %i\n",*data);

  dataArrayPtr[n] = *data;

  return 0;

}

#endif /* end of include guard: DATA_H */
