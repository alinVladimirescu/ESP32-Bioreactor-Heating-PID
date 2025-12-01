#define THERMISTOR_PIN A0
#define SERIES_RESISTOR 10000    
#define THERMISTOR_NOMINAL 10000 
#define TEMPERATURE_NOMINAL 25   
#define B_COEFFICIENT 4220      
#define VCC 3.3      
#define MOSFET_PIN 9
#define TARGET_TEMP 25
#define SAFETY_MAX_TEMP 35

float kp = 1.58;
float ki = 1.34;
float kd = 0.12;
unsigned long previousTime; 
float previousError = 0;
float p, i, d;
float getNtcTemperature()
{
  float average = 0;
  for(int i = 0; i < 5; i ++)
  {
    average += analogRead(THERMISTOR_PIN);
    delay(50);
  }
  average /= 5.0;

  float resistance = SERIES_RESISTOR / ((4095.0/average) - 1.0);

  float steinhart;
  steinhart = resistance / THERMISTOR_NOMINAL;
  steinhart = log(steinhart);
  steinhart /= B_COEFFICIENT;
  steinhart += 1.0 / (TEMPERATURE_NOMINAL + 273.15);
  steinhart = 1.0 / steinhart;
  steinhart -= 273.15;
  
  return steinhart;

}

void setup() {

  Serial.begin(9600);
  pinMode(THERMISTOR_PIN, INPUT);
  pinMode(MOSFET_PIN, OUTPUT);
  digitalWrite(MOSFET_PIN, LOW);
  analogWrite(MOSFET_PIN, 0);
  previousTime = millis();
  Serial.print("Heating Subsystem ready!");
  
}

void loop() {
  unsigned long currentTime = millis();
  float dt = (currentTime - previousTime) / 1000.0;
  
  if ( dt >= 1.0)
  {
  float temp = getNtcTemperature();
  float error = TARGET_TEMP - temp;

  p = kp * error;

  i += error * dt;

  i = constrain(i, -50, 50);
  float integral = ki * i;

  d = (error - previousError) /dt;
  float derivative = kd * d;

  float pidOutput = p + integral + derivative;

  float pwmValue = constrain(pidOutput, 0, 255);

  if (temp > SAFETY_MAX_TEMP) {
      pwmValue = 0;
      integral = 0; 
      Serial.print("!!! EMERGENCY SHUTOFF !!! ");
    }
    analogWrite(MOSFET_PIN, pwmValue);

  float powerPercent = (pwmValue / 255.0) * 100.0;
    
    Serial.print(currentTime / 1000);
    Serial.print("s | ");
    Serial.print(temp, 2);
    Serial.print("°C | ");
    Serial.print(error, 2);
    Serial.print("° | ");
    Serial.print(p, 1);
    Serial.print(" | ");
    Serial.print(integral, 1);
    Serial.print(" | ");
    Serial.print(derivative, 1);
    Serial.print(" | ");
    Serial.print(pwmValue);
    Serial.print(" | ");
    Serial.print(powerPercent, 0);
    Serial.println("%");
    
  previousError = error;
  previousTime = currentTime;
  }  
 
}
