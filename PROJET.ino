#include <ESP8266WiFi.h> //bibliothèques pour le wifi
#include <PubSubClient.h>
#include "DHT.h" //bibliothèque du capteur

#define DHTTYPE DHT11     // DHT 11

// initialisation de nos variables

//const int capteur = A0; pluie
//float p=0; pluie 
const int analogPin = A0; //pour le capteur de luminosité
int sensorValue = 0; //luminosité
int percentage = 0; //luminosité
const int DHTPin = 5; //capteur humidité et température sur D1 soit pin 5

// Permet de se connecter au routeur choisi
const char* ssid = "The Lord of the Ping";
const char* password = "12345678";

// Se connecter au MQTT
const char* mqttServer = "iot.fr-par.scw.cloud";
const int mqttPort = 1883;
const char* mqttUser = "2212fcd6-7781-4f01-826a-c813ae44bbae";
const char* mqttPassword = "";

//initialisation de l'esp client
WiFiClient espClient;
PubSubClient client(espClient);

// Initialisation du DHT11
DHT dht(DHTPin, DHTTYPE);

// Initialisation des timers
long now = millis();
long lastMeasure = 0;

// Connexion au routeur (rien à changer)
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

// This functions is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
// Change the function below to add logic to your program, so when a device publishes a message to a topic that 
// your ESP8266 is subscribed you can actually do something
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println(); 
}

// Reconnection si déconnecté
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
      Serial.println("connected");  
      client.subscribe("room/temperature");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

//Defini les inputs et outputs et commence la connexion serial
void setup() {

  //pinMode(capteur, INPUT); pluie
  dht.begin();
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

//boucle du programme
void loop() {

  if (!client.connected()) { //vérifie que l'on est bien connecté
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client", mqttUser, mqttPassword );

  now = millis();
  if (now - lastMeasure > 30000) { //donne la température et l'humidité toutes les 30s
    lastMeasure = now;
    float h = dht.readHumidity();
    float t = dht.readTemperature(); //en celsius
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // si il y a un soucis alors il reconnecte
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Computes temperature values in Celsius
    float hic = dht.computeHeatIndex(t, h, false);
    static char temperatureTemp[7];
    dtostrf(hic, 6, 2, temperatureTemp);
    
    // Uncomment to compute temperature values in Fahrenheit 
    // float hif = dht.computeHeatIndex(f, h);
    // static char temperatureTemp[7];
    // dtostrf(hic, 6, 2, temperatureTemp);
    
    static char humidityTemp[7];
    dtostrf(h, 6, 2, humidityTemp);

    // Donne les valeurs sur le Node-Red
    client.publish("room/temperature", temperatureTemp);
    client.publish("room/humidity", humidityTemp);
    
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t Heat index: ");
    Serial.print(hic);
    Serial.println(" *C ");
    // Serial.print(hif);
    // Serial.println(" *F");


    //Même chose pour la luminosité
    sensorValue = analogRead(analogPin);
    float p = map(sensorValue, 0, 1023, 0, 100);
    static char percentage[7];
    dtostrf(p, 6, 2, percentage);
    client.publish("room/Luminosité", percentage);
    Serial.println(percentage);
    delay(1000);

    //p = analogRead(capteur);
    //static char pluie[7];
    //dtostrf(p, 6, 2, pluie); 
    //client.publish("room/pluie", pluie);
    //Serial.print("Analog value : ");
    //Serial.println(p); 
    //Serial.println("");


    //code deepsleep de 10min
    ESP.deepSleep(600e6);
    
    }

    
} 
