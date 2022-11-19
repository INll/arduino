unsigned int average_value[60];
int i = 0;

long lastDebounceTime = 0;
long debounceDelay = 300;

long counter = 0;

void setup() {
  // initialize the serial communication:
  Serial.begin(230400);
  pinMode(8, INPUT); // Setup for leads off detection LO +
  pinMode(9, INPUT); // Setup for leads off detection LO -

  attachInterrupt(0, right_Hand_Detect, FALLING);
  //attachInterrupt(1, left_Hand_Detect, FALLING);

}

void loop() {
  float sum = 0.0;
  float average = 0.0;
  
  if((digitalRead(10) == 1)||(digitalRead(11) == 1)){
    Serial.print('!');
  }
  else{
    //Serial.print('@');
    if ((millis() - lastDebounceTime) > debounceDelay){
      average_value[i] = analogRead(A0);
      i++;
      
      if (i >= 30){
        for (int j = 0; j < 60; j++){     
          sum = sum + average_value[j];
          if (average_value[j] > 370){
            Serial.println(average_value[j]);
          }
          //Serial.println(average);
        }
        average = sum/60.0;
        //Serial.print("The Average is: ");
        //Serial.println(average);
        i = 0;
      }
    }
  }
  //Wait for a bit to keep serial data from saturating
  delay(1);
}

void right_Hand_Detect(){
  if(digitalRead(3) == 0){
    //Serial.print(counter);
    //Serial.println("Right hand");
    //counter++;
    lastDebounceTime = millis();
    Serial.println(lastDebounceTime);
  }
}