#define WIFI_SSID ""
#define WIFI_PASS ""
#define BOT_TOKEN ""

#define MAX_SPI_SPEED 500000
#include "GyverMAX7219.h"
#define AM_W 12*8
#define AM_H 8*8
MAX7219 < 5, AM_W / 8, AM_H / 8 > mtrx; // CS, W, H

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
HTTPClient https;

#define DEBUGLN(x) Serial.println(x)
#define DEBUG(x) Serial.print(x)

void setup() {
  mtrx.update();
  Serial.begin(115200);
  Serial.println();
  client->setInsecure();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");
}

void loop() {
  static uint32_t tmr, tmr2, tmr3, tmr4;
  if (millis() - tmr4 > 2 * 3600 * 1000ul) {
    tmr4 = millis();
    mtrx.clear();
  }
  if (millis() - tmr3 > 500) {
    tmr3 = millis();
    mtrx.update();
  }
  if (millis() - tmr2 > 5000) {
    tmr2 = millis();
    mtrx.begin();
    mtrx.update();
  }
  if (millis() - tmr > 300) {
    tmr = millis();
    checkBot();
  }
}

void checkBot() {
  static long ID = 0;
  if (https.begin(*client, (String)"https://api.telegram.org/bot" + BOT_TOKEN + "/getUpdates?offset=" + ID + "&limit=3")) {
    if (https.GET() == HTTP_CODE_OK) {
      String json = https.getString();
      if (https.getSize() > 5000) {
        DEBUGLN("ovf");
        int IDpos = json.indexOf("{\"update_id\":", IDpos);
        if (IDpos > 0) ID = json.substring(IDpos + 13, json.indexOf(',', IDpos)).toInt();
        ID++;
        https.end();
        return;
      }
      // считаем, сколько в пакете данных (метка update_id)
      int count = 0;
      int IDpos = 0;
      while (1) {
        IDpos = json.indexOf("{\"update_id\":", IDpos);
        if (IDpos > 0) {
          ID = json.substring(IDpos + 13, json.indexOf(',', IDpos)).toInt() + 1;  // сразу ++ для следующего пакета
          IDpos++;
          count++;
        } else break;
      }

      // считаем и парсим сообщения
      if (count) {
        int textPos = 0;
        while (1) {
          textPos = json.indexOf(",\"text\":\"", textPos);
          if (textPos < 0) break;
          int endPos = json.indexOf("\"}}", textPos);
          int endPos2 = json.indexOf("\",\"entities", textPos);
          if (endPos > 0 && endPos2 > 0) endPos = min(endPos, endPos2);
          else if (endPos < 0) endPos = endPos2;
          String str = json.substring(textPos + 9, endPos);
          parseCMD(str);
          textPos = endPos;
        }
      }
    } else DEBUGLN("GET telegram failed");
    https.end();
  } else DEBUGLN("Unable to connect");
}

void parseCMD(String& str) {
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
