// константы
#define WIFI_SSID ""
#define WIFI_PASS ""
#define BOT_TOKEN ""
#define BOT_NAME ""
#define BOT_USERNAME ""
#define CHAT_ID ""

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "ESP8266TelegramBOT.h"
TelegramBOT bot(BOT_TOKEN, BOT_NAME, BOT_USERNAME);

#define MAX_SPI_SPEED 500000
#include "GyverMAX7219.h"
#define AM_W 12*8
#define AM_H 8*8
MAX7219 < 5, AM_W / 8, AM_H / 8 > mtrx; // CS, W, H

void setup() {
  delay(4000);
  mtrx.update();
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");
  bot.begin();
}

void loop() {
  parseBot();
}

void parseBot() {
  static uint32_t tmr;
  static byte count = 0;
  if (millis() - tmr > 1000) {
    bot.getUpdates(bot.message[0][1]);
    for (int i = 1; i < bot.message[0][0].toInt() + 1; i++) {
      if (bot.message[i][4] == CHAT_ID && bot.message[i][5].length() <= 12) {
        String str = bot.message[i][5];
        byte mode = 2;
        if (str.startsWith("/set ")) mode = 1;
        else if (str.startsWith("/clr ")) mode = 0;
        if (mode == 2) continue;

        // супер парсинг без стрингов
        int pos[2] = {0, 0};
        int count = -1;
        for (byte i = 5; i < 12; i++) {
          char sym = str[i];
          if (sym >= 48 && sym <= 57) {
            if (count == -1) count = 0;
            pos[count] *= 10;
            pos[count] += sym - '0';
          } else if (sym == 32) {
            if (count == 0) count++;
            else mode = 2;
          } else if (sym == NULL) {
            if (count != 1) mode = 2;
            break;
          } else {
            mode = 2;
          }
        }
        if (mode != 2) {
          Serial.print(mode);
          Serial.print(',');
          Serial.print(pos[0]);
          Serial.print(',');
          Serial.println(pos[1]);
          mtrx.dot(pos[0], pos[1], mode);
          if (++count == 100) {
            count = 0;
            mtrx.begin();
          }
          mtrx.update();
        }
      }
    }
    bot.message[0][0] = "";
    tmr = millis();
  }
}
