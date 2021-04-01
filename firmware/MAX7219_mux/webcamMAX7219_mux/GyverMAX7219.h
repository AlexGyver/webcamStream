#define MUX_S0 5
#define MUX_S1 6
#define MUX_S2 7
#define MUX_S3 8
#define MUX_EN 10

#define HC595_CLK 4
#define HC595_DAT 2
#define HC595_CS 3

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

class MUX {
  public:
    void init(byte s0, byte s1, byte s2, byte s3, byte en) {
      _s0 = s0;
      _s1 = s1;
      _s2 = s2;
      _s3 = s3;
      _en = en;
      pinMode(_s0, OUTPUT);
      pinMode(_s1, OUTPUT);
      pinMode(_s2, OUTPUT);
      pinMode(_s3, OUTPUT);
      pinMode(_en, OUTPUT);
      clear();
    }
    void setOut(byte num) {
      clear();
      fastWrite(_s0, num & 1);
      num >>= 1;
      fastWrite(_s1, num & 1);
      num >>= 1;
      fastWrite(_s2, num & 1);
      num >>= 1;
      fastWrite(_s3, num & 1);
      fastWrite(_en, 0);
    }
    void clear() {
      fastWrite(_en, 1);
    }
  private:
    byte _s0, _s1, _s2, _s3, _en;
};

class HC595 {
  public:
    void init(byte clk, byte cs, byte dat) {
      _clk = clk;
      _cs = cs;
      _dat = dat;
      pinMode(_clk, 1);
      pinMode(_cs, 1);
      pinMode(_dat, 1);
      fastWrite(_cs, 0);
      fastShiftOut(_dat, _clk, MSBFIRST, 0xff);
      fastWrite(_cs, 1);
    }
    void setCS(byte num) {
      fastWrite(_cs, 0);
      fastShiftOut(_dat, _clk, MSBFIRST, 0xff & ~bit(num));
      fastWrite(_cs, 1);
    }
    void clear() {
      fastWrite(_cs, 0);
      fastShiftOut(_dat, _clk, MSBFIRST, 0xff);
      fastWrite(_cs, 1);
    }
  private:
    byte _clk = 0, _cs = 0, _dat = 0;
};

template <byte CSpin, byte width, byte height, byte buf = 1>
class MAX7219 : public GyverGFX {
  public:
    MAX7219() {
      begin();
    }
    ~MAX7219() {
      SPI.end();
      SPI.endTransaction();
    }
    void begin() {
      mux.init(MUX_S0, MUX_S1, MUX_S2, MUX_S3, MUX_EN);
      CSsw.init(HC595_CLK, HC595_CS, HC595_DAT);

      pinMode(CSpin, OUTPUT);
      SPI.begin();
      SPI.beginTransaction(SPISettings(MAX_SPI_SPEED, MSBFIRST, SPI_MODE0));
      sendCMD(0x0f, 0x00);  // отключить режим теста
      sendCMD(0x09, 0x00);  // выключить декодирование
      sendCMD(0x0a, 0x00);  // яркость
      sendCMD(0x0b, 0x0f);  // отображаем всё
      sendCMD(0x0C, 0x01);  // включить
      clear();
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
          beginData(0);
          int prevI = 0;
          for (int i = 0; i < _amount; i++) {
            if (i != prevI) {
              prevI = i;
              int num = i / (width * 2);   // каждые 2 строки
              beginData(num);
            }
            sendByte(k, data);
          }
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
        int yy = y;
        y &= 15;
        if ((y >> 3) & 1) {               // если это нечётная матрица: (y / 8) % 2
          x = width * 8 - 1 - x;          // отзеркалить x
          y = (y & 0xF8) + (7 - (y & 7)); // отзеркалить y: (y / 8) * 8 + (7 - (y % 8));
        }
        // позиция в буфере
        int curByte = width * (2 - 1 - (y >> 3)) + (width - 1 - (x >> 3)) + (y & 7) * width * 2;
        curByte += width * 16 * (yy / 16);
        bitWrite(buffer[curByte], x & 7, fill);
      }
    }
    void update() {
      if (buf) {
        int count = 0;
        for (int h = 0; h < height / 2; h++) {
          for (int k = 0; k < 8; k++) {
            beginData(h);
            for (int w = 0; w < width * 2; w++) {
              sendByte(k, buffer[count++]);
            }
            endData();
          }
        }
        /*
                int count = 0;
                for (int k = 0; k < 8; k++) {
                  beginData(0);
                  int prevI = 0;
                  for (int i = 0; i < _amount; i++) {
                    if (i != prevI) {
                      prevI = i;
                      int num = i / (width * 2);   // каждые 2 строки
                      beginData(num);
                    }
                    sendByte(k, buffer[count++]);
                  }
                  endData();
                }*/
      }
    }

    // stream
    void restartStream() {
      _row = 0;
      _count = 0;
      _line = 0;
    }
    void nextByte(byte data) {
      if (_line == height / 2) return;
      if (_count == 0) beginData(_line);
      sendByte(_row, data);
      if (++_count == width * 2) {
        endData();
        _count = 0;
        _row++;
        if (_row == 8) {
          _row = 0;
          _line++;          
        }
      }
      /*if (_row == 8) return;
        if (_count == 0) beginData();
        sendByte(_row, data);
        if (++_count == _amount) {
        endData();
        _count = 0;
        _row++;
        }*/
    }
    byte buffer[width * height * 8 * buf];
    HC595 CSsw;
    MUX mux;
  private:
    void beginData(byte num) {
      //fastWrite(CSpin, 0);
      //num += 2;
      mux.setOut(num);
      CSsw.setCS(num);
      //delayMicroseconds(10);
    }
    void endData() {
      //fastWrite(CSpin, 1);
      mux.clear();
      CSsw.clear();
      //delayMicroseconds(10);
    }
    void sendCMD(uint8_t address, uint8_t value) {
      beginData(0);
      int prevI = 0;
      for (int i = 0; i < _amount; i++) {
        if (i != prevI) {
          prevI = i;
          int num = i / (width * 2);   // каждые 2 строки
          beginData(num);
        }
        SPI.transfer(address);
        SPI.transfer(value);
      }
      endData();
    }

    const int _amount = width * height;
    int _row = 0, _count = 0, _line = 0;
};
#endif
