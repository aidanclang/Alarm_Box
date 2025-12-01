#include <SSD1306Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#define B1 
#define B2
#define B3
#define B4
#define LED1
#define LED2
#define LED3
#define LED4

// default timer settings
int hours = 8;
int minutes = 0;
int seconds = 0;

// default alarm settings
bool alarm = true;
int alarmVolume = 5;
int alarmDelay = 0;

//default buzzer settings
bool buzzer = true;
int buzzerDelay = 0;

// default shaking settings
bool shake = true;
int shakeTime = 30;

// default game settings
bool game = true;
int gameLevels = 10;

// default light sensor settings
bool light = true;

Adafruit_MPU6050 mpu;
SSD1306Wire lcd(0x3c, SDA, SCL);
void setup() {
  lcd.init();
  lcd.flipScreenVertically();
  lcd.clear();
  lcd.setFont(ArialMT_Plain_24);
  String text = "00:00:00";
  lcd.drawString(0, 0, text);
  lcd.display(); 
}

int buttonRead(int button){
  
}

void timer(unsigned long time){
  unsigned long endTime = millis() + time;
  while(millis() < endTime){
    lcd.clear();
    unsigned long curr = endTime - millis();
    String text = "";
    text += curr/600000;
    text += (curr/60000) % 10;
    text += ":";
    text += (curr/10000) % 6;
    text += (curr/1000) % 10;
    text += ":";
    text += (curr/100) % 10;
    text += (curr/10) % 10;
    lcd.drawString(0, 0, text);
    lcd.display();
  }
}

void set_settings(){
  
}

void loop() {
  set_settings();
}