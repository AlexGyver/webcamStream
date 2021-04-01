#define MAX_SPI_SPEED 500000
#include "GyverMAX7219.h"

// CLK 13, DAT 11
#define AM_W 4
#define AM_H 2
MAX7219<9, AM_W, AM_H, 0> mtrx;  // CS, W, H, buf

#include "microUART.h"
microUART uart;

void setup() {
  uart.begin(100000);
  mtrx.begin();
  mtrx.setBright(0);
  /*
    for (;;) {
      mtrx.begin();
      mtrx.restartStream();
      for (int i = 0; i < AM_W * AM_H * 8; i++) {
        mtrx.nextByte(0b00011000);
        delayMicroseconds(1000);
      }
      mtrx.restartStream();
      for (int i = 0; i < AM_W * AM_H * 8; i++) {
        mtrx.nextByte(0);
        delayMicroseconds(1000);
      }
      mtrx.clear();
    }
  */
}
byte counter = 0;
void loop() {
  if (uart.available()) {      // если есть что на приём
    byte data = uart.read();   // прочитать
    if (data == 1) {
      mtrx.restartStream();
      if (!(++counter & 0b111)) mtrx.begin();
    }
    else if (counter & 0b111) mtrx.nextByte(data);
  }
}
