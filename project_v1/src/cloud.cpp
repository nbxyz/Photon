#include "application.h"
#include "settings.h"
#include "cloud.h"

Cloud::Cloud() {

}

int Cloud::connect() {

  if(!WiFi.ready()) { // Check if the WiFi is already connected.

    if(!WiFi.connecting()) { // Check if the device is trying to connect.

      WiFi.connect(); // Connect!

    }

  } else {

    // Device is already connected.

  }

  return WiFi.ready(); // Return status.

}

String Cloud::getStatus() {

  if(WiFi.ready()) {

    status = "Connected!";

  } else {

    status = "Not connected!";

  }

  return status;

}

int Cloud::selectWiFi(String input) {

  if(input == "nbxyzmobile") WiFi.setCredentials("nbxyzmobile","orangepanda",WPA2);

  return connect();

}

int Cloud::publishVariable(String variableName, int *variablePointer) {

  if(variables <= 20) {
    status = "Too many variables.";
    return false;
  }

  variableNames[variables] = variableName;
  variablePointers[variables] = variablePointer;
  variables++;

  return Particle.variable(variableName,*variablePointer);

}

int Cloud::publishFunction(String functionName, int (*functionPointer)(String input)) {

  if(functions <= 12) {
    status = "Too many functions";
    return false;
  }

  functionNames[functions] = functionName;
  functionPointers[functions] = functionPointer;
  functions++;

  return Particle.function(functionName,functionPointer);

}

int Cloud::publishFunctions() {

  if(!WiFi.ready()) return false;

  bool success;

  for(int i = 0; i < functions; i++) {

    success = Particle.function(functionNames[i],functionPointers[i]);

    if(!success) {

      status = String("Function: "+functionNames[i]+" failed to be published.");

      return false;

    }

  }

  return true;

}
