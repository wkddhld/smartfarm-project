#include "network.h"

String macAddressCleaned;

void InitialSafetyValue() {
    macAddressCleaned = WiFi.macAddress();
    macAddressCleaned.replace(":", "");

    // String temperatureTopic = "farm/house/temperature/" + macAddressCleaned;
    // String humidityTopic = "farm/house/humidity/" + macAddressCleaned;
    // String soilMoistureTopic = "farm/house/soilMoisture/" + macAddressCleaned;
    String temperatureTopic = "farm/house/temperature";
    String humidityTopic = "farm/house/humidity";
    String soilMoistureTopic = "farm/house/soilMoisture";

    if (client.subscribe(temperatureTopic.c_str(), 1)) {
        Serial.println("Successfully subscribed to temperature topic: " + temperatureTopic);
    } else {
        Serial.println("Failed to subscribe to temperature topic: " + temperatureTopic);
    }

    if (client.subscribe(humidityTopic.c_str(), 1)) {
        Serial.println("Successfully subscribed to humidity topic: " + humidityTopic);
    } else {
        Serial.println("Failed to subscribe to humidity topic: " + humidityTopic);
    }

    if (client.subscribe(soilMoistureTopic.c_str(), 1)) {
        Serial.println("Successfully subscribed to soil moisture topic: " + soilMoistureTopic);
    } else {
        Serial.println("Failed to subscribe to soil moisture topic: " + soilMoistureTopic);
    }
}

void ModifiedSafetyValue(float temp, float humidity, float soilMoisture, const String& macAddress) {
    // String macAddressCleaned = macAddress;
    // macAddressCleaned.replace(":", "");

    // String temperatureTopic = "farm/house/temperature/modify/" + macAddressCleaned;
    // String humidityTopic = "farm/house/humidity/modify/" + macAddressCleaned;
    // String soilMoistureTopic = "farm/house/soilMoisture/modify/" + macAddressCleaned;
    String temperatureTopic = "farm/house/temperature/modify";
    String humidityTopic = "farm/house/humidity/modify";
    String soilMoistureTopic = "farm/house/soilMoisture/modify";

    client.subscribe(temperatureTopic.c_str(), 1);
    client.subscribe(humidityTopic.c_str(), 1);
    client.subscribe(soilMoistureTopic.c_str(), 1);


    if (client.subscribe(temperatureTopic.c_str(), 1)) {
        Serial.println("Successfully subscribed to modified temperature topic: " + temperatureTopic);
    } else {
        Serial.println("Failed to subscribe to modified temperature topic: " + temperatureTopic);
    }

    if (client.subscribe(humidityTopic.c_str(), 1)) {
        Serial.println("Successfully subscribed to modified humidity topic: " + humidityTopic);
    } else {
        Serial.println("Failed to subscribe to modified humidity topic: " + humidityTopic);
    }

    if (client.subscribe(soilMoistureTopic.c_str(), 1)) {
        Serial.println("Successfully subscribed to modified soil moisture topic: " + soilMoistureTopic);
    } else {
        Serial.println("Failed to subscribe to modified soil moisture topic: " + soilMoistureTopic);
    }
}


void ControlDevices() {
    // String macAddressCleaned = WiFi.macAddress();
    // macAddressCleaned.replace(":", "");

    // // 조명 제어 토픽 생성
    // String lightingControlTopic = "cmd/farm/house/lighting/control/req/" + macAddressCleaned;
    String lightingControlTopic = "cmd/farm/house/lighting/control/req";

    // Lighting Control Topic Subscription
    if (client.subscribe(lightingControlTopic.c_str(), 1)) {
        Serial.println("Successfully subscribed to lighting control topic: " + lightingControlTopic);
    } else {
        Serial.println("Failed to subscribe to lighting control topic: " + lightingControlTopic);
    }

    // 펌프 제어 토픽 생성
    // String pumpControlTopic = "cmd/farm/house/pump/control/req/" + macAddressCleaned;
    String pumpControlTopic = "cmd/farm/house/pump/control/req";

    // Pump Control Topic Subscription
    if (client.subscribe(pumpControlTopic.c_str(), 1)) {
        Serial.println("Successfully subscribed to pump control topic: " + pumpControlTopic);
    } else {
        Serial.println("Failed to subscribe to pump control topic: " + pumpControlTopic);
    }

    // DC Fan Motor Control Topic
    String fanControlTopic = "cmd/farm/house/dcpan/control/req";

    if (client.subscribe(fanControlTopic.c_str(), 1)) {
        Serial.println("Successfully subscribed to fan control topic: " + fanControlTopic);
    } else {
        Serial.println("Failed to subscribe to fan control topic: " + fanControlTopic);
    }

    // Heater Control Topic
    String heaterControlTopic = "cmd/farm/house/heater/control/req";

    if (client.subscribe(heaterControlTopic.c_str(), 1)) {
        Serial.println("Successfully subscribed to heater control topic: " + heaterControlTopic);
    } else {
        Serial.println("Failed to subscribe to heater control topic: " + heaterControlTopic);
    }

    // Humidifier Control Topic
    String humidifierControlTopic = "cmd/farm/house/humidifier/control/req";

    if (client.subscribe(humidifierControlTopic.c_str(), 1)) {
        Serial.println("Successfully subscribed to humidifier control topic: " + humidifierControlTopic);
    } else {
        Serial.println("Failed to subscribe to humidifier control topic: " + humidifierControlTopic);
    }

    // Fertilzer Control Topic
    String fertilizerControlTopic = "cmd/farm/house/fertilizer/control/req";

    if (client.subscribe(fertilizerControlTopic.c_str(), 1)) {
        Serial.println("Successfully subscribed to fertilizer control topic: " + fertilizerControlTopic);
    } else {
        Serial.println("Failed to subscribe to fertilizer control topic: " + fertilizerControlTopic);
    }
}


