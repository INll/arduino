#include "TimerOne.h"

int analogPin = A0;

int val = 0;
int adc_flag = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(8, INPUT);
  pinMode(9, INPUT);

  Timer1.initialize(500);

  Timer1.attachInterrupt( timerIsr );

  Serial.begin(230400);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (adc_flag) 
  {
    adc_flag = 0;
    delay(1);
  }
}

void timerIsr() {
  val = analogRead(analogPin);
  Serial.println(val);
  adc_flag = 1;
}