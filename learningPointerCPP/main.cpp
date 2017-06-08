#include "data.h"

int main() {

  int myData = 8;

  int dataArray[7];

  Data d = Data(dataArray);

  d.add(&myData,4);

  printf("Output data: %i\n", dataArray[4]);

}
