#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <ESP_Mail_Client.h>

#define WIFI_SSID "vivoY27"
#define WIFI_PASSWORD "Steveseq@71"

#define sig 15
int flag = 0;
#define SMTP_HOST "smtp.gmail.com"

/** The smtp port e.g. 
 * 25  or esp_mail_smtp_port_25
 * 465 or esp_mail_smtp_port_465
 * 587 or esp_mail_smtp_port_587
*/
#define SMTP_PORT esp_mail_smtp_port_465 //gmail

/* The log in credentials */
#define AUTHOR_EMAIL "ecsminiproject22@gmail.com"
#define AUTHOR_PASSWORD "Miniproj@#25"

/* The SMTP Session object used for Email sending */
SMTPSession smtp;

#define ON_Board_LED 2

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);

ESP_Mail_Session session;
SMTP_Message message;

const char rootCACert[] PROGMEM = "-----BEGIN CERTIFICATE-----\n"
                                  "-----END CERTIFICATE-----\n";

void setup() {
  Serial.begin(115200);

  pinMode(sig, INPUT_PULLUP); // Assuming sig is connected to a push button or a sensor with pull-up resistor
  
  pinMode(ON_Board_LED, OUTPUT);
  digitalWrite(ON_Board_LED, HIGH);

  Serial.println();

#if defined(ARDUINO_ARCH_SAMD)
  while (!Serial)
    ;
  Serial.println();
  Serial.println("* Custom built WiFiNINA firmware need to be installed.*\nTo install firmware, read the instruction here, https://github.com/mobizt/ESP-Mail-Client#install-custom-built-wifinina-firmware");
#endif

  Serial.print("Connecting to AP");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }

  digitalWrite(ON_Board_LED, HIGH);

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  smtp.debug(1);
  smtp.callback(smtpCallback);

  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;

  message.sender.name = "Smart Mail Box";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "Hi circuiTician, you have received a mail";
  message.addRecipient("Someone", "crce.9505.ecs@gmail.com");

  String textMsg = "You have received a mail, content not verified";
  message.text.content = textMsg.c_str();

  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
}

void loop() {
  if (digitalRead(sig) == LOW && flag == 0) {
    if (!smtp.connect(&session))
      return;

    if (!MailClient.sendMail(&smtp, &message))
      Serial.println("Error sending Email, " + smtp.errorReason());

    ESP_MAIL_PRINTF("Free Heap: %d\n", MailClient.getFreeHeap());
    flag = 1;
  }
  else if (flag == 1 && digitalRead(sig) == HIGH) {
    flag = 0;
  }
  else {
    // Do nothing
  }
  delay(2000);
  Serial.print("flag: ");
  Serial.println(flag);
}

void smtpCallback(SMTP_Status status) {
  Serial.println(status.info());

  if (status.success()) {
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failed: %d\n", status.failedCount());
    Serial.println("----------------\n");

    struct tm dt;
    for (size_t i = 0; i < smtp.sendingResult.size(); i++) {
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
    }
    Serial.println("----------------\n");

    smtp.sendingResult.clear();
  }
}
