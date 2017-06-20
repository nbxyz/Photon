#ifndef DATA_H
#define DATA_H

struct reading_structure {
  unsigned long count; // in ml
  unsigned long sequence_number;
  double timestamp;
  bool is_send;
};

#endif /* end of include guard: DATA_H */
