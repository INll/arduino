#include <TimerOne.h>
#include <LiquidCrystal.h>

int analogPin = A0;

int val = 0;
int adc_flag = 0;

// Initialize LiquidCrystal library
const int rs = 10, en = 11, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  // put your setup code here, to run once:
  pinMode(8, INPUT);
  pinMode(9, INPUT);

  lcd.begin(16, 2);
  lcd.print("CityU BME!!!!");

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

  lcd.setCursor(0, 1);
  lcd.print(millis() / 1000);
}

void timerIsr() {
  val = analogRead(analogPin);
  Serial.println(val);
  adc_flag = 1;
}