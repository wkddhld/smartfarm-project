#include "saferanges.h"
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

bool safetyValuesLoaded = false;  // 안전 범위 값이 불러와졌는지 여부를 확인하는 변수

void setupSPIFFS() {
    if (!SPIFFS.begin(true)) {
        Serial.println("An error occurred while mounting SPIFFS. Attempting to format...");
        if (!SPIFFS.format()) {
            Serial.println("SPIFFS format failed.");
        } else {
            Serial.println("SPIFFS format successful. Please reset the device.");
        }
        return;
    }
    Serial.println("SPIFFS mounted successfully");
}


// Save the safe value
void saveSafetyValues(float minTemperature, float maxTemperature, float minHumidity, float maxHumidity, float minSoilMoisture, float maxSoilMoisture) {
    StaticJsonDocument<400> doc;
    doc["minTemperature"] = minTemperature;
    doc["maxTemperature"] = maxTemperature;
    doc["minHumidity"] = minHumidity;
    doc["maxHumidity"] = maxHumidity;
    doc["minSoilMoisture"] = minSoilMoisture;
    doc["maxSoilMoisture"] = maxSoilMoisture;

    if (SPIFFS.exists("/safety_values.json")) {
        SPIFFS.remove("/safety_values.json");  // 기존 파일 삭제
        Serial.println("Previous safety values deleted.");
    }


    File file = SPIFFS.open("/safety_values.json", FILE_WRITE);

    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }

    if (serializeJson(doc, file) == 0) {
        Serial.println("Failed to write to file");
    } else {
        Serial.println("Safety values saved successfully");
        safetyValuesLoaded = true;  // 안전 범위 값이 저장되었음을 표시
    }

    file.close();
}
// load safe value
void loadSafetyValues(float &minTemperature, float &maxTemperature, float &minHumidity, float &maxHumidity, float &minSoilMoisture, float &maxSoilMoisture) {
    File file = SPIFFS.open("/safety_values.json", FILE_READ);

    if (!file) {
        Serial.println("Failed to open file for reading. Using default safety values.");
        return;
    }

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, file);

    if (error) {
        Serial.print("Failed to read existing safety values, error: ");
        Serial.println(error.c_str());
        file.close();
        return;
    } 
        minTemperature = doc["minTemperature"];
        maxTemperature = doc["maxTemperature"];
        minHumidity = doc["minHumidity"];
        maxHumidity = doc["maxHumidity"];
        minSoilMoisture = doc["minSoilMoisture"];
        maxSoilMoisture = doc["maxSoilMoisture"];

        Serial.println("Safety values loaded successfully");
        Serial.print("Min Temp: "); Serial.println(minTemperature);
        Serial.print("Max Temp: "); Serial.println(maxTemperature);
        Serial.print("Min Humidity: "); Serial.println(minHumidity);
        Serial.print("Max Humidity: "); Serial.println(maxHumidity);
        Serial.print("Min Soil Moisture: "); Serial.println(minSoilMoisture);
        Serial.print("Max Soil Moisture: "); Serial.println(maxSoilMoisture);
        
    

    file.close();
}

