int w = 24*8;
int h = 16*8;
int rate = 30;
int baud = 200000;

int W = w/8;
int H = h/8;

// video
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
byte buf[] = new byte[W*H*8];
int tmr;
int ov = 0;

void setup() {  
  size(400, 200);
  GUIinit();
}

void draw() {    
  if (streamStatus) {
    if (millis() - tmr > 1000/rate && cam.available()) {
      tmr = millis();
      cam.read();      
      frame = cam;
      ditherImage(frame);
      background(130);
      image(frame, 100, -ov);
      if (COMstatus) sendImage();
    }
  } else {
    background(130);
  }
}

void sendImage() {
  //frame.loadPixels();
  myPort.write(1);

  for (int yy = 0; yy < h; yy++) {
    for (int xx = 0; xx < w; xx+=8) {
      int x = xx;
      int y = yy;
      byte thisByte = 0;
      boolean odd = ((y / 8) % 2 == 1);
      for (int k = 0; k < 8; k++) {      
        if ((byte)get(100+x+k, y) != 0) {
          if (odd) thisByte |= 1 << (7-k);
          else thisByte |= 1 << k;
        }
      }
      if (thisByte == 1) thisByte++;
      y &= 15;
      if (odd) {          // если это нечётная матрица: (y / 8) % 2
        x = W * 8 - 1 - x;              // отзеркалить x
        y = (y / 8) * 8 + (7 - (y % 8)); // отзеркалить y: (y / 8) * 8 + (7 - (y % 8));
      }
      int curByte = W * (2 - 1 - y / 8) + (W - 1 - x / 8) + (y % 8) * W * 2;
      curByte += W * 16 * (yy / 16);
      buf[curByte] = thisByte;
    }
  }
  //frame.updatePixels();
  myPort.write(buf);
}
