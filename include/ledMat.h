#ifndef LEDMAT_H
#define LEDMAT_H

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <queue>

// ==== Konfigurasi Hardware ====
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW // Ganti jika modul berbeda
#define MAX_DEVICES 4
#define DATA_PIN    23  // MOSI (VSPI)
#define CLK_PIN     18  // SCK (VSPI)
#define CS_PIN      5   // SS

// ==== Inisialisasi Parola ====
MD_Parola ledmat = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// ==== Konfigurasi Display ====
std::queue<String> msgQueue;
bool isAnimating = false;

// ==== Fungsi Tambah Pesan ====
void displayLEDMAT(const String& msg) {
  msgQueue.push(msg);
}

// ==== Fungsi Update Display ====
void updateLEDMAT() {
  if (ledmat.displayAnimate()) { // Animasi selesai
    isAnimating = false;
    ledmat.displayReset(); // Reset untuk pesan berikutnya
  }

  if (!isAnimating && !msgQueue.empty()) {
    String nextMsg = msgQueue.front();
    msgQueue.pop();

    // Gunakan displayText() untuk testing awal
    ledmat.displayText(nextMsg.c_str(), PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    isAnimating = true;
  }
}

// ==== Fungsi Inisialisasi ====
void initLEDMAT() {
  SPI.begin(CLK_PIN, -1, DATA_PIN, CS_PIN); // ESP32: CLK, MISO, MOSI, SS
  ledmat.begin();
  ledmat.setFont(NULL);
  ledmat.setIntensity(5);
  ledmat.displayClear();
  ledmat.displaySuspend(false); // Pastikan display aktif
}

#endif