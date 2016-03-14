#if defined(ENABLEDISCBRAKE)
void setupDiscBrake()
{
    //Brake1
    pinMode(PINSERVOBRAKE1, OUTPUT);
    //Brake2
    pinMode(PINSERVOBRAKE2, OUTPUT);

    Servo servoDiscBrake1;
    Servo servoDiscBrake2;
    servoDiscBrake1.attach(PINSERVOBRAKE1, servoMinPWM, servoMaxPWM);
    servoDiscBrake2.attach(PINSERVOBRAKE2, servoMinPWM, servoMaxPWM);
}


void readDiscCurrentSensor()
{
  Serial.print("Reading Sensor: ");
  Serial.println(analogRead(A0));
}

int readAnalogSensorPin(byte pin)
{
  byte measurements = 5;
  float totalMeasurement = 0;
  for (byte i = 0; i < measurements; i++)
  {
    totalMeasurement += analogRead(A0);
    delay(1);
  }
  return totalMeasurement / measurements;
}
#endif

