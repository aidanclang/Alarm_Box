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
  int screenCycle = 0;
  if (buttonRead(B1) == HIGH){
    // cycle screen to previous setting
    screenCycle -= 1;
    if (screenCycle < 0){
      screenCycle = 0;
    }
  }
  if (buttonRead(B4) == HIGH){
    // cycle screen to next setting
    screenCycle += 1;
  }


  if (screenCycle == 0){
    // set hours
    if (buttonRead(B2) == HIGH){
      // increase
      hours += 1;
      if (hours > 23){
        hours = 0;
      }
    }
    if (buttonRead(B3) == HIGH){
      // decrease
      hours -= 1;
      if (hours < 0){
        hours = 23;
      }
    }
  }
  else if (screenCycle == 1){
    // set minutes
    if (buttonRead(B2) == HIGH){
      // increase
      minutes += 1;
      if (minutes > 59){
        minutes = 0;
      }
    }
    if (buttonRead(B3) == HIGH){
      // decrease
      minutes -= 1;
      if (minutes < 0){
        minutes = 59;
      }
    }
  }
  else if (screenCycle == 2){
    // set seconds
    if (buttonRead(B2) == HIGH){
      // increase
      seconds += 1;
      if (seconds > 59){
        seconds = 0;
      }
    }
    if (buttonRead(B3) == HIGH){
      // decrease
      seconds -= 1;
      if (seconds < 0){
        seconds = 59;
      }
    }
  }
  else if (screenCycle == 3){
    // set alarm
    if (buttonRead(B2) == HIGH){
      // toggle alarm
      alarm = !alarm;
    }
  }
  else if (screenCycle == 4){
    // set buzzer
    if (buttonRead(B2) == HIGH){
      // toggle buzzer
      buzzer = !buzzer;
    }
  }
  else if (screenCycle == 5){
    // set shaking
    if (buttonRead(B2) == HIGH){
      // toggle shaking
      shake = !shake;
    }
  }
  else if (screenCycle == 6){
    // set game
    if (buttonRead(B2) == HIGH){
      // toggle game
      game = !game;
    }
  }
  else if (screenCycle == 7){
    // set light sensor
    if (buttonRead(B2) == HIGH){
      // toggle light sensor
      light = !light;
    }
  }
}

int lightLVL(bool setting){
  if (setting == true){
    // read light sensor value
    int sensorValue = analogRead();
    return sensorValue;
  }
}

void loop() {
  set_settings();
}