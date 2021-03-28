const int analogPin = A0; 
int sensorValue = 0;
int percentage = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  sensorValue = analogRead(analogPin);
  percentage = map(sensorValue, 0, 1023, 0, 100);
  Serial.println(sensorValue);
  Serial.println(percentage);
  delay(1000);

}
