/*
  To upload through terminal you can use: curl -u admin:admin -F "image=@firmware.bin" esp8266-webupdate.local/firmware
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include "PrivateAccounts.h"

//#ifndef STASSID
//#define STASSID "Calzone"
//#define STAPSK  ""
//#endif

const char* host = "esp8266-webupdate";
const char* update_path = "/firmware";
const char* update_username = UPDATEUSER;
const char* update_password = UPDATEPW;

const char* ssid1 = WLAN1GSSID;
const char* password1 = WLAN1GPSK;

const char* ssid2 = WLAN2GSSID;
const char* password2 = WLAN2GPSK;

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

void setup(void) {

  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");
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

  MDNS.begin(host);

  httpUpdater.setup(&httpServer, update_path, update_username, update_password);
  httpServer.begin();

  MDNS.addService("http", "tcp", 80);
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local%s in your browser and login with username '%s' and password '%s'\n", host, update_path, update_username, update_password);
}

void loop(void) {
  httpServer.handleClient();
  MDNS.update();
 // Serial.println("running...");



 
}
