#ifndef CLOUD_H
#define CLOUD_H

#include "application.h"

class Cloud {

public:

  Cloud();

  int connect(); // reconnect

  int selectWiFi(String input); // Set the specifies input

  int publishVariable(String variableName, int *variablePointer);

  int publishVariables();

  int publishFunction(String function, int (*funcPtr)(String input));

  int publishFunctions();

  String getStatus();

private:

  String status;

  String functionNames[12];
  int (*functionPointers[12])(String input);
  int functions;

  String variableNames[20];
  int *variablePointers[20];
  int variables;

};

#endif /* end of include guard: CLOUD_H */
