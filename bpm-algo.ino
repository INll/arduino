#include <TimerOne.h>
#include <LiquidCrystal.h>

int analogPin = A0;

int val = 0;
int adc_flag = 0;

// Variables for BPM algorithm
int prevTime = 0;
int BPM = 0;
int beatArr[59];
int index = 0;
float beatSum = 0.0;
float avg = 0.0;

// Initialize LiquidCrystal library
const int rs = 10, en = 11, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  // put your setup code here, to run once:
  pinMode(8, INPUT);
  pinMode(9, INPUT);

  lcd.begin(16, 2);

  // Pure decorative effect
  // Initialize lcd arary of size 16x2d
  // lcd.display();
  // lcd.print("Initializing...");
  // delay(2000);

  // lcd.clear();
  // delay(1000);

  calcBeatPerMinute();

  // Timer1.initialize(500);
  // Timer1.attachInterrupt(dispHeartBeat);
  // Serial.begin(230400);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (adc_flag) 
  {
    adc_flag = 0;
    delay(1);   // Delay execution by 1ms
  }


}

void dispHeartBeat() {
  lcd.setCursor(0, 0);
  lcd.print("Heart Beat Rate:");

  lcd.setCursor(5, 2);
  avg[index] = analogRead(analogPin);
  index++;

  if (index >= 10) {
    for (int j = 0; j < 60; j++) {
      sum 
    }
  }

  lcd.setCursor(10, 2);
  lcd.print("BPM");

  adc_flag = 1;
}

void calcBeatPerMinute() {
  float arrSum = 0.0;
  float avg = 0.0;
}