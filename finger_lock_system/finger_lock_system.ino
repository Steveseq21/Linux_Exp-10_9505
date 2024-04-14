#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

#define RELAY_PIN       4
#define ACCESS_DELAY    3000 // Keep lock unlocked for 3 seconds

void setup() {
  // Set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  
  // Verify the fingerprint sensor password
  if (!finger.verifyPassword()) {
    while (1) { 
      delay(1);
    }
  }
  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);   // Switch off relay initially. Relay is LOW level triggered relay so we need to write HIGH.
}

void loop() {
  if (getFingerPrint() != -1) {
    digitalWrite(RELAY_PIN, LOW);
    delay(ACCESS_DELAY);
    digitalWrite(RELAY_PIN, HIGH);   
  }  
  delay(50); // Add some delay before the next scan.
}

// Returns -1 if failed, otherwise returns ID #
int getFingerPrint() {
  int p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // Found a match!
  return finger.fingerID;
}
