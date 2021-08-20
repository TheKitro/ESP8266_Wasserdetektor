/*
  To upload through terminal you can use: curl -u admin:admin -F "image=@firmware.bin" esp8266-webupdate.local/firmware

  Eingesetzt wird der NodeMCU Lolin V3
  GPIO2/D4 ist mit der eingebauten LED verbunden
  Wichtig keine Delay() verwenden, da sonst OTA Webupdate nicht funktioniert.
  In der PrivateAccounts.h Datei können Kennwörter festgelegt werden.
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>



#include "PrivateAccounts.h"

#define MQTT_LOG_ENABLED 1
#include <MqttClient.h>

//Setup DHT22
#include <DHT.h>
#define DHT_TYPE DHT22
 


//#region variables
const char* host = "esp8266-webupdate";
const char* update_path = "/firmware";
const char* update_username = UPDATEUSER;
const char* update_password = UPDATEPW;

const char* ssid1 = WLAN1GSSID;
const char* password1 = WLAN1GPSK;

const char* ssid2 = WLAN2GSSID;
const char* password2 = WLAN2GPSK;

int pinWasserLvl1 = 5;  //D1
int pinWasserLvl2 = 4;  //D2
int pinWasserLvl3 = 0;  //D3
int pinTestTaster = 16; //D0 
  
int pinTestLED = 2;     //D4 Eingebaute LED
int pinBuzzer = 14;     //D5

int pinDHT22 = 12;      //D6
bool onTest = false;

DHT dht(pinDHT22, DHT_TYPE);

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;


//#endregion

//#region functions
void setup(void) 
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");
    

//ToDo Konfigurationsdatei einlesen, die über OTA hochgeladen wurde und den Variablen zuweisen. 

//#region DHT22
dht.begin();
//#endregion
  
//#region Wifi
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid1, password1);
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED) 
  {
    Serial.printf("WiFi 1 '%s' failed, retrying.\n", ssid1);
    WiFi.begin(ssid2, password2);
    
    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    { 
      Serial.printf("WiFi 2 '%s' failed, retrying.\n", ssid2); 
      WiFi.begin(ssid1, password1);
    }    
  }
  
  Serial.printf("WiFi Connected: '%s'\n", WiFi.SSID());
//#endregion


//#region OTA Web  
  MDNS.begin(host);

  httpUpdater.setup(&httpServer, update_path, update_username, update_password);
  httpServer.begin();

  MDNS.addService("http", "tcp", 80);
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local%s in your browser and login with username '%s' and password '%s'\n", host, update_path, update_username, update_password);
//#endregion


//#region MQTT Client
//  MqttClient.begin("mqtt://things.ubidots.com", net);
//  matrix.begin();
//  connect();
//#endregion


//#region Wasserstandsprüfung
  pinMode(pinWasserLvl1,INPUT);
  pinMode(pinWasserLvl2,INPUT);
  pinMode(pinWasserLvl3,INPUT);
  pinMode(pinTestTaster,INPUT);  
  pinMode(pinTestLED,OUTPUT);
  pinMode(pinBuzzer,OUTPUT);  
//#endregion
}

void loop(void) 
{
//Check OTA Web
  httpServer.handleClient();
  MDNS.update();

//Check Water LVL1  
  ///ToDo Verdrahten und Code
  
//Check Water LVL2  
  ///ToDo Verdrahten und Code
  
//Check Water LVL3  
  ///ToDo Verdrahten und Code
  
//Check Temperatur
//Check Humidity  
 Serial.print("Temperatur: ");
  Serial.print(GetTemperature());
  Serial.print("°C, Luftfeuchtigkeit: ");
  Serial.print(GetHumidity());
  Serial.println("%");

  

//Check Test Button
  ///ToDo Taster mit 10K Widerstand verdrahten
  CheckTestButton();

//Baue Nachricht mit folgenden Informationen auf:
//Aktueller Zeitstempel
//Chip ID
//WLAN SSID
//WLAN Empfangsstärke (RSSI bzw. Powerlevel)
//Status Check Water LVL 1-3. (Low / High)
//Status Check Test Button.  (Low / High)
//Status aktuelle Temperatur und Luftfeuchtigskeit. (°C, %)
//Status aktueller Batterylevel. (%)

//Wenn Check Water LVL1 auf Low geht, aktiviere Buzzer alle 5 Sek für 0,5 Sek 
//Wenn Check Water LVL2 auf Low geht, aktiviere Buzzer alle 3 Sek für 1 Sek
//Wenn Check Water LVL3 auf Low geht, aktiviere Buzzer alle 0,5 Sek für 0,4 Sek

//Wenn ein Check Water LVL auf LOW geht, sende alle 60 Sekunden eine MQTT Nachricht.
//Wenn Test Button gedrückt wurde, sende eine MQTT Nachricht, lasse die LED leuchten, schreibe Nachricht in die Console.
//Wenn eine Std um ist, sende eine MQTT Nachricht.
//Gehe in den Schlafmodus bis 1Std um ist, der Test Button gedrückt wurde oder Check Water LVL1 LOW wird.

// Serial.println("running...");
}

// Gets the measured water level.
// 0 is for no water level.
short GetWaterLevel()
{
  // throw new NotImplementedException();
  return 0;
}

// Gets the measured temperature in °C.
float GetTemperature()
{ 
  return dht.readTemperature();
}

// Gets the measured humidity in %.
float GetHumidity()
{ 
  return dht.readHumidity();
}

void CheckTestButton()
{
  if (digitalRead(pinTestTaster)==HIGH)
  {
    digitalWrite(pinTestLED, HIGH);
    onTest = true;
  }

  if (onTest == true && digitalRead(pinTestTaster)==LOW)  
  {
    digitalWrite(pinTestLED, LOW);
    onTest = false;
  }
}
//#endregion
