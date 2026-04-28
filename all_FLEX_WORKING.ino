// Defining the ADC the pins
const int flexPins[] = {36, 33, 34, 35, 32}; 
const int numSensors = 5;
const char* fingerNames[] = {"Thumb", "Index", "Middle", "Ring ", "Pinky "};

void setup() {
  Serial.begin(115200);
  
  // Configuration ADC resolution 
  analogReadResolution(12);
  
  for (int i = 0; i < numSensors; i++) {
    pinMode(flexPins[i], INPUT);
  }
  
  Serial.println("System Initialized. Flex Sensor Data:");
  delay(1000);
}

void loop() {
  for (int i = 0; i < numSensors; i++) {
    int rawValue = analogRead(flexPins[i]);
    
    // Printing the values formatted for the Serial Monitor
    Serial.print(fingerNames[i]);
    Serial.print(": ");
    Serial.print(rawValue);
    Serial.print("\t"); // Tab spacing
  }
  
  Serial.println(); // New line for next set of readings
  delay(100); //delay for stability
}