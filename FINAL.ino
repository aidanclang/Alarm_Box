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
#define LIGHT_PIN

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

struct GyroData {
  float gx;
  float gy;
  float gz;
  float ax;
  float ay;
  float az;
  float temperature;
};

void setup() {
  lcd.init();
  lcd.flipScreenVertically();
  lcd.clear();
  lcd.setFont(ArialMT_Plain_24);
  String text = "00:00:00";
  lcd.drawString(0, 0, text);
  lcd.display(); 

  if (!mpu.begin()) {
    lcd.clear();
    lcd.drawString(0,0,"MPU FAIL");
    lcd.display();
  }
  pinMode(B1, INPUT_PULLUP);
  pinMode(B2, INPUT_PULLUP);
  pinMode(B3, INPUT_PULLUP);
  pinMode(B4, INPUT_PULLUP);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  pinMode(LIGHT_PIN, INPUT);
}

int buttonRead(int button){
  const unsigned long DEBOUNCE_MS = 40;
  if (digitalRead(button) == LOW) {
    unsigned long start = millis();
    while (millis() - start < DEBOUNCE_MS) {
      if (digitalRead(button) == HIGH) return HIGH;
      delay(1);
    }
    while (digitalRead(button) == LOW) {
      delay(10);
    }
    return LOW;
  }
  return HIGH;
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
  static int screenCycle = 0;
  if (buttonRead(B1) == LOW){
    // cycle screen to previous setting
    screenCycle -= 1;
    if (screenCycle < 0){
      screenCycle = 0;
    }
  }
  if (buttonRead(B4) == LOW){
    // cycle screen to next setting
    screenCycle += 1;
    if (screenCycle > 7){
      return;
      // end of settings
    }
  }

  if (screenCycle == 0){
    // set hours
    if (buttonRead(B2) == LOW){
      // increase
      hours += 1;
      if (hours > 23){
        hours = 0;
      }
    }
    if (buttonRead(B3) == LOW){
      // decrease
      hours -= 1;
      if (hours < 0){
        hours = 23;
      }
    }
  }
  else if (screenCycle == 1){
    // set minutes
    if (buttonRead(B2) == LOW){
      // increase
      minutes += 1;
      if (minutes > 59){
        minutes = 0;
      }
    }
    if (buttonRead(B3) == LOW){
      // decrease
      minutes -= 1;
      if (minutes < 0){
        minutes = 59;
      }
    }
  }
  else if (screenCycle == 2){
    // set seconds
    if (buttonRead(B2) == LOW){
      // increase
      seconds += 1;
      if (seconds > 59){
        seconds = 0;
      }
    }
    if (buttonRead(B3) == LOW){
      // decrease
      seconds -= 1;
      if (seconds < 0){
        seconds = 59;
      }
    }
  }
  else if (screenCycle == 3){
    // set alarm
    if (buttonRead(B2) == LOW){
      // toggle alarm
      alarm = !alarm;
    }
  }
  else if (screenCycle == 4){
    // set buzzer
    if (buttonRead(B2) == LOW){
      // toggle buzzer
      buzzer = !buzzer;
    }
  }
  else if (screenCycle == 5){
    // set shaking
    if (buttonRead(B2) == LOW){
      // toggle shaking
      shake = !shake;
    }
  }
  else if (screenCycle == 6){
    // set game
    if (buttonRead(B2) == LOW){
      // toggle game
      game = !game;
    }
  }
  else if (screenCycle == 7){
    // set light sensor
    if (buttonRead(B2) == LOW){
      // toggle light sensor
      light = !light;
    }
  }
}

int lightLVL(){
  int sensorValue = analogRead(LIGHT_PIN);
  return sensorValue;
}

GyroData gyroLVL(){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  GyroData d;
  d.gx = g.gyro.x; // rad/s
  d.gy = g.gyro.y;
  d.gz = g.gyro.z;
  d.ax = a.acceleration.x; // m/s^2
  d.ay = a.acceleration.y;
  d.az = a.acceleration.z;
  return d;
}

void loop() {
  set_settings();
}