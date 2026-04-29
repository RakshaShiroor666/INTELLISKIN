#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// IMPORTANT: Replace this with your Glove's MAC Address
uint8_t gloveAddress[] = {0xC0, 0xCD, 0xD6, 0xCE, 0x62, 0xB8}; 

#define SERVO_MIN_PULSE  150 
#define SERVO_MAX_PULSE  600 

// FSR Pins
const int fsr1Pin = 36; 
const int fsr2Pin = 39;

typedef struct struct_message {
    int flex[5];
} struct_message;

typedef struct struct_feedback {
    int pressure1;
    int pressure2;
} struct_feedback;

struct_message incomingData;
struct_feedback hapticData;

// Calibration ranges
int minValues[] = {0, 0, 0, 0, 20};    
int maxValues[] = {115, 50, 150, 120, 380}; 

unsigned long lastTime = 0;
unsigned long timerDelay = 100; 

void OnDataRecv(const uint8_t * mac, const uint8_t *incoming, int len) {
  memcpy(&incomingData, incoming, sizeof(incomingData));
  
  Serial.print("Data Recv: ");
  for(int i = 0; i < 5; i++) {
    int angle = map(incomingData.flex[i], minValues[i], maxValues[i], 0, 180);
    angle = constrain(angle, 0, 180);
    int pulse = map(angle, 0, 180, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
    
    pwm.setPWM(i, 0, pulse);
    
    // Kept your requested servo pulse debugging
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
  
  esp_now_register_recv_cb((esp_now_recv_cb_t)OnDataRecv);

  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, gloveAddress, 6);
  peerInfo.channel = 1;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add glove peer");
    return;
  }

  analogReadResolution(12);
  Serial.println("Receiver Ready. Servos and Haptics active.");
}

void loop() {
  // Read and send FSR data back to glove
  if ((millis() - lastTime) > timerDelay) {
    hapticData.pressure1 = analogRead(fsr1Pin);
    hapticData.pressure2 = analogRead(fsr2Pin);

    // Added FSR values to Serial Output as requested
    Serial.print(">>> Haptic Feed -> P1: ");
    Serial.print(hapticData.pressure1);
    Serial.print(" | P2: ");
    Serial.println(hapticData.pressure2);

    esp_now_send(gloveAddress, (uint8_t *) &hapticData, sizeof(hapticData));
    lastTime = millis();
  }
}