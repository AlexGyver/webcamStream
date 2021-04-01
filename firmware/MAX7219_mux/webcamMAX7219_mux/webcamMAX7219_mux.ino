#define MAX_SPI_SPEED 2000000
#include "GyverMAX7219.h"

// CLK 13, DAT 11
#define AM_W 24
#define AM_H 16
MAX7219<9, AM_W, AM_H> mtrx;  // CS, W, H, buf

#include "microUART.h"
microUART uart;

void setup() {
  uart.begin(200000);
  //test();
}
byte counter = 0;
void loop() {
  if (uart.available()) {      // если есть что на приём
    byte data = uart.read();   // прочитать
    if (data == 1) {
      mtrx.restartStream();
      if (!(++counter & 0b111111)) mtrx.begin();
    }
    else if (counter & 0b111111) mtrx.nextByte(data);
  }
}

void test() {
  for (;;) {
    mtrx.clear();
    byte radius = 3;
    static int x = (AM_W * 8 / 2) * 10, y = (AM_H * 8 / 2) * 10;
    static int velX = 15, velY = 21;
    static bool fillFlag = 0;
    x += velX;
    y += velY;
    if (x >= (AM_W * 8 - radius) * 10 || x < radius * 10) {
      velX = -velX;
      fillFlag = !fillFlag;
    }
    if (y >= (AM_H * 8 - radius) * 10 || y < radius * 10) {
      velY = -velY;
      fillFlag = !fillFlag;
    }

    mtrx.circle(x / 10, y / 10, radius, fillFlag ? GFX_STROKE : GFX_FILL);
    mtrx.update();
    }
  /*for (;;) {
    const byte amount = 10;
    const byte radius = 4;
    static bool start = false;
    static int x[amount], y[amount];
    static int velX[amount], velY[amount];
    if (!start) {
      start = 1;
      for (byte i = 0; i < amount; i++) {
        x[i] = random(10, (AM_W*8 - 1) * 10);
        y[i] = random(10, (AM_H*8 - 1) * 10);
        velX[i] = random(10, 20);
        velY[i] = random(10, 20);
      }
    }
    mtrx.clear();

    for (byte i = 0; i < amount; i++) {
      x[i] += velX[i];
      y[i] += velY[i];
      if (x[i] >= (AM_W*8 - radius) * 10 || x[i] < 0) velX[i] = -velX[i];
      if (y[i] >= (AM_H*8 - radius) * 10 || y[i] < 0) velY[i] = -velY[i];
      mtrx.circle(x[i] / 10, y[i] / 10, radius, GFX_FILL);
    }
    mtrx.update();
    delay(30);
  }*/

  for (;;) {
    mtrx.begin();
    mtrx.clear();
    for (int i = 0; i < 24 * 8; i++) {
      mtrx.clear();
      mtrx.line(i, 0, i, AM_H * 8);
      mtrx.update();
      //delay(10);
      }
    /*for (int i = 0; i < 24 * 8; i++) {
      mtrx.clear();
      mtrx.dot(i, 16);
      mtrx.update();
      }*/
    //mtrx.line(0, 0, AM_W * 8 - 1, AM_H * 8 - 1);
    /*int offs = 24 * 16;
      for (int i = 0 + offs; i < 24 * 2 + offs; i++) {
      mtrx.buffer[i] = 0b00011000;
      }
      for (int i = 24 * 2 + offs; i < 24 * 4 + offs; i++) {
      mtrx.buffer[i] = 0b00100100;
      }
      for (int i = 24 * 4 + offs; i < 24 * 6 + offs; i++) {
      mtrx.buffer[i] = 0b01000010;
      }*/
    mtrx.update();
    delay(100);
  }
  /*byte val;
    for (;;) {
    mtrx.begin();
    mtrx.fillMatrix(bit(val));
    val = ++val & 0b111;
    }*/
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
