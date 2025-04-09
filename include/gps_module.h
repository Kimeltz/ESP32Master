#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

class GPSModule {
  private:
    TinyGPSPlus gps;
    HardwareSerial& serialPort;
    int rxPin;
    int txPin;
    uint32_t baudRate;

  public:
    // Konstruktor: gunakan port serial, dan pin RX/TX (misal: 16 dan 17)
    GPSModule(HardwareSerial& port = Serial2, int rx = 16, int tx = 17, uint32_t baud = 9600)
      : serialPort(port), rxPin(rx), txPin(tx), baudRate(baud) {}

    // Inisialisasi serial untuk GPS
    void begin() {
      serialPort.begin(baudRate, SERIAL_8N1, rxPin, txPin);
      Serial.println("📡 GPS Module siap...");
    }

    // Panggil secara rutin di loop() untuk parsing data dari GPS
    void update() {
      while (serialPort.available()) {
        gps.encode(serialPort.read());
      }
    }

    // Cek apakah data GPS valid
    bool isValid() {
      return gps.location.isValid();
    }

    // Ambil latitude
    double latitude() {
      return gps.location.lat();
    }

    // Ambil longitude
    double longitude() {
      return gps.location.lng();
    }

    // Ambil kecepatan dalam km/h
    double speedKmph() {
      return gps.speed.kmph();
    }

    // Ambil waktu (jika tersedia)
    String getTime() {
      if (gps.time.isValid()) {
        char buffer[16];
        sprintf(buffer, "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
        return String(buffer);
      }
      return "N/A";
    }

    // Ambil tanggal (jika tersedia)
    String getDate() {
      if (gps.date.isValid()) {
        char buffer[16];
        sprintf(buffer, "%02d/%02d/%04d", gps.date.day(), gps.date.month(), gps.date.year());
        return String(buffer);
      }
      return "N/A";
    }

    // Altitude dalam meter
    double altitude() {
      return gps.altitude.meters();
    }

    // HDOP (akurasi horizontal)
    double hdop() {
      return gps.hdop.hdop();
    }

    // Satelit yang terkoneksi
    int satellites() {
      return gps.satellites.value();
    }
};

#endif


/*
Example

#include "gps_module.h"

GPSModule gps(Serial2, 16, 17, 9600); // RX, TX, Baudrate

void setup() {
  Serial.begin(115200);
  gps.begin();
}

void loop() {
  gps.update();

  if (gps.isValid()) {
    Serial.printf("📍 Lat: %.6f, Lon: %.6f\n", gps.latitude(), gps.longitude());
    Serial.printf("🕒 %s 📅 %s | Alt: %.2f m | Speed: %.2f km/h | Sats: %d\n",
                  gps.getTime().c_str(), gps.getDate().c_str(),
                  gps.altitude(), gps.speedKmph(), gps.satellites());
  } else {
    Serial.println("❌ GPS belum lock...");
  }

  delay(1000);
}
*/