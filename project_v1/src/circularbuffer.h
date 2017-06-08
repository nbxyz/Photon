#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

struct CircularBufferNode {

  int *dataPtr;
  CircularBufferNode *nextPtr;

};

class CBC { // Circular Buffer Controller

public:

  CBC(int size, int *dataArrayPtr);

  int add(int *dataPtr); // Add to head position.
  int *look(); // Looks at tail but doesn't remove it.
  int *remove(); // Gets tail and removes it.

  int getSize();
  int getLoad();
  int full();
  int empty();

private:

  int size;

  int dataArrayPtr;

  CircularBufferNode bufferController[];
  CircularBufferNode *head;
  CircularBufferNode *tail;

  int load;
  int total;

  String status;

};

#endif /* end of include guard: CIRCULARBUFFER_H */
