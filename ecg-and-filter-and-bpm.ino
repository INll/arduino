/*
 * VARIABLES
 * count: variable to hold count of rr peaks detected in 10 seconds
 * flag: variable that prevents multiple rr peak detections in a single heatbeat
 * hr: HeartRate (initialised to 72)
 * hrv: Heart Rate variability (takes 10-15 seconds to stabilise)
 * instance1: instance when heart beat first time
 * interval: interval between second beat and first beat
 * timer: variable to hold the time after which hr is calculated
 * value: raw sensor value of output pin
 */

long pastTime=0, timer;
double hrv =0, hr = 72, interval = 0;
int value = 0, count = 0, BPM = 0, inInt = 0, beat_new = 0, beat_old = 0;  
bool flag = 0;
float filteredValue = 0;
float doubleFilteredValue = 0;
int bpmThreshold = 620;

#define shutdown_pin 10 
#define threshold 100 // to identify R peak
#define timer_value 10000 // 10 seconds timer to calculate hr

boolean belowThreshold = true;
float beats[419];
int beatIndex;

template <int order> // order is 1 or 2

class LowPass
{
  private:
    float a[order];
    float b[order+1];
    float omega0;
    float dt;
    bool adapt;
    float tn1 = 0;
    float x[order+1]; // Raw values
    float y[order+1]; // Filtered values

  public:  
    LowPass(float f0, float fs, bool adaptive){
      // f0: cutoff frequency (Hz)
      // fs: sample frequency (Hz)
      // adaptive: boolean flag, if set to 1, the code will automatically set
      // the sample frequency based on the time history.
      
      omega0 = 6.28318530718*f0;
      dt = 1.0/fs;
      adapt = adaptive;
      tn1 = -dt;
      for(int k = 0; k < order+1; k++){
        x[k] = 0;
        y[k] = 0;        
      }
      setCoef();
    }

    void setCoef(){
      if(adapt){
        float t = micros()/1.0e6;
        dt = t - tn1;
        tn1 = t;
      }
      
      float alpha = omega0*dt;
      if(order==1){
        a[0] = -(alpha - 2.0)/(alpha+2.0);
        b[0] = alpha/(alpha+2.0);
        b[1] = alpha/(alpha+2.0);        
      }
      if(order==2){
        float alphaSq = alpha*alpha;
        float beta[] = {1, sqrt(2), 1};
        float D = alphaSq*beta[0] + 2*alpha*beta[1] + 4*beta[2];
        b[0] = alphaSq/D;
        b[1] = 2*b[0];
        b[2] = b[0];
        a[0] = -(2*alphaSq*beta[0] - 8*beta[2])/D;
        a[1] = -(beta[0]*alphaSq - 2*beta[1]*alpha + 4*beta[2])/D;      
      }
    }

    float filt(float xn){
      // Provide me with the current raw value: x
      // I will give you the current filtered value: y
      if(adapt){
        setCoef(); // Update coefficients if necessary      
      }
      y[0] = 0;
      x[0] = xn;
      // Compute the filtered values
      for(int k = 0; k < order; k++){
        y[0] += a[k]*y[k+1] + b[k]*x[k];
      }
      y[0] += b[order]*x[order];

      // Save the historical values
      for(int k = order; k > 0; k--){
        y[k] = y[k-1];
        x[k] = x[k-1];
      }
  
      // Return the filtered value    
      return y[0];
    }
};

template <int order>

class HighPass
{
  private:
    float a[order];
    float b[order+1];
    float omega0;
    float dt;
    bool adapt;
    float tn1 = 0;
    float x[order+1]; // Raw values
    float y[order+1]; // Filtered values

  public:  
    HighPass(float f0, float fs, bool adaptive){
      // f0: cutoff frequency (Hz)
      // fs: sample frequency (Hz)
      // adaptive: boolean flag, if set to 1, the code will automatically set
      // the sample frequency based on the time history.
      
      omega0 = 6.28318530718*f0;
      dt = 1.0/fs;
      adapt = adaptive;
      tn1 = -dt;
      for(int k = 0; k < order+1; k++){
        x[k] = 0;
        y[k] = 0;        
      }
      setCoef();
    }

