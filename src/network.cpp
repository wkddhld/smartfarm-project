#include "network.h"
#include "saferanges.h"

String ap_ssid = "";
const char* ap_password = "12345678";
IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);
String wifi_ssid = "";
String wifi_password = "";

WiFiClient espClient;
PubSubClient client(espClient);

void startAPMode() {
    // MAC 주소를 가져와서 AP 이름으로 사용
    String macAddress = WiFi.macAddress();
    macAddress.replace(":", "");  
    ap_ssid = "ESP32_" + macAddress;

    WiFi.softAP(ap_ssid.c_str(), ap_password);
    WiFi.softAPConfig(local_IP, gateway, subnet);
    
    Serial.print("AP SSID: ");
    Serial.println(ap_ssid);  // AP SSID 출력
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
 
    // Root 경로로 라우팅
server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<!DOCTYPE html>"
                  "<html lang=\"en\">"
                  "<head>"
                  "<meta charset=\"UTF-8\">"
                  "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
                  "<title>Wi-Fi Setup</title>"
                  "<style>"
                  "body {"
                  "    font-family: Arial, sans-serif;"
                  "    background-color: #f4f4f4;"
                  "    display: flex;"
                  "    justify-content: center;"
                  "    align-items: center;"
                  "    height: 100vh;"
                  "    margin: 0;"
                  "}"
                  ".container {"
                  "    background-color: #fff;"
                  "    padding: 20px;"
                  "    border-radius: 8px;"
                  "    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);"
                  "    max-width: 300px;"
                  "    width: 100%;"
                  "    box-sizing: border-box;"
                  "}"
                  "h1 {"
                  "    font-size: 20px;"
                  "    margin-bottom: 20px;"
                  "    text-align: center;"
                  "}"
                  "label {"
                  "    display: block;"
                  "    margin-bottom: 10px;"
                  "    font-weight: bold;"
                  "}"
                  "input[type=\"text\"], input[type=\"password\"] {"
                  "    width: 100%;"
                  "    padding: 12px;"
                  "    margin-bottom: 20px;"
                  "    border: 2px solid #90ee90;"
                  "    border-radius: 6px;"
                  "    box-sizing: border-box;"
                  "    font-size: 16px;"
                  "}"
                  "input[type=\"submit\"] {"
                  "    width: 100%;"
                  "    padding: 12px;"
                  "    background-color: #90ee90;"
                  "    color: white;"
                  "    border: none;"
                  "    border-radius: 6px;"
                  "    cursor: pointer;"
                  "    font-size: 16px;"
                  "    text-transform: uppercase;"
                  "    font-weight: bold;"
                  "}"
                  "input[type=\"submit\"]:hover {"
                  "    background-color: #8fbc8f;"
                  "}"
                  "</style>"
                  "</head>"
                  "<body>"
                  "<div class=\"container\">"
                  "<h1>WIFI SETUP</h1>"
                  "<form action=\"/connect\" method=\"POST\">"
                  "<label for=\"ssid\">WIFI SSID:</label>"
                  "<input type=\"text\" id=\"ssid\" name=\"ssid\" placeholder=\"WIFI SSID\" required>"
                  "<label for=\"password\">WIFI PASSWORD:</label>"
                  "<input type=\"password\" id=\"password\" name=\"password\" placeholder=\"WIFI PASSWORD\" required>"
                  "<input type=\"submit\" value=\"CONNECT\">"
                  "</form>"
                  "</div>"
                  "</body>"
                  "</html>";
    request->send(200, "text/html", html);
});

    // Wi-Fi 연결 라우팅
    server.on("/connect", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
        wifi_ssid = request->getParam("ssid", true)->value();
        wifi_password = request->getParam("password", true)->value();

        WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
        request->send(200, "text/plain", "Attempting to connect to Wi-Fi...");

        int attempts = 0;
        int max_attempts = 30;  // 제한 시간을 설정하여 무한 루프 방지
        while (WiFi.status() != WL_CONNECTED && attempts < max_attempts) {
            delay(500);
            Serial.print(".");
            attempts++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            String connected_ssid = WiFi.SSID(); // 현재 연결된 Wi-Fi SSID 가져오기
            String success_msg = "Wi-Fi connected successfully.\nIP Address: " + WiFi.localIP().toString() + "\n";
            request->send(200, "text/plain", success_msg);
            Serial.println("\nWi-Fi connected successfully.");
            Serial.println("IP Address: " + WiFi.localIP().toString());
            Serial.println("Connected to: " + connected_ssid);

            // Wi-Fi에 성공적으로 연결된 경우 AP 모드 종료
            WiFi.softAPdisconnect(true);
            Serial.println("AP mode disconnected.");
        } else {
            request->send(400, "text/plain", "Failed to connect to Wi-Fi. Please try again."); // HTTP 400 Bad Request
            Serial.println("\nFailed to connect to Wi-Fi.");
        }
    } else {
        request->send(400, "text/plain", "SSID and Password required."); // HTTP 400 Bad Request
        Serial.println("SSID and Password required.");
    }
});

server.begin();
Serial.println("AP Mode enabled. Connect and provide Wi-Fi credentials.");
}

void setupWiFi() {
    startAPMode();
}

void maintainWiFi() {
    static unsigned long lastReconnectAttempt = 0;
    if (WiFi.status() != WL_CONNECTED) {
        unsigned long now = millis();
        if (now - lastReconnectAttempt > 5000) { 
            lastReconnectAttempt = now;
            Serial.println("Reconnecting to Wi-Fi...");
            WiFi.disconnect();
            WiFi.reconnect();
            if (WiFi.status() == WL_CONNECTED) {
                Serial.println("Wi-Fi reconnected successfully.");
            } else {
                Serial.println("Wi-Fi reconnection failed.");
            }
        }
    }
}

void setupMQTT() {
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    reconnect();
}
String deviceId = WiFi.macAddress(); 
void reconnect() {

    float temp = 0.0;       // 온도 초기값 예시
    float humidity = 0.0;   // 습도 초기값 예시
    float soilMoisture = 0.0; // 토양 습도 초기값 예시
    String macAddress = WiFi.macAddress(); 
    while (!client.connected()) {
        if (client.connect(ID)) {
            Serial.println("MQTT connected");
            InitialSafetyValue();  // MQTT 연결 후 초기값 구독 시작
            ModifiedSafetyValue(temp, humidity, soilMoisture, macAddress);
            ControlDevices();
        } else {
            Serial.print("Failed to connect, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}
 