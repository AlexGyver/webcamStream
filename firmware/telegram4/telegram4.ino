#define WIFI_SSID ""
#define WIFI_PASS ""
#define BOT_TOKEN ""
#define CHAT_ID "-1001261708053"

#include "abitmaps.h"

#define MAX_SPI_SPEED 500000
#include "GyverMAX7219.h"
#define AM_W 12*8
#define AM_H 8*8
MAX7219 < 5, AM_W / 8, AM_H / 8 > mtrx; // CS, W, H

#include <ESP8266WiFi.h>
#include "FastBot.h"
FastBot bot(BOT_TOKEN, 8, 10000, 200);

#define DEBUGLN(x) Serial.println(x)
#define DEBUG(x) Serial.print(x)

void setup() {
  mtrx.update();
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");

  bot.setChatID(CHAT_ID);
  bot.attach(parseHandler);
}

int parseInts(String& buf, int* data, int len, char div = ',', char ter = NULL) {
  int b = 0, c = 0;
  data[b] = 0;
  while (true) {
    if (buf[c] == div) {
      b++;
      c++;
      if (b == len) return b;
      data[b] = 0;
      continue;
    }
    if (buf[c] == ter || b == len) return b + 1;
    data[b] *= 10;
    data[b] += buf[c] - '0';
    c++;
  }
}

void loop() {
  bot.tick();

  static uint32_t tmr2, tmr3, tmr4;
  if (millis() - tmr4 > 2 * 3600 * 1000ul) {
    tmr4 = millis();
    //mtrx.clear();
  }
  if (millis() - tmr2 > 5000) {
    tmr2 = millis();
    mtrx.begin();
    mtrx.update();
  }
  if (millis() - tmr3 > 500) {
    tmr3 = millis();
    mtrx.update();
  }
  /*if (Serial.available()) {
    String str = Serial.readString();
    int data[5];
    parseInts(str, data, 5);
    mtrx.rect(data[0], data[1], data[2], data[3], data[4]);
    }*/

  if (Serial.available()) {
    String str = Serial.readString();
    String arr[3];
    int startPos = 0;
    for (int i = 0; i < 3; i++) {
      int endPos = str.indexOf('\b', startPos);
      if (endPos < 0) endPos = str.length();
      String data = str.substring(startPos, endPos);
      arr[i] = data;
      startPos = endPos + 1;
    }
    int amount = arr[0].toInt();
    static bool flag = 0;
    if (amount == 50) {
      int posX = random(AM_W / 2, AM_W - 32);
      int posY = random(0, AM_H - 27 - 8);
      mtrx.drawBitmap(posX, posY, gyverlogo_32x27, 32, 27, 0, GFX_REPLACE);
      mtrx.setCursor(posX + 16 - arr[1].length() * 6 / 2, posY + 27);
      mtrx.print(arr[1]);
    } else if (amount == 100) {
      int len = arr[2].length() * 6;
      int posX = 0;
      if (len < AM_W) posX = random(0, AM_W - len);
      int posY = random(0, AM_H - 8);
      mtrx.setCursor(posX, posY);
      mtrx.invertText(flag = !flag);
      mtrx.print(arr[2]);
      if (flag) {
        mtrx.fastLineV(posX - 1, posY - 1, posY + 8);
        mtrx.fastLineH(posY - 1, posX - 1, posX + len);
      }
    } else {
      switch (amount) {
        case 10: drawImage(impostor_17x21, 17, 21); break;
        case 20: drawImage(cat_24x24, 24, 24); break;
        case 25: drawImage(catface_24x21, 24, 21); break;
        case 35: drawImage(starwars_29x32, 29, 32); break;
        case 40: drawImage(doggo_26x39, 26, 39); break;
        case 55: drawImage(picachu2_43x33, 43, 33); break;
        case 60: drawImage(nyancat_47x30, 47, 30); break;
        case 80: drawImage(picachu_38x41, 38, 41); break;
        case 90: drawImage(girl_50x50, 50, 50); break;
        //case 120: drawImage(pirate_57x42, 57, 42); break;
        //case 150: drawImage(trollface_59x47, 59, 47); break;
        //case 170: drawImage(gyver_53x64, 53, 64); break;
        //case 200: drawImage(arduino_92x64, 92, 64); break;
        case 500: mtrx.clear(); break;
      }
    }
  }
}

void drawImage(const uint8_t *frame, int width, int height) {
  int posX = random(AM_W / 2 - 2, AM_W - width + 1);
  int posY = random(0, AM_H - height);
  mtrx.drawBitmap(posX, posY, frame, width, height, 0, GFX_REPLACE);
}

void parseHandler(String& str) {
  if (str.length() <= 12) {
    DEBUGLN(str);
    byte mode = 2;
    if (str.startsWith("/set ")) mode = 1;
    else if (str.startsWith("/clr ")) mode = 0;
    if (mode == 2) return;

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
      /*Serial.print(mode);
        Serial.print(',');
        Serial.print(pos[0]);
        Serial.print(',');
        Serial.println(pos[1]);*/
      mtrx.dot(pos[0], pos[1], mode);
    }
  }
}
