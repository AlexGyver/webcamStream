#define RGB_PACK 0  // 0 - RGB16, 1 - RGB8

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#define TFT_CS  10
#define TFT_RST 8
#define TFT_DC  9
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

#include "microUART.h"
microUART uart;
void setup() {
  uart.begin(2000000);
  tft.initR(INITR_BLACKTAB);                // инициализация
  tft.setRotation(tft.getRotation() + 3);   // крутим дисп
  tft.fillScreen(ST7735_BLACK);             // чисти чисти

  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  digitalWrite(TFT_CS, 0);
}


void loop() {
  /*fill(ST7735_YELLOW >> 8, ST7735_YELLOW);
    delay(300);
    fill(ST7735_GREEN >> 8, ST7735_GREEN);
    delay(300);*/

  if (uart.available()) {      // если есть что на приём
    byte data = uart.read();   // прочитать
    if (data == 1) {           // синхронизация
      home();
      startSend();
    } else {
      if (RGB_PACK == 0) SPI.transfer(data);
      else {
        SPI.transfer((data & 0b11100000) | ((data & 0b00011000) >> 2));
        SPI.transfer((data & 0b00000111) << 2);
      }
    }
  }
}

void fill(byte colorH, byte colorL) {
  home();
  digitalWrite(TFT_CS, 0);
  digitalWrite(TFT_DC, 1);
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  for (int x = 0; x < 160 * 128; x++) {
    SPI.transfer(colorH);
    SPI.transfer(colorL);
  }
  SPI.endTransaction();
  digitalWrite(TFT_CS, 1);
}

void home() {
  digitalWrite(TFT_DC, 0);
  SPI.transfer(ST7735_CASET); // Column addr set
  SPI.transfer(0);
  SPI.transfer(0);   // XSTART
  SPI.transfer(0);
  SPI.transfer(160);   // XEND
  SPI.transfer(ST7735_RASET); // Row addr set
  SPI.transfer(0);
  SPI.transfer(0);   // YSTART
  SPI.transfer(0);
  SPI.transfer(128);   // YEND
  SPI.transfer(ST7735_RAMWR); // write to RAM
}
void startSend() {
  digitalWrite(TFT_DC, 1);
}
void setWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
  digitalWrite(TFT_CS, 0);
  digitalWrite(TFT_DC, 0);
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  SPI.transfer(ST7735_CASET); // Column addr set
  SPI.transfer(0x00);
  SPI.transfer(x0);   // XSTART
  SPI.transfer(0x00);
  SPI.transfer(x1);   // XEND

  SPI.transfer(ST7735_RASET); // Row addr set
  SPI.transfer(0x00);
  SPI.transfer(y0);   // YSTART
  SPI.transfer(0x00);
  SPI.transfer(y1);   // YEND

  SPI.transfer(ST7735_RAMWR); // write to RAM
  SPI.endTransaction();
  digitalWrite(TFT_CS, 1);
}
