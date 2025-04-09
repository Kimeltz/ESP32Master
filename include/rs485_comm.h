#ifndef RS485_COMM_H
#define RS485_COMM_H

#include <Arduino.h>

// === RS485 dengan MAX485 (half-duplex) ===
// DE dan RE dihubungkan ke satu pin kontrol (biasanya HIGH untuk kirim, LOW untuk terima)

class RS485Comm {
  private:
    HardwareSerial& serialPort;
    int controlPin;
    unsigned long baudRate;

  public:
    RS485Comm(HardwareSerial& port = Serial1, int ctrlPin = 32, unsigned long baud = 9600)
      : serialPort(port), controlPin(ctrlPin), baudRate(baud) {}

    void begin() {
      pinMode(controlPin, OUTPUT);
      setReceiveMode(); // default: mode terima
      serialPort.begin(baudRate);
    }

    // Mode kirim
    void setTransmitMode() {
      digitalWrite(controlPin, HIGH);
      delayMicroseconds(10); // waktu tunda kecil agar stabil
    }

    // Mode terima
    void setReceiveMode() {
      digitalWrite(controlPin, LOW);
      delayMicroseconds(10);
    }

    // Kirim data
    void send(const String& data) {
      setTransmitMode();
      serialPort.print(data);
      serialPort.flush(); // tunggu sampai selesai kirim
      setReceiveMode();
    }

    void send(const uint8_t* data, size_t len) {
      setTransmitMode();
      serialPort.write(data, len);
      serialPort.flush();
      setReceiveMode();
    }

    // Cek dan baca data masuk
    bool available() {
      return serialPort.available();
    }

    String readLine() {
      String incoming = "";
      while (serialPort.available()) {
        char c = serialPort.read();
        if (c == '\n') break;
        incoming += c;
      }
      return incoming;
    }

    int readByte() {
      return serialPort.read();
    }
};

#endif

/*
Example

#include "rs485_comm.h"

RS485Comm rs485(Serial1, 32, 9600); // Gunakan Serial1, pin kontrol DE+RE di GPIO32

void setup() {
  Serial.begin(115200);
  rs485.begin();
  Serial.println("📡 RS485 siap!");
}

void loop() {
  rs485.send("Hello dari ESP32!\n");
  delay(1000);

  if (rs485.available()) {
    String msg = rs485.readLine();
    Serial.print("📥 Terima RS485: ");
    Serial.println(msg);
  }
}

*/