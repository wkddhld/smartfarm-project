#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include "config.h"
// sensor initialization function declaration
void setupSensors();

// sensor dadta read function declaration
float readTemperature();
float readHumidity();
int readSoilMoisture();
int readLux();




#endif   

