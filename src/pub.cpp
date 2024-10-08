#include "network.h"
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <saferanges.h>

extern String userEmail;  // 전역 변수 선언
extern PubSubClient client;  // MQTT 클라이언트 객체
extern String macAddressCleaned;  // 전역 변수


unsigned long previousMillisRealTime = 0;  // RealTimeData 함수용 타이머
const long intervalRealTime = 1000;        // 5초 간격

unsigned long previousMillisGraph = 0;     // GraphData 함수용 타이머
const long intervalGraph = 3600000;     

unsigned long previousMillisError = 0;     // ErrorDetected 함수용 타이머
const long intervalError = 60000; 

void RealTimeData(float temp, float humidity, float soilMoisture) {
    unsigned long currentMillis = millis();  // 현재 시간을 읽음

    // 현재 시간이 이전 시간보다 5초 이상 경과했는지 확인
    if (currentMillis - previousMillisRealTime >= intervalRealTime) {
        previousMillisRealTime = currentMillis;  // 마지막으로 publish한 시간 업데이트

        StaticJsonDocument<400> doc;  // JSON 문서 크기 증가
        doc["deviceId"] = macAddressCleaned;
        doc["temperature"] = temp;
        doc["humidity"] = humidity;
        doc["soilMoisture"] = soilMoisture;

        char buffer[400];  // 버퍼 크기 증가
        size_t n = serializeJson(doc, buffer);

        Serial.println("Serialized JSON: " + String(buffer));  // 직렬화된 JSON 출력
        Serial.println("Size of JSON: " + String(n));  // JSON 크기 출력

        String topic = "dt/farm/house/realtime";  // MQTT 토픽 설정
        Serial.print("Publishing to topic: ");
        Serial.println(topic);

        if (client.publish(topic.c_str(), buffer, n)) {  // 메시지 길이 n으로 수정
            Serial.println("Successfully published to topic: " + topic);
        } else {
            Serial.println("Failed to publish to topic: " + topic);
        }
    }
}

void GraphData(float temp, float humidity, float soilMoisture) {
    unsigned long currentMillis = millis();  // 현재 시간을 읽음

    // 현재 시간이 이전 시간보다 1분 이상 경과했는지 확인
    if (currentMillis - previousMillisGraph >= intervalGraph) {
        previousMillisGraph = currentMillis;  // 마지막으로 publish한 시간 업데이트

        StaticJsonDocument<400> doc;  // JSON 문서 크기 증가
        doc["deviceId"] = macAddressCleaned;
        doc["temperature"] = temp;
        doc["humidity"] = humidity;
        doc["soilMoisture"] = soilMoisture;

        char buffer[400];  // 버퍼 크기 증가
        size_t n = serializeJson(doc, buffer);

        Serial.println("Serialized JSON: " + String(buffer));  // 직렬화된 JSON 출력
        Serial.println("Size of JSON: " + String(n));  // JSON 크기 출력

        String topic = "dt/farm/house/graph";  // MQTT 토픽 설정
        Serial.print("Publishing to topic: ");
        Serial.println(topic);

        if (client.publish(topic.c_str(), buffer, n)) {  // 메시지 길이 n으로 수정
            Serial.println("Successfully published to topic: " + topic);
        } else {
            Serial.println("Failed to publish to topic: " + topic);
        }
    }
}

bool firstWarningSent = false; 

void ErrorDetected(float temp, float humidity, float soilMoisture) {
    unsigned long currentMillis = millis();  // 현재 시간을 읽음

    if (!firstWarningSent || (currentMillis - previousMillisError >= intervalError)) {
        previousMillisError = currentMillis;  // 마지막으로 publish한 시간 업데이트
        firstWarningSent = true;  // 첫 번째 경고를 보냈음을 표시

            // SPIFFS에서 안전 범위 값이 불러와지지 않았으면 경고를 보내지 않음
 
        // 저장된 안전 범위 불러오기
        float minTemperature, maxTemperature, minHumidity, maxHumidity, minSoilMoisture, maxSoilMoisture;
        loadSafetyValues(minTemperature, maxTemperature, minHumidity, maxHumidity, minSoilMoisture, maxSoilMoisture);

        // 실시간 값이 안전 범위를 벗어났는지 확인
        bool isTempOutOfRange = (temp < minTemperature || temp > maxTemperature);
        bool isHumidityOutOfRange = (humidity < minHumidity || humidity > maxHumidity);
        bool isSoilMoistureOutOfRange = (soilMoisture < minSoilMoisture || soilMoisture > maxSoilMoisture);

        // 안전 범위 벗어난 값이 없을 경우 종료
        if (!isTempOutOfRange && !isHumidityOutOfRange && !isSoilMoistureOutOfRange) {
            Serial.println("All values are within the safe range. No warnings sent.");
            return;
        }

        // 디바이스 ID 생성 (MAC 주소)
        String deviceId = WiFi.macAddress();  
        deviceId.replace(":", "");  // MAC 주소의 ":"를 제거

        // 온도 경고 전송
        if (isTempOutOfRange) {
            StaticJsonDocument<400> doc;
            doc["deviceId"] = deviceId;
            doc["attribute"] = "temperature";
            doc["exceed"] = (temp > maxTemperature) ? 1 : 0;

            char buffer[400];
            size_t n = serializeJson(doc, buffer);

            Serial.println("Temperature Buffer = " + String(buffer));
            String warningTopic = "dt/farm/house/temperature/warning";

            if (client.publish(warningTopic.c_str(), buffer, n)) {
                Serial.println("Successfully published temperature warning.");
            } else {
                Serial.println("Failed to publish temperature warning.");
            }
        }

        // 습도 경고 전송
        if (isHumidityOutOfRange) {
            StaticJsonDocument<400> doc;
            doc["deviceId"] = deviceId;
            doc["attribute"] = "humidity";
            doc["exceed"] = (humidity > maxHumidity) ? 1 : 0;

            char buffer[400];
            size_t n = serializeJson(doc, buffer);

            Serial.println("Humidity Buffer = " + String(buffer));
            String warningTopic = "dt/farm/house/humidity/warning";

            if (client.publish(warningTopic.c_str(), buffer, n)) {
                Serial.println("Successfully published humidity warning.");
            } else {
                Serial.println("Failed to publish humidity warning.");
            }
        }

        // 토양 수분 경고 전송
        if (isSoilMoistureOutOfRange) {
            StaticJsonDocument<400> doc;
            doc["deviceId"] = deviceId;
            doc["attribute"] = "soilMoisture";
            doc["exceed"] = (soilMoisture > maxSoilMoisture) ? 1 : 0;

            char buffer[400];
            size_t n = serializeJson(doc, buffer);

            Serial.println("Soil Moisture Buffer = " + String(buffer));
            String warningTopic = "dt/farm/house/soilMoisture/warning";

            if (client.publish(warningTopic.c_str(), buffer, n)) {
            Serial.println("Successfully published soil moisture warning.");
            } else {
            Serial.println("Failed to publish soil moisture warning.");
            }
        }
    }
}

