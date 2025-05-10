#ifndef LEDMAT_H
#define LEDMAT_H

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <deque>

// ==== Konfigurasi Hardware ====
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define DATA_PIN    23  // MOSI
#define CLK_PIN     18  // SCK
#define CS_PIN       5  // SS / LOAD
#define MAX_DEVICES   4

// ==== Inisialisasi Parola ====
MD_Parola ledmat = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// ==== Konfigurasi Display ====
const uint16_t SCROLL_SPEED = 75;
std::deque<String> msgQueue;
bool animasiAktif = false;

// ==== Fungsi untuk Menambahkan Pesan ke Antrian ====
void displayLEDMAT(const String& msg) {
  msgQueue.push_back(msg);
}

// ==== Fungsi untuk Menampilkan Animasi (dipanggil di loop) ====
void updateLEDMAT() {
  if (!animasiAktif && !msgQueue.empty()) {
    String nextMsg = msgQueue.front();
    msgQueue.pop_front();

    ledmat.displayScroll(nextMsg.c_str(), PA_CENTER, PA_SCROLL_LEFT, SCROLL_SPEED);
    ledmat.displayReset();
    animasiAktif = true;
  }

  if (animasiAktif && ledmat.displayAnimate()) {
    animasiAktif = false;
  }
}

// ==== Fungsi Setup untuk Inisialisasi ====
void initLEDMAT() {
  ledmat.begin();
  ledmat.setIntensity(5);  // Kecerahan (0–15)
  ledmat.displayClear();
}

#endif
