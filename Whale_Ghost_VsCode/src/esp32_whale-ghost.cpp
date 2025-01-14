//ID: uncomment on first Flash only !!!
#define NODE_NUMBER 12

// VERSION
#define VERSION 16

//Debug
// #define DEBUG 1
//#define DEBUG_MSG 1


// lib
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
// #include <EEPROM.h>

#ifdef DEBUG
#include "debug.h"
#endif

unsigned long lastRefresh = 0;
#define REFRESH 10

unsigned long lastInfo = 0;
#define INFOTIME 500

#if defined(ARDUINO) && ARDUINO >= 100
// No extras
#elif defined(ARDUINO) // pre-1.0
// No extras
#elif defined(ESP_PLATFORM)
#include "arduinoish.hpp"
#endif


#define HBSIZE 32
char nodeName[HBSIZE];
byte myID;
byte mySUBNET;

const int MTUu = 1472;  // Usable MTU (1500 - 20 IP - 8 UDP)
unsigned char incomingPacket[MTUu];  // buffer for incoming packets

const int INFO_TIME = 500;
unsigned long lastUpdate = 0;

void wifi_event(WiFiEvent_t event);

#include "EEPROM_fonction.h"
#include "wifi_fonction.h"
#include "ota.h"
#include "leds.h"

// SETUP
void setup() {
#ifdef DEBUG
  Serial.begin(115200);
#endif

  // NODE ID
#ifdef NODE_NUMBER
  eeprom_setID((byte)NODE_NUMBER);
#endif

  // NAME
  myID = eeprom_getID();
  ssid_init();
  String myName("Hwhale");
  sprintf(nodeName, "Hwhale %02i %i", myID, VERSION);
#ifdef DEBUG
  Serial.print("Starting ");
  Serial.println(nodeName);
#endif

  // WIFI
  wifi_init();

  // OTA
  ota_setup();

  // LEDS
  leds_init();
  leds_test();

}


// LOOP
void loop() {

  // UPDATE with data received
  if ( wifi_read(incomingPacket) )
    if (incomingPacket[0] == myID)
      leds_set( incomingPacket );

  // LEDS SHOW
  if ((millis() - lastRefresh) > REFRESH) {
    if (!wifi_isok()) black_out();
    leds_show();
    lastRefresh = millis();
  }

  // INFO HeartBeat
  if ((millis() - lastInfo) > INFOTIME) {
    wifi_send((uint8_t*) nodeName, HBSIZE );
    lastInfo = millis();
  }

  // MILLIS overflow protection
  if (millis() < lastRefresh) {
    lastRefresh = millis();
    lastInfo = millis();
  }

  // OTA
  ota_loop();
}
