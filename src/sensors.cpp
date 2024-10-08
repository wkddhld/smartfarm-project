#include "sensors.h"
#include "config.h"
#include <DHT.h>

// Create DHT11 sensor object
DHT dht(DHT11_PIN, DHT11);

void setupSensors() {
    dht.begin();  // Initialize DHT11 sensor
    pinMode(DHT11_PIN, INPUT);  // Set DHT11 pin mode to input
}

float readTemperature() {
    return dht.readTemperature();  // Read temperature
}

float readHumidity() {
    return dht.readHumidity();  // Read humidity
}

int readSoilMoisture() {
    int rawValue = analogRead(SOIL_MOISTURE_PIN);  // Read soil moisture sensor value
    int maxValue = 4095;  // Maximum value from the sensor (assuming 12-bit ADC)

    // Convert the raw value to a percentage
    int percentage = 100 - ((rawValue / (float)maxValue) * 100);

    return percentage;
}

int readLux() {
    return analogRead(LUX_PIN);  // Read light intensity
}
