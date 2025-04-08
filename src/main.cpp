#include <Arduino.h>
#include "wifi_connect.h"
#include "credentials.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  connectToWiFi(WIFI_SSID, WIFI_PASS);
}

void loop() {
  if(WiFi.status() != WL_CONNECTED)
  {
    
  }
}
