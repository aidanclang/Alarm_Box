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

unsigned long get_ms(int hour, int min, int sec){
  //TODO
  return 0;
}

void start_noises(unsigned long init_time){
  //TODO
}

void start_shake(unsigned long init_time){
  //TODO
}

void start_game(unsigned long init_time){
  //TODO
}

void start_light(unsigned long init_time){
  //TODO
}

void go_off(){
  //TODO
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