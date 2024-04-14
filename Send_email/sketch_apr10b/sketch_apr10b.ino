#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "base64.h" // Include the base64 library

const char* ssid = "vivo Y27";        // Your WiFi SSID
const char* password = "Steveseq@71";  // Your WiFi password

const char* host = "smtp.gmail.com"; // SMTP server address
const int port = 465; // SMTP port

const char* emailFrom = "ecsminiproject22@gmail.com"; // Your Gmail address
const char* passwordEmail = "Miniproj@#25"; // Your Gmail password
const char* emailTo = "crce.9505.ecs@gmail.com"; // Recipient email address

#define IR_SENSOR_PIN 15  // Define the pin to which the IR sensor is connected

WiFiClientSecure client;

void setup() {
  Serial.begin(9600);

  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi");

  // Initialize IR sensor pin
  pinMode(IR_SENSOR_PIN, INPUT);
}

void loop() {
  int irSensorValue = digitalRead(IR_SENSOR_PIN);

  if (irSensorValue == HIGH) { // If obstacle detected (letter incoming)
    sendEmail();
    Serial.println("Email sent!");
    delay(5000); // Wait 5 seconds to prevent multiple email sending for the same event
  }

  delay(500); // Delay before next sensor check
}

void sendEmail() {
  if (!client.connect(host, port)) {
    Serial.println("Connection to SMTP server failed!");
    return;
  }

  if (!client.verify(host, String(port).c_str())) { // Convert port to string
    Serial.println("Certificate verification failed!");
    return;
  }

  client.println("EHLO example.com");
  delay(100);
  client.println("AUTH LOGIN");
  delay(100);
  client.println(base64::encode(emailFrom)); // Encode email address
  delay(100);
  client.println(base64::encode(passwordEmail)); // Encode password
  delay(100);
  client.println("MAIL FROM:<" + String(emailFrom) + ">");
  delay(100);
  client.println("RCPT TO:<" + String(emailTo) + ">");
  delay(100);
  client.println("DATA");
  delay(100);
  client.println("Subject: New Mail Notification");
  client.println("From: ESP32 Mailbox <" + String(emailFrom) + ">");
  client.println("To: " + String(emailTo));
  client.println();
  client.println("A new letter has been received in the mailbox.");
  client.println(".");
  delay(100);
  client.println("QUIT");
  delay(100);
  client.flush();
  client.stop();
}
