#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

// MAC Address of your Receiver ESP32 (Robotic Hand)
uint8_t broadcastAddress[] = {0x88, 0x57, 0x21, 0x6F, 0xBD, 0x50}; 

// Pins for the vibration motors
const int motor1Pin = 27; 
const int motor2Pin = 26;
const int pressureThreshold = 500; // Adjust this sensitivity (0-4095)

// Structure to SEND flex data (Existing)
typedef struct struct_message {
    int flex[5];
} struct_message;

// Structure to RECEIVE pressure data (New)
typedef struct struct_feedback {
    int pressure1;
    int pressure2;
} struct_feedback;

struct_message myData;
struct_feedback incomingFeedback;

const int flexPins[] = {33, 39, 34, 35, 32}; 
const int numSensors = 5;
const char* fingerNames[] = {"Thumb(33)", "Index(39)", "Middle(34)", "Ring(35)", "Pinky(32)"};

// Existing Callback for sending status
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// NEW: Callback for receiving pressure data from the Hand
void OnDataRecv(const uint8_t * mac, const uint8_t *incoming, int len) {
  memcpy(&incomingFeedback, incoming, sizeof(incomingFeedback));
  
  Serial.print("Feedback Received -> P1: ");
  Serial.print(incomingFeedback.pressure1);
  Serial.print(" | P2: ");
  Serial.println(incomingFeedback.pressure2);

  // Trigger Motor 1
  if (incomingFeedback.pressure1 > pressureThreshold) {
    digitalWrite(motor1Pin, HIGH);
  } else {
    digitalWrite(motor1Pin, LOW);
  }

  // Trigger Motor 2
  if (incomingFeedback.pressure2 > pressureThreshold) {
    digitalWrite(motor2Pin, HIGH);
  } else {
    digitalWrite(motor2Pin, LOW);
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize motor pins
  pinMode(motor1Pin, OUTPUT);
  pinMode(motor2Pin, OUTPUT);

  WiFi.mode(WIFI_STA);

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb((esp_now_send_cb_t)OnDataSent);
  
  // REGISTER the receive callback to get haptic data
  esp_now_register_recv_cb((esp_now_recv_cb_t)OnDataRecv);
  
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo)); 
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 1;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  analogReadResolution(12);
  for (int i = 0; i < numSensors; i++) {
    pinMode(flexPins[i], INPUT);
  }
  
  Serial.println("System Initialized. Haptic Feedback Active.");
}

void loop() {
  for (int i = 0; i < numSensors; i++) {
    myData.flex[i] = analogRead(flexPins[i]);
    
    Serial.print(fingerNames[i]);
    Serial.print(": ");
    Serial.print(myData.flex[i]);
    Serial.print("\t");
  }
  
  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
  Serial.println(); 
  delay(200); 
}