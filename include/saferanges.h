#ifndef SAFERANGES_H
#define SAFERANGES_H

void setupSPIFFS();
void saveSafetyValues(float minTemperature, float maxTemperature, float minHumidity, float maxHumidity, float minSoilMoisture, float maxSoilMoisture);
void loadSafetyValues(float &minTemperature, float &maxTemperature, float &minHumidity, float &maxHumidity, float &minSoilMoisture, float &maxSoilMoisture);

#endif
