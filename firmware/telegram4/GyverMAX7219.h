#ifndef GyverMAX7219_h
#define GyverMAX7219_h
// самая резкая библиотека для матриц MAX7219 на диком западе

#include <Arduino.h>
#include <SPI.h>
#include "GyverGFX.h"
#include "FastIO.h"

#ifndef MAX_SPI_SPEED
#define MAX_SPI_SPEED 1000000
#endif

template <byte CSpin, byte width, byte height, byte buf = 1>
class MAX7219 : public GyverGFX {
  public:
    MAX7219() : GyverGFX(width * 8, height * 8) {
      begin();
    }
    ~MAX7219() {
      SPI.end();
      SPI.endTransaction();
    }
    void begin() {
      pinMode(CSpin, OUTPUT);
      SPI.begin();
      SPI.beginTransaction(SPISettings(MAX_SPI_SPEED, MSBFIRST, SPI_MODE0));
      sendCMD(0x0f, 0x00);  // отключить режим теста
      sendCMD(0x09, 0x00);  // выключить декодирование
      sendCMD(0x0a, 0x00);  // яркость
      sendCMD(0x0b, 0x0f);  // отображаем всё
      sendCMD(0x0C, 0x01);  // включить
      //clear();
    }
    void setBright(byte value) {  // 8x8: 0/8/15 - 30/310/540 ma
      sendCMD(0x0a, value);  // яркость 0-15
    }
    void setPower(bool value) {
      sendCMD(0x0c, value);
    }
    void clear() {
      fillMatrix(0);
    }
    void fill() {
      fillMatrix(255);
    }
    void fillMatrix(byte data) {
      if (buf) {
        for (int i = 0; i < width * height * 8; i++) buffer[i] = data;
      } else {
        for (int k = 7; k >= 0; k--) {
          beginData();
          for (int i = 0; i < _amount; i++) sendByte(k, data);
          endData();
        }
      }
    }
    void sendByte(uint8_t address, uint8_t value) {
      SPI.transfer(8 - address);
      SPI.transfer(value);
    }
    void dot(int x, int y, byte fill = 1) {
      if (buf && x >= 0 && x < width * 8 && y >= 0 && y < height * 8) {
        if ((y >> 3) & 1) {               // если это нечётная матрица: (y / 8) % 2
          x = width * 8 - 1 - x;          // отзеркалить x
          y = (y & 0xF8) + (7 - (y & 7)); // отзеркалить y: (y / 8) * 8 + (7 - (y % 8));
        }
        // позиция в буфере
        int curByte = width * (height - 1 - (y >> 3)) + (width - 1 - (x >> 3)) + (y & 7) * width * height;
        bitWrite(buffer[curByte], x & 7, fill);
      }
    }
    void update() {
      if (buf) {
        int count = 0;
        for (int k = 0; k < 8; k++) {
          beginData();
          for (int i = 0; i < _amount; i++) sendByte(k, buffer[count++]);
          endData();
        }
      }
    }

    // stream
    void restartStream() {
      _row = 0;
      _count = 0;
    }
    void nextByte(byte data) {
      if (_row == 8) return;
      if (_count == 0) beginData();
      sendByte(_row, data);
      if (++_count == _amount) {
        endData();
        _count = 0;
        _row++;
      }
    }
    byte buffer[width * height * 8 * buf];
  private:
    void beginData() {
      fastWrite(CSpin, 0);
    }
    void endData() {
      fastWrite(CSpin, 1);
    }
    void sendCMD(uint8_t address, uint8_t value) {
      beginData();
      for (int i = 0; i < _amount; i++) {
        SPI.transfer(address);
        SPI.transfer(value);
      }
      endData();
    }

    const int _amount = width * height;
    int _row = 0, _count = 0;
};
#endif
