#ifndef RS485_COMM_H
#define RS485_COMM_H

#include <Arduino.h>

// === RS485 dengan MAX485 (half-duplex) ===
// DE (Driver Enable) → HIGH untuk kirim
// RE (Receiver Enable) → LOW untuk menerima

class RS485Comm {
  private:
    HardwareSerial& serialPort;
    int dePin;
    int rePin;
    unsigned long baudRate;

  public:
    RS485Comm(HardwareSerial& port = Serial1, int de = 32, int re = 33, unsigned long baud = 9600)
      : serialPort(port), dePin(de), rePin(re), baudRate(baud) {}

    void begin() {
      pinMode(dePin, OUTPUT);
      pinMode(rePin, OUTPUT);
      setReceiveMode(); // default ke mode terima
      serialPort.begin(baudRate);
    }

    void setTransmitMode() {
      digitalWrite(dePin, HIGH);
      digitalWrite(rePin, HIGH);
      delayMicroseconds(10);
    }

    void setReceiveMode() {
      digitalWrite(dePin, LOW);
      digitalWrite(rePin, LOW);
      delayMicroseconds(10);
    }

    void send(const String& data) {
      setTransmitMode();
      serialPort.print(data);
      serialPort.flush();
      setReceiveMode();
    }

    void send(const uint8_t* data, size_t len) {
      setTransmitMode();
      serialPort.write(data, len);
      serialPort.flush();
      setReceiveMode();
    }

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

    void ping() {
      send("PING\n");
    }

    void pong() {
      send("PONG\n");
    }
};

#endif
