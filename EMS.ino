// $File: //ASP/Dev/SBS/4_Controls/4_4_Equipment_Control/4_4_3_Diagnostics_Control/sw/Weather_Station/ArduinoControlApp/weatherstation/weatherstation.ino $
// $Revision: #3 $
// $DateTime: 2017/02/08 17:42:05 $
// Last checked in by: $Author: raen $
//
// Description
// Arduino Weather Station Control
//
// Copyright (c) 2017 Australian Synchrotron
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// Licence as published by the Free Software Foundation; either
// version 2.1 of the Licence, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public Licence for more details.
//
// You should have received a copy of the GNU Lesser General Public
// Licence along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
//
// Contact details:
// simon.cunningham@synchrotron.org.au
// 800 Blackburn Road, Clayton, Victoria 3168, Australia.
//

#include <SPI.h>
#include <Ethernet.h>
#include <avr/wdt.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <RTClib.h>
#include <Adafruit_Sensor.h>

#define DHTPIN 8     // what pin we're connected to

#define DHTTYPE DHT22   // DHT 22  (AM2302)

RTC_Millis RTC;
DHT dht(DHTPIN, DHTTYPE);

//Adafruit_BMP085 bmp;
Adafruit_BMP085 bmp;
// Enter a MAC address and IP address for your controller below.
byte mac[] = {
  0x32, 0x06, 0xB0, 0xE3, 0xEC, 0xF7
};
//IPAddress ip(10, 17, 115, 201);
IPAddress ip(10, 17, 20, 17);
IPAddress gateway(10, 17, 0, 254);
IPAddress subnet(255, 255, 0, 0);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
  //  Serial.begin(9600);
  Serial.begin(9600);
  //initiate the various sensors and components
  Wire.begin();
  wdt_enable(WDTO_2S);
  dht.begin();
  bmp.begin();

  //  if (!bmp.begin()) {
  //	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  //	while (1) {}
  //  }

  // start the Ethernet connection and the server:
  //  Ethernet.begin(mac, ip);
  Ethernet.begin(mac, ip);
}

void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());


  float h = dht.readHumidity();
  float th = dht.readTemperature();
  float tp = bmp.readTemperature();
  float t = (dht.readTemperature() + bmp.readTemperature()) / 2;
  float p = bmp.readPressure();

  Serial.println();
  Serial.println ("{");
  Serial.print("\"HUMIDITY_SENSOR_TEMPERATURE_MONITOR\": ");
  Serial.print(th);
  Serial.println(",");
  Serial.print("\"PRESSURE_SENSOR_TEMPERATURE_MONITOR\": ");
  Serial.print(tp);
  Serial.println(",");
  Serial.print("\"TEMPERATURE_MONITOR\": ");
  Serial.print(t);
  Serial.println(",");
  Serial.print("\"HUMIDITY_MONITOR\": ");
  Serial.print(h);
  Serial.println(",");
  Serial.print("\"PRESSURE_MONITOR\": ");
  Serial.println(p);
  Serial.println("}");
  wdt_reset();

  EthernetClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          client.println();
          client.println ("{");
          client.print("\"HUMIDITY_SENSOR_TEMPERATURE_MONITOR\": ");
          client.print(th);
          client.println(",");
          client.print("\"PRESSURE_SENSOR_TEMPERATURE_MONITOR\": ");
          client.print(tp);
          client.println(",");
          client.print("\"TEMPERATURE_MONITOR\": ");
          client.print(t);
          client.println(",");
          client.print("\"HUMIDITY_MONITOR\": ");
          client.print(h);
          client.println(",");
          client.print("\"PRESSURE_MONITOR\": ");
          client.println(p);
          client.println("}");

          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }

}

// end


