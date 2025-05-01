#include <Arduino.h>
#include "wifi_connect.h"
#include "credentials.h"
#include "gps_module.h"

GPSModule gps(Serial2);

void setup() {
  Serial2.begin(9600, SERIAL_8N1, 16, 17);  // RX=16, TX=17
  Serial.begin(115200);
  delay(1000);
  // connectToWiFi(WIFI_SSID, WIFI_PASS);
  Serial.println("Menghubungkan GPS");
  gps.begin(9600);
  Serial.println("GPS Module siap...");
}

void loop() {
  // if(WiFi.status() != WL_CONNECTED)
  // {
  //   connectToWiFi(WIFI_SSID, WIFI_PASS);
  // }

  gps.update();
  if (gps.hasValidLocation()) {
    gps.printStatus();
  }
  delay(1000);
}
