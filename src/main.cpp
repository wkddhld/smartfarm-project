#include <Arduino.h>
#include "sensors.h"
#include "network.h"
#include "config.h"
#include "saferanges.h"

extern unsigned long pumpStartTime;  // 펌프 작동 시작 시간
extern bool pumpActive;          // 펌프 활성화 상태
extern int pumpDuration; 
extern unsigned long fertilizerStartTime;  // 펌프 작동 시작 시간
extern bool fertilizerActive;          // 펌프 활성화 상태
extern int fertilizerDuration; 


// Initialization function
void setup() {
    Serial.begin(115200);
    
    setupWiFi();
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("WiFi connected"); 
    setupMQTT();// SETUPMQTT - reconnect - Initialvalue() Sequence -unsub-subudate()
    setupSPIFFS();

    pinMode(RELAY_PIN, OUTPUT);
    pinMode(RELAY_PIN_FAN, OUTPUT);
    pinMode(RELAY_PIN_HUMIDIFIER, OUTPUT);

    // 조명 초기 상태 설정 (OFF 상태)
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(RELAY_PIN_FAN, LOW);
    digitalWrite(RELAY_PIN_HUMIDIFIER, LOW);
    setupSensors();

    ledcSetup(0, 5000, 8); // 채널 0, 5kHz, 8-bit 해상도
    ledcSetup(1, 5000, 8); // 채널 1, 5kHz, 8-bit 해상도
    ledcSetup(2, 5000, 8); // 채널 2, 5kHz, 8-bit 해상도
    ledcSetup(3, 5000, 8); // 채널 3, 5kHz, 8-bit 해상도
    ledcSetup(4, 5000, 8); // 채널 4, 5kHz, 8-bit 해상도
    ledcSetup(5, 5000, 8); // 채널 5, 5kHz, 8-bit 해상도

    // PWM 채널을 PUMPA와 PUMPB 핀에 연결합니다
    ledcAttachPin(PUMPA, 0); // PUMPA 핀을 채널 0에 연결
    ledcAttachPin(PUMPB, 1); // PUMPB 핀을 채널 1에 연결

    ledcAttachPin(PELTIER_A, 2); // PELTIER_A 핀을 채널 0에 연결
    ledcAttachPin(PELTIER_B, 3); // PELTIER_B 핀을 채널 1에 연결

    ledcAttachPin(FERTILIZER_A, 4); // PELTIER_A 핀을 채널 0에 연결
    ledcAttachPin(FERTILIZER_B, 5); // PELTIER_B 핀을 채널 1에 연결
}

// Sensor data reading and processing function
void readSensors() {
    // Read sensor data
    float temp = readTemperature();
    float humid = readHumidity();
    int soilMoisture = readSoilMoisture();
    int lux = readLux();

    // Print sensor data (or send as MQTT messages)
    Serial.print("Temperature: ");
    Serial.println(temp);
    Serial.print("Humidity: ");
    Serial.println(humid);
    Serial.print("Soil Moisture: ");
    Serial.println(soilMoisture);
    Serial.print("Lux: ");
    Serial.println(lux);
    
    delay(1000);
}

void loop() {
    
    maintainWiFi();      
    
    if (WiFi.status() == WL_CONNECTED) {
       
        maintainMQTT();
      
        if (client.connected()) {
            
            float temp = readTemperature();
            float humid = readHumidity();
            int soilMoisture = readSoilMoisture();
            String macAddress = WiFi.macAddress(); 
            
            readSensors();

            RealTimeData(temp, humid, soilMoisture);
            GraphData(temp, humid, soilMoisture);
            ErrorDetected(temp, humid, soilMoisture);
        }
    } else {
        
        Serial.println("Wi-Fi disconnected. Re-entering AP mode.");
        setupWiFi();  
    }

    if (pumpActive && (millis() - pumpStartTime >= pumpDuration * 1000)) {
        // 펌프를 끄기
        ledcWrite(0, 0);  // PUMPA를 끔
        ledcWrite(1, 0);  // PUMPB를 끔

        Serial.println("Pump turned off after duration.");

        // 펌프 활성화 상태를 비활성화로 설정
        pumpActive = false;
    }

    if (fertilizerActive && (millis() - fertilizerStartTime >= fertilizerDuration * 1000)) {
        // 펌프를 끄기
        ledcWrite(4, 0);  // A를 끔
        ledcWrite(5, 0);  // B를 끔

        Serial.println("Fertilizer turned off after duration.");

        // 펌프 활성화 상태를 비활성화로 설정
        fertilizerActive = false;
    }
    
}

// MQTT connection maintenance function
void maintainMQTT() {
    if (!client.connected()) {
        reconnect();       // Reconnect MQTT
    }
    client.loop();          // Maintain MQTT client loop
}







