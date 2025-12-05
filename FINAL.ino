#include <SSD1306Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define B0 0
#define B1 0
#define B2 0
#define B3 0

#define LED0 0
#define LED1 0
#define LED2 0
#define LED3 0

#define hours         0
#define minutes       1
#define seconds       2 
#define alarm         3
#define alarm_volume  4
#define alarm_delay   5
#define buzzer        6
#define buzzer_delay  7
#define shake         8
#define shake_time    9
#define game          10
#define game_levels   11
#define light         12

// things for button reading
int button_delay = 50;
int buttons[] = {B0, B1, B2, B3};
unsigned long last_changes[] = {0, 0, 0, 0};
int last_states[] = {LOW, LOW, LOW, LOW};
int current_states[] = {LOW, LOW, LOW, LOW};
int pressed[] = {0, 0, 0, 0};

//default settings
int settings[] = {
  8, 0, 0,            // hours, minutes, seconds
  1, 5, 15,           // alarm, alarm_volume, alarm_delay
  1, 0,               // buzzer, buzzer_delay
  1, 30,              // shake, shake_time
  1, 5,               // game, game_time
  1                   // light
};

// accelerometer and screen
Adafruit_MPU6050 mpu;
SSD1306Wire lcd(0x3c, SDA, SCL);

void setup() {
  pinMode(B0, INPUT_PULLUP);
  pinMode(B1, INPUT_PULLUP);
  pinMode(B2, INPUT_PULLUP);
  pinMode(B3, INPUT_PULLUP);

  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  lcd.init();
  lcd.flipScreenVertically();
  lcd.clear();
  lcd.setFont(ArialMT_Plain_24);
  String text = "00:00:00";
  lcd.drawString(0, 0, text);
  lcd.display(); 
}

unsigned long get_ms(int hr, int min, int sec){
  //TODO
  return 0;
}

void go_off(){
  //TODO
}


void start_game() {
  unsigned long last_act = millis();
  int pattern[] = {1,1,1,1};
  int j = 0;
  int sum = 0;
  for(int i = 0; i< settings[11]; i++) {
    for(int i = 0; i<4; i++) {
      pattern[i] = random(1,4);
    }
    while(j < 4) {
      if(millis() - last_act > 15000) {
        start_noises();
        last_act = millis();
      }
      for(int i = 0; i<4; i++) {
        if(pattern[i] == 1) {
          digitalWrite(LED0, 1);
          delay(50);
          digitalWrite(LED0, 0);
        }else if(pattern[i] == 2) {
          digitalWrite(LED1, 1);
          delay(50);
          digitalWrite(LED1, 0);
        }else if(pattern[i] == 3) {
          digitalWrite(LED2, 1);
          delay(50);
          digitalWrite(LED2, 0);
        }else if(pattern[i] == 4) {
          digitalWrite(LED3, 1);
          delay(50);
          digitalWrite(LED3, 0);
        }
      }
      j = 0;
      while(j < 4) {
        unsigned long temp = millis();
        sum = 0;
        while (millis()-temp < 5000) {
          read_buttons();
          for(int i = 0; i<4; i++) {
            sum += buttons[i];
          }
          if (sum > 0) {
            last_act = millis();
            break;
          }
        }
        if (buttons[pattern[j]-1] == 1) {
          j++;
        } else {
          break;
        }
      }
    }
  }  
}
void start_light() {
  unsigned long last_act = millis();
  bool light_received = false;
  while (light_received = false) {
    if(millis() - last_act > 15000) {
      start_noises();
      last_act = millis();
    }
    if(digitalRead(photoresistor) = 1) {
      light_received = true;
    }
  }
}

// button reader with debouncing
void read_buttons(){
  for(int i = 0; i < 4; i++){
    int button = buttons[i];
    int reading = digitalRead(button);
    if(reading != last_states[i])
      last_changes[i] = millis();
    
    if(millis() > last_changes[i] + button_delay){
      if(reading != current_states[i])
        last_states[i] = current_states[i];
        current_states[i] = reading;
        pressed[i] = 1 - reading;
    }
    pressed[i] = 0;
  }
}

int button_is_pressed(int button_number){
  return pressed[button_number];
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
  //TODO
}

void loop() {
  set_settings();
}