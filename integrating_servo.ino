#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Initialize the PCA9685 at default I2C address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Depending on your servo make, the pulse widths for 0 and 180 degrees 
// usually fall between 150 and 600. You may need to tweak these.
#define SERVOMIN  150 // Minimum pulse length count (out of 4096)
#define SERVOMAX  600 // Maximum pulse length count (out of 4096)
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

uint8_t numServos = 5;

void setup() {
  Serial.begin(115200);
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz

  delay(10);
}

void loop() {
  Serial.println("Moving 0 to 180");
  for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) {
    for (int n = 0; n < numServos; n++) {
      pwm.setPWM(n, 0, pulselen);
    }
    delay(2); // Controls speed of sweep
  }

  delay(500);

  Serial.println("Moving 180 to 0");
  for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) {
    for (int n = 0; n < numServos; n++) {
      pwm.setPWM(n, 0, pulselen);
    }
    delay(2);
  }

  delay(500);
}