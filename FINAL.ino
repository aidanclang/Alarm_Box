#include <SSD1306Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define B0 18
#define B1 17
#define B2 16
#define B3 19

#define LED0 27
#define LED1 26
#define LED2 25
#define LED3 33

#define BUZZ 13
#define VIBR 14
#define LIGHT 4

#define hours         0
#define minutes       1
#define seconds       2 
#define alarm         3
#define alarm_delay   4
#define buzzer        5
#define buzzer_delay  6
#define shake         7
#define shake_time    8
#define game          9
#define game_levels   10
#define light         11

// constants
const unsigned long noise_time = 15000;
const float shake_threshold = 20; // needs testing
const unsigned long shake_increment = 200; // needs testing

const int alarm_freq = 300;
const int alarm_note_len = 250;
const int len_between_notes = 250;

const int lights[] = {LED0, LED1, LED2, LED3};

const int light_threshold = 2000;

// things for button reading
int button_delay = 50;
int buttons[] = {B0, B1, B2, B3};
unsigned long last_changes[] = {0, 0, 0, 0};
int current_states[] = {HIGH, HIGH, HIGH, HIGH};
int pressed[] = {0, 0, 0, 0};

//default settings and their maximum values
int default_settings[] = {
  8, 0, 0,            // hours, minutes, seconds
  1, 15,              // alarm, alarm_delay
  1, 0,               // buzzer, buzzer_delay
  1, 30,              // shake, shake_time
  1, 5,               // game, game_levels
  1                   // light
};
int settings[12];

int max_values[] = {
  23, 59, 59,
  1, 30,
  1, 30,
  1, 60,
  1, 15,
  1
};

// accelerometer and screen
Adafruit_MPU6050 mpu;
SSD1306Wire lcd(0x3c, SDA, SCL);

