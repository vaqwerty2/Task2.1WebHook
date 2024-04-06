#include <WiFiNINA.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#include "DHT.h"
char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

#define DHTPIN 2 // Define ditgital pin for DHT sensor
#define DHTTYPE DHT22 //Define type of DHT sensor (DHT22)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);      // Initialize serial 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  ThingSpeak.begin(client);  // Initialize ThingSpeak 
  dht.begin();
}

void loop() {

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
  // Data fields 
  float humidity = dht.readHumidity();
  float temperatureinC = dht.readTemperature();

  // Check
   if (isnan(humidity) || isnan(temperatureinC)) {
    Serial.println("Failed to read from DHT sensor! ");
    delay(2000);
    return;
  }

  // Data potting on ThingSpeak
  ThingSpeak.setField(1, temperatureinC);
  ThingSpeak.setField(2, humidity);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful. ");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  } 
  delay(60000); // Updates after every 60 seconds
}
