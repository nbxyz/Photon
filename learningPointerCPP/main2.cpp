#include "circularbuffer.h"
#include "circularbuffer.cpp"
#include "reading.h"

int main() {

  int size = 7;

  Reading dataArray[7];

  int *dataArrayPtr = (int*)dataArray;

  CBNode circle[size];

  CBC cbc = CBC(size,dataArrayPtr,circle);

  Reading myReading;

  myReading.reading = 9;
  myReading.date = 170605;
  myReading.time = 232300;

  cbc.add((int*)&myReading);

  myReading.reading = 13;

  Reading *otherReading = (Reading*)cbc.get();

  printf("Other Reading: %i\n", otherReading->reading);

  return 1;

}
