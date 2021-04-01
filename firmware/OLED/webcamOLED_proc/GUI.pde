void GUIinit() {
  cp5 = new ControlP5(this); 
  cp5.addSlider("overlap").setCaptionLabel("overlap").setPosition(0, 50).setSize(100, 25).setRange(0, 150).setValue(0);
  cp5.getController("overlap").getCaptionLabel().setPaddingX(-40);

  cp5.addButton("start_stream").setCaptionLabel("start stream").setPosition(0, 75).setSize(100, 25);
  cp5.addButton("stop_stream").setCaptionLabel("stop stream").setPosition(0, 100).setSize(100, 25);
  cp5.addButton("start_send").setCaptionLabel("start send").setPosition(0, 125).setSize(100, 25);
  cp5.addButton("stop_send").setCaptionLabel("stop send").setPosition(0, 150).setSize(100, 25);

  // лист СAМ
  cam_list = cp5.addScrollableList("CAM")
    .setCaptionLabel("CAMERA")
    .setPosition(0, 25)
    .setSize(100, 100)
    .setBarHeight(25)
    .setItemHeight(25)
    .close();
  cam_list.onEnter(new CallbackListener() {
    public void controlEvent(CallbackEvent ev) {
      cam_list.clear();
      cam_list.addItems(Arrays.asList(Capture.list()));
    }
  }
  );
  // лист СОМ
  com_list = cp5.addScrollableList("COM")
    .setCaptionLabel("PORT")
    .setPosition(0, 0)
    .setSize(100, 100)
    .setBarHeight(25)
    .setItemHeight(25)
    .close();
  com_list.onEnter(new CallbackListener() {
    public void controlEvent(CallbackEvent ev) {
      com_list.clear();
      com_list.addItems(Arrays.asList(Serial.list()));
    }
  }
  );
}

void overlap (int n) {
  ov = n;
  background(130);
  if (streamStatus) start_stream();
}

void COM(int n) {
  curPort = Serial.list()[n];
}
void CAM(int n) {
  curCam = Capture.list()[n];
}

void start_stream() {
  if (curCam != null) {
    frame = createImage(w, h+ov, RGB);
    cam = new Capture(this, w, h+ov, curCam, rate);
    cam.start();
    streamStatus = true;
  }  
}
void stop_stream() {
  if (curCam != null) {
    cam.stop();
    streamStatus = false;
  }
}
void start_send() {
  if (!COMstatus) {
    myPort = new Serial(this, curPort, baud);
    COMstatus = true;
  }
}
void stop_send() {
  myPort.stop();
  COMstatus = false;
}
