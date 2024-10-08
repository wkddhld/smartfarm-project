#include "network.h"
#include "saferanges.h"

String userObjectId;

unsigned long pumpStartTime = 0;  // 펌프 작동 시작 시간
bool pumpActive = false;          // 펌프 활성화 상태
int pumpDuration = 0; 
unsigned long fertilizerStartTime = 0;  // 펌프 작동 시작 시간
bool fertilizerActive = false;          // 펌프 활성화 상태
int fertilizerDuration = 0; 

void callback(char *topic, byte *payload, unsigned int length) {
    String messageTemp;
    Serial.println("Received topic: " + String(topic));

    // 원본 메시지 출력
    for (int i = 0; i < length; i++) {
        messageTemp += (char)payload[i];
    }
    Serial.println("Message payload: " + messageTemp);

    // JSON 파싱 시도
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, messageTemp);

    // 파싱 오류 처리
    if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.f_str());
        return;
    }

    // Lighting and pump topics do not require userObjectId processing
    if (strstr(topic, "lighting/control/req")) {
        bool control = doc["control"]; 
        
        Serial.print("Lighting control command received: ");
        Serial.println(control ? "ON" : "OFF");
        digitalWrite(RELAY_PIN, control ? HIGH : LOW);

    } else if (strstr(topic, "pump/control/req")) {
        int water = doc["water"] | -999;
        Serial.print("Pump control command received: ");
        Serial.println(water);

        // Turn off the pumps initially
        ledcWrite(0, 0);  // Turn off PUMPA
        ledcWrite(1, 0);  // Turn off PUMPB

        // Calculate the duration to run the pump in seconds
        pumpDuration = (water / 100);  // water value in hundredths of a second

        if (pumpDuration > 0) {
            ledcWrite(0, 230);  // Turn on PUMPA with 230 (90% duty cycle)
            pumpStartTime = millis();  // Record the start time
            pumpActive = true;  // Activate the pump
        } else {
            Serial.println("Invalid water value, pumps will remain off.");
        }
        
    } else if (strstr(topic, "fertilizer/control/req")) {
        int nutrient = doc["nutrient"] | -999;
        Serial.print("fertilizer control command received: ");
        Serial.println(nutrient);

        // Turn off the fertilzers initially
        ledcWrite(4, 0);  // Turn off A
        ledcWrite(5, 0);  // Turn off B

        // Calculate the duration to run the fertilzer in seconds
        fertilizerDuration = (nutrient / 10);

        if (fertilizerDuration > 0) {
            ledcWrite(4, 230);  // Turn on A with 230 (90% duty cycle)
            fertilizerStartTime = millis();  // Record the start time
            fertilizerActive = true;  // Activate the fertilzer
        } else {
            Serial.println("Invalid nutrient value, fertilizers will remain off.");
        }

    } else if (strstr(topic, "dcpan/control/req")) {
        bool control = doc["control"]; 
        
        Serial.print("Fan control command received: ");
        Serial.println(control ? "ON" : "OFF");
        digitalWrite(RELAY_PIN_FAN, control ? HIGH : LOW);

    } else if (strstr(topic, "heater/control/req")) {
        bool control = doc["control"]; 
        
        Serial.print("Heater control command received: ");
        Serial.println(control ? "ON" : "OFF");

        if (control) {
            // 히터 켜기 (PWM 신호를 통해)
            ledcWrite(2, 230);  // PELTIER_A 켜기
            ledcWrite(3, 0);    // PELTIER_B 끄기
        } else {
            // 히터 끄기
            ledcWrite(2, 0);    // PELTIER_A 끄기
            ledcWrite(3, 0);    // PELTIER_B 끄기
        }
        
    } else if (strstr(topic, "humidifier/control/req")) {
        bool control = doc["control"]; 
        
        Serial.print("Humidifier control command received: ");
        Serial.println(control ? "ON" : "OFF");
        digitalWrite(RELAY_PIN_HUMIDIFIER, control ? HIGH : LOW);

    }

    else {
        static float minTemperature = -999, maxTemperature = -999;
        static float minHumidity = -999, maxHumidity = -999;
        static float minSoilMoisture = -999, maxSoilMoisture = -999;

        // 초기값 처리 및 저장
        if (strstr(topic, "temperature") && !strstr(topic, "modify")) {
            minTemperature = doc["minTemperature"] | -999;
            maxTemperature = doc["maxTemperature"] | -999;
            Serial.print("Min Temperature: ");
            Serial.println(minTemperature);
            Serial.print("Max Temperature: ");
            Serial.println(maxTemperature);

        } else if (strstr(topic, "humidity") && !strstr(topic, "modify")) {
            minHumidity = doc["minHumidity"] | -999;
            maxHumidity = doc["maxHumidity"] | -999;
            Serial.print("Min Humidity: ");
            Serial.println(minHumidity);
            Serial.print("Max Humidity: ");
            Serial.println(maxHumidity);

        } else if (strstr(topic, "soilMoisture") && !strstr(topic, "modify")) {
            minSoilMoisture = doc["minSoilMoisture"] | -999;
            maxSoilMoisture = doc["maxSoilMoisture"] | -999;
            Serial.print("Min Soil Moisture: ");
            Serial.println(minSoilMoisture);
            Serial.print("Max Soil Moisture: ");
            Serial.println(maxSoilMoisture);
        }

        // 초기값이 모두 설정된 경우 저장
        if (minTemperature != -999 && maxTemperature != -999 &&
            minHumidity != -999 && maxHumidity != -999 &&
            minSoilMoisture != -999 && maxSoilMoisture != -999) {
            
            saveSafetyValues(minTemperature, maxTemperature, minHumidity, maxHumidity, minSoilMoisture, maxSoilMoisture);
            Serial.println("Safety values saved successfully.");
        }

        // 수정된 값 처리 및 업데이트
  bool modified = false;  // 값이 수정되었는지 확인하는 플래그

if (strstr(topic, "temperature/modify")) {
    float modifiedTemp = doc["temp"] | -999;
    if (modifiedTemp != -999) {
        minTemperature = modifiedTemp;
        maxTemperature = modifiedTemp;  // 필요에 따라 동일하게 설정
        Serial.println("Modified Temperature updated.");
        modified = true;  // 값이 수정되었음을 표시
    }

} else if (strstr(topic, "humidity/modify")) {
    float modifiedHumidity = doc["humidity"] | -999;
    if (modifiedHumidity != -999) {
        minHumidity = modifiedHumidity;
        maxHumidity = modifiedHumidity;  // 필요에 따라 동일하게 설정
        Serial.println("Modified Humidity updated.");
        modified = true;  // 값이 수정되었음을 표시
    }

} else if (strstr(topic, "soilMoisture/modify")) {
    float modifiedSoilMoisture = doc["soilMoisture"] | -999;
    if (modifiedSoilMoisture != -999) {
        minSoilMoisture = modifiedSoilMoisture;
        maxSoilMoisture = modifiedSoilMoisture;  // 필요에 따라 동일하게 설정
        Serial.println("Modified Soil Moisture updated.");
        modified = true;  // 값이 수정되었음을 표시
    }
}

// 수정된 값이 있을 때만 한 번 저장
if (modified) {
    saveSafetyValues(minTemperature, maxTemperature, minHumidity, maxHumidity, minSoilMoisture, maxSoilMoisture);
    Serial.println("Modified safety values saved successfully.");
    ErrorDetected(minTemperature, minHumidity, minSoilMoisture);
}
    }
}