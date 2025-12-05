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

#define BUZZ 0
#define VIBR 0
#define LIGHT 0

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

// constants
const unsigned long noise_time = 15000;
const float shake_threshold = 25; // needs testing
const unsigned long shake_increment = 1; // needs testing

const int alarm_freq = 300;
const int alarm_note_len = 250;
const int len_between_notes = 250;

// things for button reading
int button_delay = 50;
int buttons[] = {B0, B1, B2, B3};
unsigned long last_changes[] = {0, 0, 0, 0};
int last_states[] = {LOW, LOW, LOW, LOW};
int current_states[] = {LOW, LOW, LOW, LOW};
int pressed[] = {0, 0, 0, 0};

//default settings and their maximum values
int settings[] = {
  8, 0, 0,            // hours, minutes, seconds
  1, 5, 15,           // alarm, alarm_volume, alarm_delay
  1, 0,               // buzzer, buzzer_delay
  1, 30,              // shake, shake_time
  1, 5,               // game, game_levels
  1                   // light
};
int max_values[] = {
  12, 12, 12,
  1, 10, 30,
  1, 30,
  1, 60,
  1, 15,
  1
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
  lcd.setFont(ArialMT_Plain_24);
  lcd.display();
}

unsigned long get_ms(int hour, int min, int sec){
  return (hour * 3600000) + (min * 60000) + (sec * 1000);
}

void start_noises(unsigned long init_time){
  // display instructions
  String message[] = {"Press any button. :"};
  int value[] = {0};
  update_screen(message, value, -1);

  bool input = false;
  while(!input){
    read_buttons();
    input = button_is_pressed(0) ||
            button_is_pressed(1) ||
            button_is_pressed(2) ||
            button_is_pressed(3);
    unsigned long curr_time = millis();
    if(settings[alarm] == 1 && curr_time - init_time > settings[alarm_delay]*1000){
      tone(BUZZ, alarm_freq);
      digitalWrite(VIBR, LOW);
      delay(alarm_note_len);
      noTone(BUZZ);
      if(settings[buzzer] == 1 && curr_time - init_time > settings[buzzer_delay]*1000)
        digitalWrite(VIBR, HIGH);
      delay(len_between_notes);
    }
  }
}

void start_shake(unsigned long init_time){
  if(settings[shake] == 0)
    return;
  unsigned long last_activity = millis();
  unsigned long time_shaken = 0;
  while(time_shaken < settings[shake_time]){
    // display progress
    String messages[] = {"Goal: ", "Current: "};
    int values[] = {settings[shake_time], time_shaken};
    update_screen(messages, values, -1);

    if(millis() - last_activity > noise_time){
      start_noises(init_time);
      time_shaken = 0;
    } else if(total_acceleration() > shake_threshold){
      time_shaken += shake_increment;
      last_activity = millis();
    }
  }
}

void start_game(unsigned long init_time){
  //TODO
}

void start_light(unsigned long init_time){
  //TODO
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

float total_acceleration(){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  return sqrt(
    a.acceleration.x * a.acceleration.x +
    a.acceleration.y * a.acceleration.y +
    a.acceleration.z * a.acceleration.z
  );
}

void update_screen(String messages[], int values[], int being_changed){
  lcd.clear();
  for(int i = 0; i < sizeof(messages); i++){
    String text = messages[i];
    if(max_values[i] == 1){
      text += values[i] == 1 ? "On" : "Off";
    }else{
      text += values[i];
    }
    if(i == being_changed)
      text += " <";
    lcd.println(text);
  }
  lcd.display();
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
  int index = 0;
  while(index < 13){
    read_buttons();

    // display proper settings on screen
    if(index == hours || index == minutes || index == seconds){
      String current_settings[] = {"Hours: ", "Minutes: ", "Seconds: "};
      int values[] = {settings[hours], settings[minutes], settings[seconds]};
      update_screen(current_settings, values, index);
    }else if(index == alarm || index == alarm_volume || index == alarm_delay){
      String current_settings[] = {"Alarm: ", "Alarm Volume: ", "Alarm Delay: "};
      int values[] = {settings[alarm], settings[alarm_volume], settings[alarm_delay]};
      update_screen(current_settings, values, index);
    }else if(index == buzzer || index == buzzer_delay){
      String current_settings[] = {"Vibration: ", "Vibration Delay: "};
      int values[] = {settings[buzzer], settings[buzzer_delay]};
      update_screen(current_settings, values, index);
    }else if(index == shake || index == shake_time){
      String current_settings[] = {"Shake to Disable: ", "Shake Time: "};
      int values[] = {settings[shake], settings[shake_time]};
      update_screen(current_settings, values, index);
    }else if(index == game || index == game_levels){
      String current_settings[] = {"Memory Game to Disable: ", "Number of Levels: "};
      int values[] = {settings[game], settings[game_levels]};
      update_screen(current_settings, values, index);
    }else if(index == light){
      String current_settings[] = {"Light to Disable: "};
      int values[] = {settings[light]};
      update_screen(current_settings, values, index);
    }

    // update settings or move to previous/next setting
    if(button_is_pressed(0)){
      index = max(index - 1, 0);
    } else if(button_is_pressed(3)){
      index += 1;
    } else if(button_is_pressed(1)){
      settings[index] = max(settings[index] - 1, 0);
    } else if(button_is_pressed(2)){
      settings[index] = min(settings[index] + 1, max_values[index]);
    }
  }
}

void loop() {
  set_settings();
}