    void setCoef(){
      if(adapt){
        float t = micros()/1.0e6;
        dt = t - tn1;
        tn1 = t;
      }
      
      float alpha = omega0*dt;
      if(order==1){
        float alphaFactor = 1/(1 + alpha/2.0);
        a[0] = -(alpha/2.0 - 1)*alphaFactor;
        b[0] = alphaFactor;
        b[1] = -alphaFactor;      
      }
      if(order==2){
        float alpha = omega0*dt;
        float dtSq = dt*dt;
        float c[] = {omega0*omega0, sqrt(2)*omega0, 1};
        float D = c[0]*dtSq + 2*c[1]*dt + 4*c[2];
        b[0] = 4.0/D;
        b[1] = -8.0/D;
        b[2] = 4.0/D;
        a[0] = -(2*c[0]*dtSq - 8*c[2])/D;
        a[1] = -(c[0]*dtSq - 2*c[1]*dt + 4*c[2])/D;   
      }
    }

    float filt(float xn){
      // Provide me with the current raw value: x
      // I will give you the current filtered value: y
      if(adapt){
        setCoef(); // Update coefficients if necessary      
      }
      y[0] = 0;
      x[0] = xn;
      // Compute the filtered values
      for(int k = 0; k < order; k++){
        y[0] += a[k]*y[k+1] + b[k]*x[k];
      }
      y[0] += b[order]*x[order];

      // Save the historical values
      for(int k = order; k > 0; k--){
        y[k] = y[k-1];
        x[k] = x[k-1];
      }
  
      // Return the filtered value    
      return y[0];
    }
};

// Signal instance
LowPass<2> lp(3,1000,true);
HighPass<2> hp(3,1000,true);

void setup() {
  Serial.begin(230400);
  pinMode(9, INPUT); // Setup for leads off detection LO +
  pinMode(8, INPUT); // Setup for leads off detection LO -
}

void loop() { 
  if((digitalRead(8) == 1)||(digitalRead(9) == 1)){
    Serial.println("leads off!");
    digitalWrite(shutdown_pin, LOW); //standby mode
    pastTime = micros();
    timer = millis();
  }

  else {
    digitalWrite(shutdown_pin, HIGH); //normal mode
    value = analogRead(A0);
    // filteredValue = map(value, 250, 400, 0, 100);  // Basic filter
    // LP + HP
    filteredValue = lp.filt(value); // LP filter
    // doubleFilteredValue = hp.filt(filteredValue); // HP filter

    // HP only
    // filteredValue = hp.filt(value); // HP filter

    if((value > threshold) && (!flag)) {
      count++;  
      Serial.println("in");
      flag = 1;
      interval = micros() - pastTime; //RR interval
      pastTime = micros();
    }

    else if((value < threshold)) {
      flag = 0;
    }

    if ((millis() - timer) > 10000) {
      hr = count*6;
      timer = millis();
      count = 0; 
    }

    hrv = hr/60 - interval/1000000;
    // Serial.print(hr);
    // Serial.print(",");
    // Serial.print(hrv);
    // Serial.print(",");
    Serial.println(filteredValue); // For LP + HP
    // Serial.println(filteredValue);
    delay(1);

    String inString = Serial.readStringUntil('\n');
    Serial.println(inString);

    if (inString != " ") {
      // If leads off detection is true notify with blue line
      if (inString.equals("leads off!")) { 
        return;
      }
      // If the data is good let it through
      else 
      {
        inInt = inString.toInt(); 
        
        // BPM calculation check
        if (inInt > bpmThreshold && belowThreshold == true)
        {
          calculateBPM();
          belowThreshold = false;
        }
        else if(inInt < bpmThreshold)
        {
          belowThreshold = true;
        }
      }
    }
  }
}
  
void calculateBPM () 
{  
  int beat_new = millis();    // get the current millisecond
  int diff = beat_new - beat_old;    // find the time between the last two beats
  float currentBPM = 60000 / diff;    // convert to beats per minute
  beats[beatIndex] = currentBPM;  // store to array to convert the average
  float total = 0.0;
  for (int i = 0; i < 419; i++){
    total += beats[i];
  }
  BPM = int(total / 419);
  Serial.println(currentBPM);
  beat_old = beat_new;
  beatIndex = (beatIndex + 1) % 419;  // cycle through the array instead of using FIFO queue
  }