#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include <Arduino.h>
#include <TinyGPS++.h>

/*
  GPSModule
  --------------
  Modul ini menggabungkan TinyGPS++ dengan objek HardwareSerial.
  Pastikan kamu menghubungkan modul GPS NEO-6M ke salah satu port hardware serial ESP32
  (misalnya Serial1, Serial2) dan atur pin sesuai dengan board ESP32 kamu.

  Cara penggunaan:
  1. Di file utama, sertakan header ini: #include "gps_module.h"
  2. Buat objek GPSModule, misalnya: 
       GPSModule gps(Serial1);
  3. Inisialisasi serial GPS di setup(), misalnya: gps.begin(9600);
  4. Di loop(), panggil gps.update() secara terus-menerus agar data terproses.
  5. Ambil data seperti koordinat, altitudo, atau jumlah satelit melalui fungsi yang ada.
*/

class GPSModule {
public:
  // Instance TinyGPS++ untuk parsing data GPS
  TinyGPSPlus gps;

private:
  // Pointer ke port serial yang digunakan untuk komunikasi GPS
  HardwareSerial* gpsSerial;

public:
  // Constructor: menerima referensi ke HardwareSerial (misal, Serial1)
  GPSModule(HardwareSerial &serialPort) : gpsSerial(&serialPort) {}

  // Inisialisasi port serial untuk komunikasi dengan modul GPS
  void begin(unsigned long baudRate = 9600) {
    gpsSerial->begin(baudRate, SERIAL_8N1, 26, 27);
  }

  // Update data GPS: panggil fungsi ini di dalam loop() agar data baru selalu diproses
  void update() {
    while (gpsSerial->available()) {
      char c = gpsSerial->read();
      gps.encode(c);
    }
  }

  // Cek apakah GPS memiliki data lokasi yang valid
  bool hasValidLocation() {
    return gps.location.isValid();
  }

  // Ambil nilai latitude. Jika data tidak valid, akan bernilai 0.0
  double getLatitude() {
    return gps.location.lat();
  }

  // Ambil nilai longitude. Jika data tidak valid, akan bernilai 0.0
  double getLongitude() {
    return gps.location.lng();
  }

  // Ambil nilai altitude (meter)
  double getAltitude() {
    return gps.altitude.meters();
  }

  // Ambil jumlah satelit yang digunakan GPS
  int getSatellites() {
    return gps.satellites.value();
  }

  // Fungsi debug: mencetak status data GPS ke Serial monitor
  void printStatus() {
    Serial.print("Latitude: ");
    Serial.println(getLatitude());
    Serial.print("Longitude: ");
    Serial.println(getLongitude());
    Serial.print("Altitude: ");
    Serial.print(getAltitude());
    Serial.println(" m");
    Serial.print("Satellites: ");
    Serial.println(getSatellites());
  }
};

#endif

/*
#include <Arduino.h>
#include "gps_module.h"

// Misalnya gunakan Serial1 untuk GPS, pastikan pin TX/RX sesuai dengan wiring pada ESP32 kamu.
GPSModule gps(Serial1);

void setup() {
  Serial.begin(115200);
  // Inisialisasi serial untuk GPS dengan baud rate 9600 (umum untuk NEO-6M)
  gps.begin(9600);
  Serial.println("GPS Module siap...");
}

void loop() {
  // Baca data GPS secara terus-menerus
  gps.update();
  
  // Jika data lokasi valid, cetak informasinya
  if (gps.hasValidLocation()) {
    gps.printStatus();
  }
  
  delay(1000);
}

*/