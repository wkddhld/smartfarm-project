#ifndef CONFIG_H
#define CONFIG_H

#define DHT11_PIN 15
#define LUX_PIN 34

#define DHT11_PIN 15
#define LUX_PIN 34
#define SOIL_MOISTURE_PIN 35
#define PUMPA 18
#define PUMPB 19
#define RELAY_PIN 25
#define RELAY_PIN_FAN 4
#define RELAY_PIN_HUMIDIFIER 12
#define PELTIER_A 16
#define PELTIER_B 17
#define FERTILIZER_A 26
#define FERTILIZER_B 27



// MQTT settings
const char ID[] = "esp32";
const char TOPIC[] = "test_topic";

//const char mqtt_server[] = "220.88.160.207";

const char mqtt_server[] = "15.164.230.30";

// const char mqtt_server[] = "34.64.78.252";


#endif 

