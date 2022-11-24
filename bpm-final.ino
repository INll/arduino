#include "TimerOne.h"
#include <LiquidCrystal.h>

// Acquire signal connected to the selected analog pin
int analogPin = A0;

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

// Initialize Liquid Crysal Display (LCD)
const int rs = 12, en = 11, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Heart drawing 
byte heart1[8] = {B11111, B11111, B11111, B11111, B01111, B00111, B00011, B00001};
byte heart2[8] = {B00011, B00001, B00000, B00000, B00000, B00000, B00000, B00000};
byte heart3[8] = {B00011, B00111, B01111, B11111, B11111, B11111, B11111, B01111};
byte heart4[8] = {B11000, B11100, B11110, B11111, B11111, B11111, B11111, B11111};
byte heart5[8] = {B00011, B00111, B01111, B11111, B11111, B11111, B11111, B11111};
byte heart6[8] = {B11000, B11100, B11110, B11111, B11111, B11111, B11111, B11110};
byte heart7[8] = {B11000, B10000, B00000, B00000, B00000, B00000, B00000, B00000};
byte heart8[8] = {B11111, B11111, B11111, B11111, B11110, B11100, B11000, B10000};

// This 'setup' function will be called once when Arduino board powers on or is reset.
void setup() {
  pinMode(8, INPUT); // Setup for leads off detection LO +
  pinMode(9, INPUT); // Setup for leads off detection LO -
  
  // Setup serial port
  Serial.begin (230400);
  // Baud rate of 230400 can securely support 4 kB/s data transfer rate between Arduino and PC serial port
  // 230400 is also the baud rate used in Project 3 in lab 4608 
  
  // Set up the LCD's number of columns and rows:
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

// This 'loop' function will be executed repeatedly
void loop() {
  sampleBuffer[index] = analogRead(A0);

  // Show results every 500 ms once the buffer is filled
  if (nSamples >= N_SAMPLES && millis()-lastTime > 500) {
    lastTime = millis();  // End of last time window
    
    // Define baselines
    int maxValue = 0;
    int minValue = 1024;
    
    // Dynamically set threshold based on max and min in sampleBuffer
    // Ideally, max and min represent the R and S wave values
    for (int i = 0; i < N_SAMPLES; i++) {
      maxValue = max(sampleBuffer[i], maxValue);
      minValue = min(sampleBuffer[i], minValue); 
    }
  
    // Threshold is determined as 70% of diff between Max and Min, plus Min
    float diff = max(maxValue - minValue, minDiff);
    float threshold = diff * rThreshold + minValue;

    int nHeartbeats = 0;
    int tHeartbeats = 0;
    int lastHeartbeat = 0;
    
    for (int i = 1; i < N_SAMPLES; i++) {
      if (sampleBuffer[(index+i+1)%N_SAMPLES] >= threshold 
          && sampleBuffer[(index+i)%N_SAMPLES] < threshold) {
        // Save time difference and increase counter
        // Don't count time difference if it is too short or too long
        // - 15 measurements correspond to roughly 250 bpm
        // - 150 measurements correspond to roughly 25 bpm
        if (lastHeartbeat && i-lastHeartbeat > 15 && i-lastHeartbeat < 150) {
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

    if (bpm >= 60 && bpm <= 120) {
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
    else if (bpm > 120) {
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