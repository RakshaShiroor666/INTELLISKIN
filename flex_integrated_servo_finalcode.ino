#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h> // Required for manual channel setting

// MAC Address of your Receiver ESP32
uint8_t broadcastAddress[] = {0x88, 0x57, 0x21, 0x6F, 0xBD, 0x50}; 

typedef struct struct_message {
    int flex[5];
} struct_message;

struct_message myData;

const int flexPins[] = {33, 39, 34, 35, 32}; 
const int numSensors = 5;
const char* fingerNames[] = {"Thumb(33)", "Index(39)", "Middle(34)", "Ring(35)", "Pinky(32)"};

// Callback for sending status
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);

  // 1. Initialize WiFi
  WiFi.mode(WIFI_STA);

  // 2. Force Channel 1 using Native API
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  // 3. Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // 4. Register the send callback (with cast for compatibility)
  esp_now_register_send_cb((esp_now_send_cb_t)OnDataSent);
  
  // 5. Setup Peer Info
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo)); // CRITICAL: Fixes 'Invalid Interface' error
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 1;  
  peerInfo.encrypt = false;
  
  // 6. Add the Peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  analogReadResolution(12);
  for (int i = 0; i < numSensors; i++) {
    pinMode(flexPins[i], INPUT);
  }
  
  Serial.println("System Initialized. Broadcasting on Channel 1...");
}

void loop() {
  for (int i = 0; i < numSensors; i++) {
    myData.flex[i] = analogRead(flexPins[i]);
    
    Serial.print(fingerNames[i]);
    Serial.print(": ");
    Serial.print(myData.flex[i]);
    Serial.print("\t");
  }
  
  // Send data to the Receiver
  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
  Serial.println(); 
  delay(100); 
}