//Libraries
#include <DHT.h>;
#include <ESP8266WiFi.h>
#include <CayenneMQTTESP8266.h>;

//Constants
#define CAYENNE_PRINT Serial
#include <CayenneMQTTESP8266.h>
#define DHTPIN D4         // * antes 14 what pin we're connected to
#define DHTTYPE DHT11     // * DHT 11 
DHT dht(DHTPIN, DHTTYPE);   // Initialize DHT sensor for normal 16mhz Arduino

// replace with your channel thingspeak API key and your SSID and password
String apiKey = "";  // * thingspeak API key
const char* server = "api.thingspeak.com";  // *
// Existing WiFi network
char ssid[] = "";    //* ssid
char wifiPassword[] = ""; //* pass

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "MQTT Usernane"; //* MQTT Usernane";
char password[] = "MQTT Password"; //* MQTT Password";
char clientID[] = "Client ID"; //* Client ID";

unsigned long lastMillis = 0;
WiFiClient client;  // *
const int sleepSeconds = 1800;  // * sleep for this many seconds, 30 min = 1800

void setup() {
  Serial.begin(9600);
  delay(10);  // *
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  dht.begin();
  WiFi.begin(ssid, wifiPassword); // *
  Serial.println(); // * saca por pto serie
  Serial.println(); // *
  Serial.print("Connecting to "); // *
  Serial.println(ssid);   // *
 
  WiFi.begin(ssid, wifiPassword); // *
  while (WiFi.status() != WL_CONNECTED)   // *
  {
    delay(500); // *
    Serial.print(".");  // *
  }
  Serial.println(""); // *
  Serial.println("WiFi connected"); // *
  pinMode(D0, WAKEUP_PULLUP);  // * Connect D0 to RST to wake up
}

//Variables
float h;  //Stores humidity value
float t; //Stores temperature value

void loop(){
    Cayenne.loop();
    delay(2000);
    //Read data and store it to variables h and t
    h = dht.readHumidity();
    t= dht.readTemperature();
    if (isnan(h) || isnan(t))   // * bucle hasta obtener medida
    {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    Cayenne.celsiusWrite(2, t);
    Cayenne.virtualWrite(1, h);
    Serial.println("Datos enviados a Cayenne!");

 
    if (client.connect(server,80)) {
    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String(t);
    postStr +="&field2=";
    postStr += String(h);
    postStr += "\r\n\r\n";
 
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    Serial.println("Datos enviados a Thingspeak!");
}
client.stop();
 

Serial.printf("Sleep for %d seconds\n\n", sleepSeconds);
    ESP.deepSleep(sleepSeconds * 1000000); // * 30 min retardo entre lecturas
}