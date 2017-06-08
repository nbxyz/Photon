#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

struct CBNode {

  int *dataPtr;
  CBNode *nextPtr;

};

class CBC { // Circular Buffer Controller

public:

  CBC(int size, int *dataArrayPtr, CBNode *cirle);

  int add(int *dataPtr);
  int *get();
  int *remove();

  int getSize();
  int getLoad();
  int full();
  int empty();

private:

  int size;
  int *dataArrayPtr;

  CBNode *circle;
  CBNode *head;
  CBNode *tail;

  int load;
  int total;

  char status[64];

};

#endif /* end of include guard: CIRCULARBUFFER_H */