void setup() {
  Serial.begin(115200);

  pinMode(B0, INPUT_PULLUP);
  pinMode(B1, INPUT_PULLUP);
  pinMode(B2, INPUT_PULLUP);
  pinMode(B3, INPUT_PULLUP);

  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  pinMode(BUZZ, OUTPUT);
  pinMode(VIBR, OUTPUT);
  pinMode(LIGHT, INPUT);

  ledcSetup(0, 5000, 8);
  ledcAttachPin(BUZZ, 0);

  if (!mpu.begin()) {
    Serial.println("Sensor init failed");
    while (1)
      yield();
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

  lcd.init();
  lcd.flipScreenVertically();
  lcd.clear();
  lcd.setFont(ArialMT_Plain_16);
  lcd.display();
}

unsigned long get_ms(int hour, int min, int sec){
  return (hour * 3600000) + (min * 60000) + (sec * 1000);
}

void start_noises(unsigned long init_time){
  // display instructions
  String message[] = {"Press any button. :"};
  int value[] = {0};
  update_screen(1, message, value, -1);

  unsigned long last_switch = 0;
  bool sound_on = false;
  bool input = false;
  int tone = 3000;
  while(!input){
    read_buttons();
    input = button_is_pressed(0) ||
            button_is_pressed(1) ||
            button_is_pressed(2) ||
            button_is_pressed(3);
    unsigned long curr_time = millis();
    if(curr_time - last_switch > 250){
      last_switch = curr_time;
      sound_on = !sound_on;
      if(settings[alarm] == 1 && curr_time - init_time > settings[alarm_delay]*1000){
        if(sound_on){
          ledcWriteTone(0, tone);
          tone = 7000 - tone;
          digitalWrite(VIBR, LOW);
        } else {
          ledcWriteTone(0, 0);
          if(settings[buzzer] == 1 && curr_time - init_time > settings[buzzer_delay]*1000)
            digitalWrite(VIBR, HIGH);}
      } else if (settings[buzzer] == 1 && curr_time - init_time > settings[buzzer_delay]*1000){
        if(!sound_on){
          digitalWrite(VIBR, LOW);
        } else {
          digitalWrite(VIBR, HIGH);
        }
      }
    }
  }
  ledcWriteTone(0,0);
  digitalWrite(VIBR, LOW);
}

void start_shake(unsigned long init_time){
  if(settings[shake] == 0)
    return;
  unsigned long last_activity = millis();
  unsigned long time_shaken = 0;
  while(time_shaken < settings[shake_time]*1000){
    // display progress
    String messages[] = {"Goal: ", "Current: "};
    int values[] = {settings[shake_time], time_shaken/1000};
    update_screen(2, messages, values, -1);

    if(millis() - last_activity > noise_time){
      start_noises(init_time);
      last_activity = millis();
      time_shaken = 0;
    } else if(total_acceleration() > shake_threshold){
      time_shaken += shake_increment;
      last_activity = millis();
      delay(100);
    }
  }
}

void start_game(unsigned long init_time) {
  if(settings[game] == 0)
    return;
  String msg[] = {"Copy the pattern          ", "Level: "};
  String msg2[] = {"Copy the pattern          ", "Level: ", "GO                          "};
  int nums[] = {0, 1};
  int nums2[] = {0, 1, 0};
  unsigned long last_act = millis();
  int pattern[settings[game_levels]];
  for(int i = 0; i<settings[game_levels]; i++) {
    pattern[i] = random(0,4);
  }
  int level = 1;
  while(level <= settings[game_levels]) {
    nums[1] = level;
    nums2[1] = level;
    update_screen(2, msg, nums, -1);
    for(int i = 0; i<level; i++) {
      delay(500);
      digitalWrite(lights[pattern[i]], HIGH);
      delay(1000);
      digitalWrite(lights[pattern[i]], LOW);
      last_act = millis();
    }
    update_screen(3, msg2, nums2, -1);
    int step = 0;
    while(step < level) {
      read_buttons();
      int sum = pressed[0] + pressed[1] + pressed[2] + pressed[3];
      if (sum > 0){
        last_act = millis();
        if (sum > 1)
          break;
        if (pressed[pattern[step]] == 1) {
          step++;
        } else {
          break;
        }
      }
      if(millis() - last_act > 15000) {
        start_noises(init_time);
        last_act = millis();
        break;
      }
    }
    if (step >= level)
      level++;
  }  
}

void start_light(unsigned long init_time) {
  if(settings[light] == 0)
    return;
  String msg[] = {"Turn on lights         "};
  int filler[] = {0};
  update_screen(1, msg, filler, -1);
  unsigned long last_act = millis();
  while (analogRead(LIGHT) < light_threshold) {
    if(millis() - last_act > 15000) {
      start_noises(init_time);
      last_act = millis();
    }
  }
}

void go_off(){
  unsigned long init_time = millis();
  start_noises(init_time);
  start_shake(init_time);
  start_game(init_time);
  start_light(init_time);
}

// button reader with debouncing
void read_buttons(){
  for(int i = 0; i < 4; i++){
    int button = buttons[i];
    int reading = digitalRead(button);

    if(millis() > last_changes[i] + button_delay && reading != current_states[i]){
        current_states[i] = reading;
        pressed[i] = 1 - reading;
        last_changes[i] = millis();
      }else{
      pressed[i] = 0;
    }
  }
}

int button_is_pressed(int button_number){
  return pressed[button_number];
}

float total_acceleration(){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  return sqrt(
    a.acceleration.x * a.acceleration.x +
    a.acceleration.y * a.acceleration.y +
    a.acceleration.z * a.acceleration.z
  );
}

void update_screen(int num_messages, String messages[], int values[], int being_changed){
  lcd.clear();
  int y = 0;
  for(int i = 0; i < num_messages; i++){
    String text = messages[i];
    text += values[i];
    if(i == being_changed)
      text += " <";
    lcd.drawString(0, y, text);
    y += 15;
  }
  lcd.display();
}

void timer(unsigned long time){
  unsigned long endTime = millis() + time;
  while(millis() < endTime){
    read_buttons();
    if(button_is_pressed(3))
      return;
    lcd.clear();
    unsigned long curr = endTime - millis();
    String text = "";
    text += curr/36000000;
    text += (curr/3600000) % 10;
    text += ":";
    text += curr/600000 % 6;
    text += (curr/60000) % 10;
    text += ":";
    text += (curr/10000) % 6;
    text += (curr/1000) % 10;
    lcd.drawString(0, 0, text);
    lcd.display();
  }
  go_off();
}

void set_settings(){
  int index = 0;
  while(index < 12){
    read_buttons();

    // display proper settings on screen
    if(index == hours || index == minutes || index == seconds){
      String current_settings[] = {"Hours: ", "Minutes: ", "Seconds: "};
      int values[] = {settings[hours], settings[minutes], settings[seconds]};
      update_screen(3, current_settings, values, index);
    }else if(index == alarm || index == alarm_delay){
      String current_settings[] = {"Alarm: ", "Alarm Delay: "};
      int values[] = {settings[alarm], settings[alarm_delay]};
      update_screen(2, current_settings, values, index - 3);
    }else if(index == buzzer || index == buzzer_delay){
      String current_settings[] = {"Vibrate: ", "Vibrate Delay: "};
      int values[] = {settings[buzzer], settings[buzzer_delay]};
      update_screen(2, current_settings, values, index - 5);
    }else if(index == shake || index == shake_time){
      String current_settings[] = {"Shake: ", "Shake Time: "};
      int values[] = {settings[shake], settings[shake_time]};
      update_screen(2, current_settings, values, index - 7);
    }else if(index == game || index == game_levels){
      String current_settings[] = {"Simon Game: ", "Levels: "};
      int values[] = {settings[game], settings[game_levels]};
      update_screen(2, current_settings, values, index - 9);
    }else if(index == light){
      String current_settings[] = {"Light Sensor: "};
      int values[] = {settings[light]};
      update_screen(1, current_settings, values, index - 11);
    }

    // update settings or move to previous/next setting
    if(button_is_pressed(3)){
      index = max(index - 1, 0);
    } else if(button_is_pressed(0)){
      index += 1;
    } else if(button_is_pressed(2)){
      settings[index] = max(settings[index] - 1, 0);
    } else if(button_is_pressed(1)){
      settings[index] = min(settings[index] + 1, max_values[index]);
    }
  }
  timer(get_ms(settings[hours], settings[minutes], settings[seconds]));
}

void loop() {
  for(int i = 0; i < 12; i++){
    settings[i] = default_settings[i];
  }
  set_settings();
}