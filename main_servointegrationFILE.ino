#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVO_MIN_PULSE  150 
#define SERVO_MAX_PULSE  600 

typedef struct struct_message {
    int flex[5];
} struct_message;

struct_message incomingData;

// Calibration ranges
int minValues[] = {0, 0, 0, 0, 20};    
int maxValues[] = {115, 50, 150, 120, 380}; 

// CLASSIC CALLBACK FORMAT (For ESP32 Core 2.0.17)
void OnDataRecv(const uint8_t * mac, const uint8_t *incoming, int len) {
  memcpy(&incomingData, incoming, sizeof(incomingData));
  
  Serial.print("Data Recv: ");
  for(int i = 0; i < 5; i++) {
    int angle = map(incomingData.flex[i], minValues[i], maxValues[i], 0, 180);
    angle = constrain(angle, 0, 180);
    int pulse = map(angle, 0, 180, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
    
    pwm.setPWM(i, 0, pulse);
    
    // DEBUG: Look at these numbers in the Serial Monitor
    Serial.print("S"); Serial.print(i); 
    Serial.print(":"); Serial.print(pulse); 
    Serial.print(" ");
  }
  Serial.println();
}
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  pwm.begin();
  pwm.setPWMFreq(50); 

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // FIXED LINE: Adding the (esp_now_recv_cb_t) cast to bypass the error
  esp_now_register_recv_cb((esp_now_recv_cb_t)OnDataRecv);
  
  Serial.println("Receiver Ready. Waiting for data...");
}

void loop() {
}