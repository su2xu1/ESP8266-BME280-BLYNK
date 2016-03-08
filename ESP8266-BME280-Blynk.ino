//Example>Blynk>BoardAndShield>ESP8266StandAlone
//
/**************************************************************
 * Blynk is a platform with iOS and Android apps to control
 * Arduino, Raspberry Pi and the likes over the Internet.
 * You can easily build graphic interfaces for all your
 * projects by simply dragging and dropping widgets.
 *
 *   Downloads, docs, tutorials: http://www.blynk.cc
 *   Blynk community:            http://community.blynk.cc
 *   Social networks:            http://www.fb.com/Blynkapp
 *                               http://twitter.com/blynk_app
 *
 * Blynk library is licensed under MIT license
 * This example code is in public domain.
 *
 **************************************************************
 * This example runs directly on ESP8266 chip.
 *
 * You need to install this for ESP8266 development:
 *   https://github.com/esp8266/Arduino
 * 
 * Please be sure to select hte right ESP8266 module
 * in the Tools -> Board menu!
 *
 * Change WiFi ssid, pass, and Blynk auth token to run :)
 *
 **************************************************************/
//Pragmas
//#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
//#define BLYNK_DEBUG //For debug

//Include Headers
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <BME280_MOD-1022.h>
#include <Wire.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "*************";

unsigned long lastCheck = 0;
double tempMostAccurate, humidityMostAccurate, pressureMostAccurate;
char buff[50];

// Arduino needs this to bring pretty numbers
void printFormattedFloat(float x, uint8_t precision) {
  char buffer[10];
  dtostrf(x, 7, precision, buffer);
  Serial.print(buffer);
}
void formattedFloat(float x, uint8_t precision, char *buff) {
  dtostrf(x, 7, precision, buff);
}

void setup()
{
  Serial.begin(115200);
#ifdef BLYNK_DEBUG
  Serial.println("");
  Serial.println("Begin Serial");
#endif
  Blynk.begin(auth, "ssid", "password");//ssid,pass
#ifdef BLYNK_DEBUG
  Serial.println("Begin Wifi");
  Serial.println("Begin BME280");
#endif
  Wire.begin();
  pinMode(12,OUTPUT);
  
  //BME280 Setup
  // need to read the NVM compensation parameters
  BME280.readCompensationParams();
  BME280.writeStandbyTime(tsb_0p5ms);        // tsb = 0.5ms
  BME280.writeFilterCoefficient(fc_16);      // IIR Filter coefficient 16
  BME280.writeOversamplingPressure(os16x);    // pressure x16
  BME280.writeOversamplingTemperature(os2x);  // temperature x2
  BME280.writeOversamplingHumidity(os1x);    // humidity x1
  BME280.writeMode(smNormal);
   
}

BLYNK_READ(V0)
{
  tempMostAccurate = BME280.getTemperatureMostAccurate();
#ifdef BLYNK_DEBUG
  //Serial.print("Temp ");
  //printFormattedFloat(tempMostAccurate, 2);
  //Serial.println();
#endif
  char buff1[50];
  formattedFloat(tempMostAccurate, 2, buff1);
  Blynk.virtualWrite(V0, buff1);
}

BLYNK_READ(V1)
{
  humidityMostAccurate = BME280.getHumidityMostAccurate();
#ifdef BLYNK_DEBUG
  //Serial.print("humid ");
  //printFormattedFloat(humidityMostAccurate, 2);
  //Serial.println();
#endif
  char buf[6];
  dtostrf(humidityMostAccurate,3,2,buf);
  Blynk.virtualWrite(V1, String(buf));
}

BLYNK_READ(V2)
{
  pressureMostAccurate = BME280.getPressureMostAccurate();
#ifdef BLYNK_DEBUG
  //Serial.print("pressure ");
  //printFormattedFloat(pressureMostAccurate, 2);
  //Serial.println();
#endif
  char buf8[8];
//  formattedFloat(pressureMostAccurate, 2, buff);
  dtostrf(pressureMostAccurate,5,2,buf8);
  Blynk.virtualWrite(V2, String(buf8));
}


void loop()
{
  Blynk.run();
  
  int diff = millis() - lastCheck;
  if (diff > 1000) {
    while (BME280.isMeasuring()) {
    }
    // read out the data - must do this before calling the getxxxxx routines
    BME280.readMeasurements();
    
    //For Debugging, Serial Print Temp+Humid+Pressure
#ifdef BLYNK_DEBUG
    tempMostAccurate = BME280.getTemperatureMostAccurate();
    Serial.print("Temp:");
    printFormattedFloat(tempMostAccurate, 2);    
    humidityMostAccurate = BME280.getHumidityMostAccurate();
    Serial.print(" Humid:");
    printFormattedFloat(humidityMostAccurate, 2);
    pressureMostAccurate = BME280.getPressureMostAccurate();
    Serial.print(" Pressure:");
    printFormattedFloat(pressureMostAccurate, 2);
    Serial.println();
#endif

    lastCheck = millis();
  } else if (diff < 0) {
    lastCheck = 0;
  }
}

