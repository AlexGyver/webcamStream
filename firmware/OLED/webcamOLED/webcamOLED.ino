#define USE_MICRO_WIRE
#include <GyverOLED.h>
GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;
#include "microUART.h"
microUART uart;
bool startup = false;

void setup() {  
  uart.begin(500000);
  oled.init();  
  Wire.setClock(800000);
  oled.sendCommand(OLED_ADDRESSING_MODE, OLED_HORIZONTAL);
  oled.clear();
}

void loop() {
  if (uart.available()) {      // если есть что на приём
    byte data = uart.read();   // прочитать
    if (data == 1) {           // синхронизация
      if (!startup) oled.endData();
      startup = 1;
      oled.setWindow(0, 0, 127, 7);
      oled.beginData();
    } else {
      if (startup) oled.writeOptimised(data);
    }
  }
}
