int w = 128;
int h = 64;
int rate = 30;
int baud = 500000;

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
byte buf[] = new byte[1024];
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
  int counter = 0;  
  for (int j = 0; j < h/8; j++) {
    for (int i = 0; i < w; i++) {
      byte thisByte = 0;
      for (int k = 0; k < 8; k++) {      
        //if ((byte)frame.pixels[i + (j*8+k)*w + ov*w] != 0) thisByte |= 1 << k;
        if ((byte)get(100+i,(j*8+k)) != 0) thisByte |= 1 << k;
      }
      if (thisByte == 1) thisByte++;
      buf[counter] = thisByte;
      counter++;
    }
  }
  //frame.updatePixels();
  myPort.write(buf);
}
