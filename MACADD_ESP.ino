#include "WiFi.h"

void setup() {
  Serial.begin(115200);
  delay(1000); // Give the Serial Monitor time to connect

  // Set WiFi to Station Mode - This initializes the radio hardware
  WiFi.mode(WIFI_STA);
  WiFi.begin(); 

  Serial.println("Waking up radio...");
  delay(1000); // Wait for the hardware to stabilize

  Serial.print("SUCCESS! Your MAC Address is: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  // Nothing here
}