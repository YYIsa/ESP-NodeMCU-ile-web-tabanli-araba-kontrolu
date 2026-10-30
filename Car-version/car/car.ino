#include <ESP8266WiFi.h>

WiFiServer server(80);
const char* ssid = "YID";
const char* password = "DENEME";

/* Motor Pin Tanımlamaları */
int leftMotorForward = 2;   // D4
int rightMotorForward = 15; // D8
int leftMotorBackward = 0;  // D3
int rightMotorBackward = 13; // D7
int rightMotorENB = 14;     // D5
int leftMotorENB = 12;      // D6

void setup() {
  Serial.begin(115200);
  
  /* Pin modlarını ayarla */
  pinMode(leftMotorForward, OUTPUT);
  pinMode(rightMotorForward, OUTPUT);
  pinMode(leftMotorBackward, OUTPUT);
  pinMode(rightMotorBackward, OUTPUT);
  pinMode(leftMotorENB, OUTPUT);
  pinMode(rightMotorENB, OUTPUT);

  /* WiFi'ye bağlan */
  connectWiFi();
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  // İstemciden veri gelene kadar bekle
  String request = client.readStringUntil('\r');
  client.flush();

  // URL içindeki komutları kontrol et (Örn: IP_ADRESI/forward)
  if (request.indexOf("/forward") != -1) MotorForward();
  else if (request.indexOf("/backward") != -1) MotorBackward();
  else if (request.indexOf("/left") != -1) TurnLeft();
  else if (request.indexOf("/right") != -1) TurnRight();
  else if (request.indexOf("/stop") != -1) MotorStop();

  // Basit bir HTTP yanıtı gönder (Tarayıcıda boş sayfa kalmaması için)
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML><html>Komut Alindi</html>");
  delay(1);
}

/* --- Motor Fonksiyonları --- */

void MotorForward() {
  digitalWrite(leftMotorENB, HIGH);
  digitalWrite(rightMotorENB, HIGH);
  digitalWrite(leftMotorForward, HIGH);
  digitalWrite(rightMotorForward, HIGH);
  digitalWrite(leftMotorBackward, LOW);
  digitalWrite(rightMotorBackward, LOW);
}

void MotorBackward() {
  digitalWrite(leftMotorENB, HIGH);
  digitalWrite(rightMotorENB, HIGH);
  digitalWrite(leftMotorBackward, HIGH);
  digitalWrite(rightMotorBackward, HIGH);
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(rightMotorForward, LOW);
}

void TurnLeft() {
  digitalWrite(leftMotorENB, HIGH);
  digitalWrite(rightMotorENB, HIGH);
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(rightMotorForward, HIGH);
  digitalWrite(leftMotorBackward, HIGH);
  digitalWrite(rightMotorBackward, LOW);
}

void TurnRight() {
  digitalWrite(leftMotorENB, HIGH);
  digitalWrite(rightMotorENB, HIGH);
  digitalWrite(leftMotorForward, HIGH);
  digitalWrite(rightMotorForward, LOW);
  digitalWrite(leftMotorBackward, LOW);
  digitalWrite(rightMotorBackward, HIGH);
}

void MotorStop() {
  digitalWrite(leftMotorENB, LOW);
  digitalWrite(rightMotorENB, LOW);
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(rightMotorForward, LOW);
  digitalWrite(leftMotorBackward, LOW);
  digitalWrite(rightMotorBackward, LOW);
}

void connectWiFi() {
  Serial.print("Baglaniyor: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Baglandi!");
  Serial.print("IP Adresi: ");
  Serial.println(WiFi.localIP());
}
