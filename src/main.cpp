#include <Arduino.h>
#include "wifi_connect.h"
#include "credentials.h"
#include "gps_module.h"
#include "rs485_comm.h"
#include "ledMat.h"

// === Sensor Data ===
float lpg = 0.0, co = 0.0, smoke = 0.0;
float t1 = 0.0, t2 = 0.0, t3 = 0.0, t4 = 0.0;
float humid = 0.0, press = 0.0;

// === Pins ===
#define RS485_DE_PIN 32
#define RS485_RE_PIN 33

// === GPS ===
GPSModule gps(Serial2);
long gpsLastUpdate = 0;
long gpsUpdateInterval = 1000; // 1 second
double latitude = 0.0;
double longitude = 0.0;
double altitude = 0.0;
int satellites = 0;

// === RS485 ===
#define RS485_BAUD 9600
RS485Comm rs485(Serial1, RS485_DE_PIN, RS485_RE_PIN, RS485_BAUD); // DE = GPIO32, RE = GPIO33

void setup() {
  Serial.begin(115200);
  rs485.begin();
  delay(1000);
  connectToWiFi(WIFI_SSID, WIFI_PASS);
  Serial.println("Menghubungkan GPS");
  gps.begin(9600);
  Serial.println("GPS Module siap...");
  initLEDMAT();
  delay(100);
  displayLEDMAT("Ruangan Aman");
}

void loop() {
  if(WiFi.status() != WL_CONNECTED)
  {
    connectToWiFi(WIFI_SSID, WIFI_PASS);
  }
  gpsRoutine(); 
  updateLEDMAT(); 
  readRS485();
  delay(50);
}

void readRS485() {
  if (rs485.available()) {
    String data = rs485.readLine();  // Terima satu baris
    Serial.print("📥 Diterima: ");
    Serial.println(data);

    // Parsing data (contoh: LPG:23.45;CO:14.67;SMK:76.23;...)
    parseData(data);
  }
}

void parseData(const String& data) {
  int start = 0;
  while (start < data.length()) {
    int sep = data.indexOf(':', start);
    int end = data.indexOf(';', sep);
    if (sep == -1 || end == -1) break;

    String key = data.substring(start, sep);
    float value = data.substring(sep + 1, end).toFloat();

    if (key == "LPG") lpg = value;
    else if (key == "CO") co = value;
    else if (key == "SMK") smoke = value;
    else if (key == "T1") t1 = value;
    else if (key == "T2") t2 = value;
    else if (key == "T3") t3 = value;
    else if (key == "T4") t4 = value;
    else if (key == "HUM") humid = value;
    else if (key == "PRS") press = value;

    start = end + 1;
  }
}

void gpsRoutine() {
  if (gpsUpdateInterval > millis() - gpsLastUpdate) 
  {
    gpsLastUpdate = millis();
    gps.update();
    if (gps.hasValidLocation()) 
    {
      altitude = gps.getAltitude();
      latitude = gps.getLatitude();
      longitude = gps.getLongitude();
      satellites = gps.getSatellites();
    }
    delay(1000);
  }
}