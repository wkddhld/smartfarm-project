# 스마트팜 & 스마트팜 관리 서비스

## 🎙️프로젝트 소개

-    집안 어디에서나 쉽게 식물을 키울 수 있으며, 웹을 이용하여 모니터링이 가능한 가정용 농장


## 🪄 기술 스택

-    Device: PlatformIO(c++)
-    DB : MongoDB(NoSQL)

## ⏰ 개발 기간

-   2024.08.12-2024.09.06

## 사용된 장비

프로젝트에서 사용된 주요 장비 목록은 다음과 같습니다

1. **DC 팬 (DC Fan)**: 
   - 온도 조절을 위해 공기 흐름을 조절하는 용도로 사용됩니다. 펠티어 소자와 함께 사용하여 온도를 제어할 수 있습니다.
   
2. **DHT11 센서**: 
   - 온도 및 습도 측정 센서로, 실내 또는 기기 주변의 환경 데이터를 수집합니다. 안전 범위 값을 기준으로 이상 상태를 감지하는 데 사용됩니다.

3. **통양수분 측정 센서**: 
   - 토양의 수분 정도를 측정하여 식물의 상태를 모니터링합니다. 특정 수분 값 이하일 경우 펌프를 작동시켜 물을 공급하는 자동화 기능을 구현합니다.

4. **펌프 (Pump)**: 
   - **두 개의 펌프**가 사용되었습니다. 
     - 첫 번째 펌프는 토양에 물을 공급하기 위한 용도로 사용됩니다. 수분 측정 센서에서 부족 상태가 감지되면 자동으로 작동됩니다.
     - 두 번째 펌프는 비료를 공급하는 역할을 합니다. 비료가 필요한 시점에 자동으로 작동하여 식물의 성장을 도울 수 있습니다.

5. **펠티어 소자 (Peltier Element)**: 
   - 열전 효과를 이용해 냉각 및 가열을 동시에 수행할 수 있는 소자입니다. 온도 조절에 매우 유용하며, DC 팬과 함께 설치하여 열을 배출합니다.

6. **12V 조명**: 
   - 식물 생장을 돕기 위한 조명으로 사용됩니다. 자동 조명 제어 기능을 통해 일정 시간 동안만 켜지도록 설정됩니다.

7. **가습기 모듈 (Humidifier Module)**: 
   - 실내 습도를 조절하여 식물의 생장 환경을 최적화하는 역할을 합니다. 제어 신호를 박으면 작동합니다. 적정 습도를 유지하는 데 기여합니다.


### ✨ 주요 기능

## 1. AP 모드 (Access Point Mode)

AP 모드는 디바이스가 자체적으로 Wi-Fi 액세스 포인트(AP)를 설정하여 다른 장치가 해당 네트워크에 연결할 수 있도록 하는 모드입니다. 이 기능을 사용하면 사용자가 기기를 네트워크에 쉽게 등록하고 관리할 수 있습니다.

### 사용 시나리오
- **기기 초기 설정**: 사용자가 네트워크에 기기를 처음 연결할 때 AP 모드를 사용하여 쉽게 등록할 수 있습니다.
- **Wi-Fi 설정 변경**: 기기의 Wi-Fi 설정을 재설정하거나 변경할 때 유용합니다.

### 특징
- **사용자 직접 등록**: 사용자가 스마트폰이나 다른 디바이스를 통해 직접 AP에 연결하고, 필요한 정보를 입력하여 기기를 네트워크에 등록할 수 있습니다.
- **네트워크 독립성**: 기존 Wi-Fi 네트워크에 의존하지 않고, 자체 네트워크를 구성해 장치 간 연결을 관리합니다.

### 구현 방법
1. **ESP32 AP 모드 설정**: AP 모드 설정을 위해 `WiFi.softAP(ssid, password)` 함수를 사용하여 SSID와 비밀번호를 설정합니다.
2. **기기 등록 페이지 제공**: HTTP 서버를 통해 AP 모드에 연결된 기기에서 웹페이지를 열어 등록 절차를 진행할 수 있습니다.
3. **MQTT 통신 설정**: AP 모드를 통해 등록된 기기를 MQTT 브로커에 연결하여 데이터를 주고받습니다.

## 2. SPIFFS 파일 적용

SPIFFS 파일 시스템을 사용하여 사용자가 기기 등록 후 입력한 안전 범위 값을 저장하고, 이를 현재 센서로부터 수집된 데이터와 비교하여 이상 알림을 전송하는 기능을 구현하였습니다. 이 알림은 MQTT를 통해 백엔드 서버에 전달됩니다.

