#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

#include <WiFi.h>

void connectToWiFi(const char* ssid, const char* password, int maxRetries = 20) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("🔌 Menghubungkan ke WiFi: ");
  Serial.println(ssid);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < maxRetries) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi tersambung!");
    Serial.print("📶 IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ Gagal menyambung ke WiFi.");
  }
}

#endif