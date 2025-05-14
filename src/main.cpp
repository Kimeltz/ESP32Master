#include <Arduino.h>
#include "wifi_connect.h"
#include "credentials.h"
#include "gps_module.h"
#include "rs485_comm.h"
#include "ledMat.h"
#include <HTTPClient.h>

// === Sensor Data ===
float lpg = 0.0, co = 0.0, smoke = 0.0;
float t1 = 0.0, t2 = 0.0, t3 = 0.0, t4 = 0.0;
float humid = 0.0, press = 0.0;
String SID;

// === Pins ===
#define RS485_DE_PIN 32
#define RS485_RE_PIN 33

// === GPS ===
HardwareSerial GPS_Serial(2);
GPSModule gps(GPS_Serial);
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
  sendDataToServer();
  if (detectFire()) {
    Serial.println("🔥 Kebakaran terdeteksi!");
    displayLEDMAT("Kebakaran Terdeteksi");
  } else {
    Serial.println("✅ Ruangan Aman");
    displayLEDMAT("Ruangan Aman");
  }
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

    if (key == "SID") SID = value;
    // else if (key == "LPG") lpg = value;
    else if (key == "CO") co = value;
    else if (key == "GAS") lpg = value;
    // else if (key == "SMK") smoke = value;
    else if (key == "TEMP1") t1 = value;
    else if (key == "TEMP2") t2 = value;
    else if (key == "TEMP3") t3 = value;
    else if (key == "TEMP4") t4 = value;
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

bool detectFire() {
  float avgTemp = (t1 + t2 + t3 + t4) / 4.0;

  bool highDeviation = (abs(t1-avgTemp) > 5.0) || (abs(t2-avgTemp) > 5.0) || (abs(t3-avgTemp) > 5.0) || (abs(t4-avgTemp) > 5.0);
  bool highTemp = avgTemp > 50.0;         // Suhu di atas 50°C
  bool lowHumidity = humid < 30.0;        // Kelembapan di bawah 30%
  bool highLPG = lpg > 400.0;             // Nilai ADC atau ppm indikatif
  bool highCO = co > 300.0;               // Nilai ADC atau ppm indikatif

  // Deteksi kebakaran jika setidaknya 3 dari 4 kondisi terpenuhi
  int fireIndicators = 0;
  if (highTemp) fireIndicators++;
  if (lowHumidity) fireIndicators++;
  if (highLPG) fireIndicators++;
  if (highCO) fireIndicators++;
  if (highDeviation) fireIndicators++;

  return fireIndicators >= 3;
}

int classifyCondition() {
  float avgTemp = (t1 + t2 + t3 + t4) / 4.0;
  int score = 0;
  if (avgTemp > 50) score++;
  if (humid < 30) score++;
  if (lpg > 400) score++;
  if (co > 300) score++;
  if ((abs(t1-avgTemp) > 5.0) || (abs(t2-avgTemp) > 5.0) || (abs(t3-avgTemp) > 5.0) || (abs(t4-avgTemp) > 5.0)) score++;

  if (score >= 4) return 3; // Kebakaran
  else if (score == 3) return 2; // Bahaya
  else if (score == 2) return 1; // Waspada
  else return 0; // Normal
}

void sendDataToServer() {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin("http://localhost/api/sensor/reading");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", TOKEN);

  int kondisi = classifyCondition();

  String json = "{";
  json += "\"device_id\": \"ce6041de-2860-4e34-b968-85df3dd9d8f3\",";
  json += "\"SID\": \"" + SID + "\",";
  json += "\"TP1\": " + String(t1, 2) + ",";
  json += "\"TP2\": " + String(t2, 2) + ",";
  json += "\"TP3\": " + String(t3, 2) + ",";
  json += "\"TP4\": " + String(t4, 2) + ",";
  json += "\"HMD\": " + String(humid, 2) + ",";
  json += "\"PRS\": " + String(press, 2) + ",";
  json += "\"CMO\": " + String(co, 0) + ",";
  json += "\"GAS\": " + String(lpg, 0) + ",";
  json += "\"LAT\": " + String(latitude, 6) + ",";
  json += "\"LON\": " + String(longitude, 6) + ",";
  json += "\"ALT\": " + String(altitude, 2) + ",";
  json += "\"KND\": " + String(kondisi);
  json += "}";

  int httpResponseCode = http.POST(json);
  Serial.print("[HTTP] Status: ");
  Serial.println(httpResponseCode);
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Response: " + response);
  }

  http.end();
}
