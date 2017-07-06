/*****************************************************
 * Author: Rubén Espino San José
 * Puma Pride Robotics Team
 * Date: 5-July-2017
 * 
 * This program is used as a chronometer to measure lap
 * times in line-following robots competitions.
 ****************************************************/

#include <TimerOne.h>
#include "TM1637.h"
#define ON 1
#define OFF 0

#define CLK 3   
#define DIO 2
TM1637 tm1637(CLK,DIO);

int8_t TimeDisp[] = {0x00,0x00,0x00,0x00};
char Get_time = ON;
char Update = ON;
int s = 0;
int ms = 0;
unsigned int best_time = 10000;

#define SENSOR 4
#define LED 13
#define BUZZER 10
#define BATTERY A3

char blinky = OFF;
float bat = 8.4;
unsigned long bat_count = 0;
char enable_buzzer = OFF;
char enable_double_beep = OFF;
unsigned long buzzer_count = 0;

void setup() {
  pinMode(SENSOR, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
//  Serial.begin(115200);

  bat = (float)analogRead(BATTERY)/1023*5*(47+33)/47;
  
  tm1637.set();
  tm1637.init();
  Timer1.initialize(10000);//timing for 10ms
  Timer1.attachInterrupt(TimingISR);//declare the interrupt serve routine:TimingISR  

  tm1637.point(POINT_ON);
}

void loop() {
  if(millis() - bat_count > 500)
  {
    bat_count = millis();
    bat = bat*4/5 + ((float)analogRead(BATTERY)/1023*5*(47+33)/47)/5;
    
    if(bat < 7.6)
      digitalWrite(LED, ON);
    else
      digitalWrite(LED, OFF);
  }

  if(enable_buzzer == ON && millis() - buzzer_count > 50)
  {
    enable_buzzer = OFF;
    noTone(BUZZER);

    if(enable_double_beep == ON)
      buzzer_count = millis();
  }

  if(enable_double_beep == ON && enable_buzzer == OFF && millis() - buzzer_count > 90)
  {
    enable_double_beep = OFF;
    enable_buzzer = ON;
    tone(BUZZER, 1000);
    buzzer_count = millis();
  }
  
  if(Update == ON)
  {
    TimeUpdate();
    tm1637.display(TimeDisp);
  }
}

void TimingISR()
{
  ms = ms + 10;
  if(ms == 1000)
  {
    s ++;
    if(s == 100)
      s = 0;
    ms = 0;  
  }

  if(digitalRead(SENSOR) == false && Get_time == ON)
  {
    unsigned int time_lap = 0;
    
    TimeUpdate();
    tm1637.display(TimeDisp);
    
//    Serial.print(TimeDisp[0]);
//    Serial.print(TimeDisp[1]);
//    Serial.print(":");
//    Serial.print(TimeDisp[2]);
//    Serial.println(TimeDisp[3]);

    time_lap = s*1000 + ms;
    if(time_lap < best_time)
    {
      best_time = time_lap;
      enable_double_beep = ON;
    }
    enable_buzzer = ON;
    tone(BUZZER, 1000);
    buzzer_count = millis();

    ms = 0;
    s = 0;
    Get_time = OFF;
    Update = OFF;
  }
  if(s == 1 && Get_time == OFF)
    Get_time = ON;
  if(s == 2 && Update == OFF)
    Update = ON;
}

void TimeUpdate(void)
{
  TimeDisp[0] = (int8_t)(s / 10);
  TimeDisp[1] = (int8_t)(s % 10);
  TimeDisp[2] = (int8_t)(ms / 100);
  TimeDisp[3] = (int8_t)((ms % 100) / 10);
}
