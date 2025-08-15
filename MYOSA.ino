// fire_escape_guidance.ino
// Full Arduino code from earlier response with //* ... *// comment included

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_APDS9960.h>
#include <MPU6050_light.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Adafruit_BMP085 bmp;
Adafruit_APDS9960 apds;
MPU6050 mpu(Wire);

#define BUZZER_PIN 25

enum State { NORMAL, HAZARD, ESCAPE };
State state = NORMAL;

struct Point { int x, y, z; };
Point entrance = {0,0,0};
Point junction;
Point room;
Point currentPos = {0,0,0};

int steps = 0;
bool turned = false;

unsigned long lastUpdate = 0;
int displayMode = 0;

void showOLED(String msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,20);
  display.println(msg);
  display.display();
}

void cycleDisplay() {
  unsigned long now = millis();
  if(now - lastUpdate > 3000) {
    lastUpdate = now;
    displayMode = (displayMode+1)%4;

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,20);

    if(displayMode==0) display.println("NO FIRE");
    if(displayMode==1) display.printf("P: %dhPa", bmp.readPressure()/100);
    if(displayMode==2) display.printf("T: %.1fC", bmp.readTemperature());
    if(displayMode==3) {
      uint16_t r,g,b,c;
      apds.getColorData(&r,&g,&b,&c);
      if(c < 1000) display.println("L: LOW");
      else if(c < 5000) display.println("L: MEDIUM");
      else display.println("L: HIGH");
    }
    display.display();
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode(BUZZER_PIN, OUTPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed");
    while(1);
  }
  if(!bmp.begin()) Serial.println("BMP init fail");
  if(!apds.begin()) Serial.println("APDS init fail");
  else apds.enableColor(true);

  byte status = mpu.begin();
  if(status!=0) { Serial.println("MPU init fail"); while(1); }
  mpu.calcOffsets();

  showOLED("Before Init");
}

void loop() {
  mpu.update();

  if(Serial.available()) {
    char cmd = Serial.read();
    if(cmd=='E') { entrance = {0,0,0}; steps=0; Serial.println("Entrance set"); }
    if(cmd=='J') { junction = {3,0,0}; Serial.println("Junction set"); }
    if(cmd=='R') { room = {3,5,0}; Serial.println("Room set"); showOLED("Setup Done"); }
    if(cmd=='H') { state = HAZARD; showOLED("FIRE!!"); digitalWrite(BUZZER_PIN,HIGH); }
    if(cmd=='N') { state = NORMAL; steps=0; digitalWrite(BUZZER_PIN,LOW); showOLED("Before Init"); }
  }

  if(state == NORMAL) {
    cycleDisplay();

    //* Real fire detection can be implemented by setting thresholds:
    // if(bmp.readTemperature()>50 || bmp.readPressure()<900 || light>7000) { state = HAZARD; } *//
  }

  if(state == HAZARD) {
    if(currentPos.x==room.x && currentPos.y==room.y) {
      showOLED("Turn Around");
      delay(3000);
      showOLED("Walk 5 steps");
      delay(5000);
      showOLED("Turn Left");
      delay(3000);
      showOLED("Walk 3 steps");
      delay(5000);
      showOLED("EXIT");
      delay(3000);
      state = NORMAL;
      digitalWrite(BUZZER_PIN,LOW);
    }
  }
}
