/* 
  Light sensor with restful web service.
  Written for ESP8266 ESP-12, and Adafruit TSL2561 sensor

   Connections
   ===========
   Connect SCL to analog 5
   Connect SDA to analog 4
   Connect VDD to 3.3V DC
   Connect GROUND to common ground

   I2C Address
   ===========
   The address will be different depending on whether you leave
   the ADDR pin floating (addr 0x39), or tie it to ground or vcc. 
   The default addess is 0x39, which assumes the ADDR pin is floating
   (not connected to anything).  If you set the ADDR pin high
   or low, use TSL2561_ADDR_HIGH (0x49) or TSL2561_ADDR_LOW
   (0x29) respectively. 

   As of 1/1/2016, the reference to "pgmspace.h" in Adafruit_TSL2561_U.h
   needs to be changed to <pgmspace.h>
*/
// Import required libraries

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <aREST.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <pgmspace.h>
#include <Adafruit_TSL2561_U.h>

// Configure ADC for supply voltage readings
ADC_MODE(ADC_VCC);
// Create aREST instance
aREST rest = aREST();

const char* ssid = "your_ssid";
const char* password = "your_password";

// The port to listen for incoming TCP connections 
#define LISTEN_PORT           80

// Create an instance of the server
WiFiServer server(LISTEN_PORT);

// Variables to be exposed to the API
int luminance;
int voltage;

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 1);

void configureSensor(void)
{
  tsl.enableAutoRange(true); /* Auto-gain ... switches automatically between 1x and 16x */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS); /* 16-bit data but slowest conversions */
}

void setup() {
  Serial.begin(115200);
  Serial.println("OTA Light Sensor Booting");
  // Init variables and expose them to REST API
  luminance = 0;
  voltage = 0;
  rest.variable("luminance",&luminance);
  rest.variable("voltage",&voltage);

  /* Initialise the sensor */
  if(!tsl.begin())
  {
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  /* Setup the sensor gain and integration time */
  configureSensor();

  // Give name and ID to device
  rest.set_id("1");
  rest.set_name("front-light-sensor");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();

  // Start the rest server
  server.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  
}

void loop() {
  /* Get a new sensor event */ 
  sensors_event_t event;
  tsl.getEvent(&event);
  delay(1);
  /* Display the results (light is measured in lux) */
  if (event.light)
  {
    luminance = event.light;
//    Serial.print(event.light); Serial.println(" lux");
  }

  voltage = ESP.getVcc();
//  Serial.print(voltage); Serial.println(" volts");
  
  // Handle REST calls
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  while(!client.available()){
    delay(1);
  }
  rest.handle(client);
  delay(1);
  ArduinoOTA.handle();
}
