#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

#include <WiFi.h>
#include <ESP32Ping.h>

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

    IPAddress targetIP = WiFi.gatewayIP();
    Serial.print("📡 Melakukan ping ke: ");
    Serial.println(targetIP);

    if (Ping.ping(targetIP, 3)) {
      Serial.println("✅ Ping berhasil! Internet kemungkinan aktif.");
    } else {
      Serial.println("⚠️ Ping gagal. Periksa koneksi jaringan.");
    }

  } else {
    Serial.println("\n❌ Gagal menyambung ke WiFi.");
  }
}

#endif