### 사용 시나리오
- **안전 범위 설정**: 사용자가 기기 등록 시 입력한 안전 범위 값(예: 온도, 습도, 압력 등)을 SPIFFS 파일 시스템에 저장하여, 나중에 해당 값과 센서 데이터 간의 비교를 진행합니다.
- **이상 상태 감지**: 저장된 안전 범위 값을 기준으로 현재 센서에서 읽어들인 값이 안전 범위를 벗어날 경우, 이를 감지하여 백엔드 서버에 알림을 전송합니다.

### 특징
- **데이터 영구 저장**: SPIFFS 파일 시스템을 사용하여 기기 등록 시 입력된 안전 범위 값을 디바이스에 영구적으로 저장합니다.
- **MQTT 기반 알림 시스템**: 이상 상태가 감지되면 MQTT를 통해 백엔드 서버에 경고 알림을 보냅니다.
- **실시간 비교**: 센서로부터 수집된 데이터를 주기적으로 저장된 안전 범위 값과 비교하여 실시간으로 상태를 모니터링합니다.

### 구현 방법
1. **SPIFFS 파일 시스템 초기화**: 디바이스가 시작될 때 SPIFFS 파일 시스템을 초기화하여 저장된 데이터를 불러올 준비를 합니다.
2. **안전 범위 값 저장**: 사용자가 기기 등록 시 입력한 안전 범위 값을 SPIFFS 파일에 기록합니다. 이 과정에서 파일은 JSON 형식으로 저장될 수 있습니다.
3. **센서 데이터와 비교**: 주기적으로 센서에서 데이터를 수집하고, SPIFFS에 저장된 값과 비교합니다.
4. **이상 알림 전송**: 센서 데이터가 저장된 안전 범위를 벗어날 경우, 이를 감지하여 MQTT를 통해 백엔드 서버로 알림을 전송합니다.


## 트러블 슈팅 (Troubleshooting)

### 1. 시간 지연 문제 해결
기기를 작동시키는 동안 시간이 밀리는 현상이 발생하여, 정확한 타이밍 제어가 어려운 상황이 발생했습니다. 이 문제를 해결하기 위해 **`millis()`** 기능을 사용하여 현재 시간을 기준으로 기기가 동작할 수 있도록 설정하였습니다.

- **문제 원인**: 타이머나 딜레이가 걸린 상태에서 다른 작업을 병행하는 경우, 시간이 밀리거나 정확한 타이밍을 유지하기 어려운 상황이 발생함.
- **해결 방법**: `millis()` 함수를 사용하여 기기가 현재 시간을 계속 인식하게 하고, 이를 기반으로 작업을 일정 간격으로 수행하도록 개선하였습니다. 이로 인해 시간이 밀리는 문제를 해결하고, 동시 작업을 효과적으로 처리할 수 있게 되었습니다.


### `millis()` 함수 적용 예시 코드:
```cpp
unsigned long previousMillis = 0;
const long interval = 1000;  // 1초 간격

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    // 1초마다 실행할 작업
    previousMillis = currentMillis;
    Serial.println("1초마다 실행");
  }
}






```## 프로젝트 파일 구조
|-- include/           # 헤더 파일 모음
|   |-- config.h       # 각 기기의 핀 설정 및 MQTT 서버 주소 정의
|   |-- network.h      # 네트워크 관련 기능 선언
|   |-- saferanges.h   # 범위 안전성 검사 관련 함수 선언
|   |-- sensors.h      # 센서 데이터 처리 관련 함수 선언
|
|-- src/               # 소스 코드 모음
|   |-- callback.cpp   # 콜백 함수 정의
|   |-- main.cpp       # 메인 실행 파일
|   |-- network.cpp    # AP모드 관리와 mqtt 연결 기능 구현
|   |-- pub.cpp        # MQTT Publish 기능 구현
|   |-- saferanges.cpp # 안전 범위 계산 및 spiffs 파일
|   |-- sensors.cpp    # 측정센서 데이터 처리 구현
|   |-- sub.cpp        # MQTT Subscribe 기능 구현
|
|
|-- .env               # 환경 변수 파일 (.gitignore에 추가 필요)
|-- platform.ini       # PlatformIO 설정 파일
|-- .gitignore         # Git에 포함하지 않을 파일 목록
|-- project.md         # 프로젝트 설명 파일
|-- file.md             # 파일 구조 설명 MD```



