#ifndef NETWORK_H
#define NETWORK_H

#include <PubSubClient.h>
#include <WiFi.h>
#include "config.h"
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

extern AsyncWebServer server;
extern PubSubClient client;

void setupWiFi();
void maintainWiFi();
void setupMQTT();
void maintainMQTT();
void reconnect();

void InitialSafetyValue();
void ControlDevices();
void callback(char* topic, byte* payload, unsigned int length); 
void ModifiedSafetyValue(float temp, float humidity, float soilMoisture, const String& macAddress);


void RealTimeData(float temp, float humidity, float soilMoisture);
void ErrorDetected(float temp, float humidity, float soilMoisture);
void GraphData(float temp, float humidity, float soilMoisture);

#endif


