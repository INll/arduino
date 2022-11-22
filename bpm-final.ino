#include "TimerOne.h"
#include <LiquidCrystal.h> //Call the LiquidCrystal library

// acquire signal connected to the selected analog pin
int analogPin = A0;

// global variable to store the value from the analog pin
int ECG = 0;
int adc_flag = 0;

// Constants
const float rThreshold = 0.7;
const int minDiff = 10;

// Sample Buffer
const int N_SAMPLES = 250;
int nSamples = 0;
int sampleBuffer[N_SAMPLES];
int index = 0;

// Last time the bpm was shown
long lastTime;

// Display BPM in LCD
const int rs = 12, en = 11, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

byte heart1[8] = {B11111, B11111, B11111, B11111, B01111, B00111, B00011, B00001};
byte heart2[8] = {B00011, B00001, B00000, B00000, B00000, B00000, B00000, B00000};
byte heart3[8] = {B00011, B00111, B01111, B11111, B11111, B11111, B11111, B01111};
byte heart4[8] = {B11000, B11100, B11110, B11111, B11111, B11111, B11111, B11111};
byte heart5[8] = {B00011, B00111, B01111, B11111, B11111, B11111, B11111, B11111};
byte heart6[8] = {B11000, B11100, B11110, B11111, B11111, B11111, B11111, B11110};
byte heart7[8] = {B11000, B10000, B00000, B00000, B00000, B00000, B00000, B00000};
byte heart8[8] = {B11111, B11111, B11111, B11111, B11110, B11100, B11000, B10000};

// This 'setup' function will be called once when Arduino board powers on or is reset.
void setup()
{
  pinMode(8, INPUT); // Setup for leads off detection LO +
  pinMode(9, INPUT); // Setup for leads off detection LO -

  // set a timer of length in microseconds
  // Timer1 will generate an interrupt with the designated time interval
  // The interrupt or data sampling frequency is ( 1e6 / time interval number in microseconds )

  Timer1.initialize(500);
  Timer1.attachInterrupt( timerIsr );

  // attach the service routine here
  // the function 'timerIsr' will be called at every timer interruptevent
  
  Serial.begin (9600); // setup serial port
  // Baud rate of 230400 can securely support 4 kB/s data transfer rate between Arduino and PC serial port
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  lcd.createChar(1, heart1);
  lcd.createChar(2, heart2);
  lcd.createChar(3, heart3);
  lcd.createChar(4, heart4);
  lcd.createChar(5, heart5);
  lcd.createChar(6, heart6);
  lcd.createChar(7, heart7);
  lcd.createChar(8, heart8);
}

void timerIsr()
{
ECG = analogRead(analogPin); // read the input pin
//Serial.println(ECG); // send the acquired data to serial port for saving on PC
adc_flag = 1;
}

// This 'loop' function will be executed repeatedly
void loop()
{
  if (adc_flag)
{
  adc_flag = 0;
  //Wait for a bit to keep serial data from saturating
//  delay(1);
  }
  sampleBuffer[index] = analogRead(A0);

  // Show results every 500 ms once the buffer is filled
  if(nSamples >= N_SAMPLES && millis()-lastTime > 500) {
    lastTime = millis();
    
    int maxValue = 0;
    int minValue = 1024;
    
    // Calculate maximum and minimum
    for(int i = 0; i < N_SAMPLES; i++) {
      maxValue = max(sampleBuffer[i], maxValue);
      minValue = min(sampleBuffer[i], minValue); 
    }
  
    float diff = max(maxValue - minValue, minDiff);
    float threshold = diff * rThreshold + minValue;
    int nHeartbeats = 0;
    int tHeartbeats = 0;
    int lastHeartbeat = 0;
    
    for(int i = 1; i < N_SAMPLES; i++) {
      if(sampleBuffer[(index+i+1)%N_SAMPLES] >= threshold 
          && sampleBuffer[(index+i)%N_SAMPLES] < threshold) {
        // Save time difference and increase counter
        // Don't count time difference if it is too short or too long
        // - 15 measurements correspond to roughly 250 bpm
        // - 150 measurements correspond to roughly 25 bpm
        if(lastHeartbeat && i-lastHeartbeat > 15 && i-lastHeartbeat < 150) {
          tHeartbeats += i-lastHeartbeat; 
          nHeartbeats++;
        }
  
        // Save timestamp
        lastHeartbeat = i;
      }
    }
  
    // Calculate bpm
    int bpm = 60000.0 * nHeartbeats / (tHeartbeats * 20);
    
    // Show results if enough heartbeats are found
    // In 5 s there should be 4 at 50 bpm
//    if(nHeartbeats > 3) {
//      Serial.print("Heart Rate (bpm): ");
//      Serial.println(bpm);

    if(bpm >= 60 && bpm <= 120) {
      Serial.println("Heart Rate (bpm): ");
      Serial.print(bpm);

      lcd.setCursor(1,1);
      lcd.write(byte(1));
      lcd.setCursor(0,1);
      lcd.write(byte(2));
      lcd.setCursor(0,0);
      lcd.write(byte(3));
      lcd.setCursor(1,0);
      lcd.write(byte(4));
      lcd.setCursor(2,0);
      lcd.write(byte(5));
      lcd.setCursor(3,0);
      lcd.write(byte(6));
      lcd.setCursor(3,1);
      lcd.write(byte(7));
      lcd.setCursor(2,1);
      lcd.write(byte(8));
  
      lcd.setCursor(4,0);
      lcd.print(" Heart Rate ");
      lcd.setCursor(5,1);
      lcd.print("  ");
      lcd.print(bpm);
      lcd.print(" ");
      lcd.print("BPM     ");

    }
    else if(bpm > 120){
      Serial.println("Warning!");
      Serial.print("Heart Rate (bpm): ");
      Serial.print(bpm);
      
      lcd.setCursor(1,1);
      lcd.write(byte(1));
      lcd.setCursor(0,1);
      lcd.write(byte(2));
      lcd.setCursor(0,0);
      lcd.write(byte(3));
      lcd.setCursor(1,0);
      lcd.write(byte(4));
      lcd.setCursor(2,0);
      lcd.write(byte(5));
      lcd.setCursor(3,0);
      lcd.write(byte(6));
      lcd.setCursor(3,1);
      lcd.write(byte(7));
      lcd.setCursor(2,1);
      lcd.write(byte(8));
  
      lcd.setCursor(5,0);
      lcd.print(" !Warning! ");
      lcd.setCursor(5,1);
      lcd.print("  ");
      lcd.print(bpm);
      lcd.print(" ");
      lcd.print("BPM     ");
      
    }
    else {
      Serial.println("No heart beat detected");

      lcd.setCursor(1,1);
      lcd.write(byte(1));
      lcd.setCursor(0,1);
      lcd.write(byte(2));
      lcd.setCursor(0,0);
      lcd.write(byte(3));
      lcd.setCursor(1,0);
      lcd.write(byte(4));
      lcd.setCursor(2,0);
      lcd.write(byte(5));
      lcd.setCursor(3,0);
      lcd.write(byte(6));
      lcd.setCursor(3,1);
      lcd.write(byte(7));
      lcd.setCursor(2,1);
      lcd.write(byte(8));
  
      lcd.setCursor(5,0);
      lcd.print("  Nothing  ");
      lcd.setCursor(5,1);
      lcd.print("  Detected ");

    }
  }
  else {
    nSamples++;
  }
  
  // Next Index
  index = (index+1) % N_SAMPLES;
  delay(20);
}
