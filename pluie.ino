
const int capteur = A0;
float p=0;

void setup()
{
    pinMode(capteur, INPUT);
    Serial.begin(115200);
}

void loop()
{
   p = analogRead(capteur);
   static char pluie[7];
   dtostrf(p, 6, 2, pluie); 
   Serial.print("Analog value : ");
   Serial.println(p); 
   Serial.println("");
    delay(1000);
}
