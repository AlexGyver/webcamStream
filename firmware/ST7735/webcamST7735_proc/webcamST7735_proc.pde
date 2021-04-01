int w = 160;
int h = 128;
int baud = 2000000;
byte pack = 2;  // 2 - RGB16, 1 - RGB8

// установить либу video
import processing.video.*;
PImage frame;
Capture cam;

// P5
import controlP5.*;
ControlP5 cp5;
ScrollableList com_list, cam_list;
String curCam = "";

// JAVA
import processing.serial.*;
import java.util.*;
Serial myPort;

String curPort = "";
boolean streamStatus = false;
boolean COMstatus = false;
int ov = 0;
byte buf[] = new byte[128*160*pack];

void setup() {  
  size(400, 200);
  GUIinit();
}

void draw() {    
  if (streamStatus) {
    if (cam.available()) {
      cam.read();      
      frame = cam;
      //ditherImage(frame);
      background(130);
      image(frame, 100, -ov);
      if (COMstatus) sendImage();
    }
  } else {
    background(130);
  }
}

void sendImage() {
  myPort.write(1);
  int count = 0;
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      int col = get(100+i, j);
      // 2 байта
      if (pack == 2) {
        col = ( ((col & 0xF80000) >> 8) | ((col & 0xFC00) >> 5) | ((col & 0xF8) >> 3) );  // RGB24 to RGB16
        byte col1 = byte((col >> 8) & 0xFF);
        byte col2 = byte(col & 0xFF);
        if (col1 == 1) col1++;
        if (col2 == 1) col2++;
        buf[count++] = col1;
        buf[count++] = col2;
        
        // 1 байт
      } else if (pack == 1) {
         col = ( ((col & 0xE00000) >> 16) | ((col & 0xC000) >> 11) | ((col & 0xE0) >> 5) );  // RGB24 to RGB8
         byte col1 = byte(col & 0xFF);
         if (col1 == 1) col1++;
         buf[count++] = col1;
      }
    }
  }
  myPort.write(buf);
}
