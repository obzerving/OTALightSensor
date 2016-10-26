OTALightSensor

A wifi-enabled Light sensor with restful web service.

Hardware Used:
- ESP8266-12 wifi module
- Adafruit TSL2561 sensor

Software Used (Note: Versions may have changed)
- Arduino development environment for ESP8266 (see https://github.com/esp8266/Arduino)
- Adafruit TSL2561 Driver v2.0 (Unified Sensor Driver) (see https://learn.adafruit.com/tsl2561/downloads)
- aREST library, which implements a REST API for Arduino & the ESP8266 WiFi chip (see https://github.com/marcoschwartz/aREST)

Work Flow

1. Install the development environment

2. Install the TSL2561 libraries and the aREST library

3. Change the OTALightSensor.ino file accordingly. I strongly suggest these:

   a. "your_ssid" to your wifi ssid

   b. "your_password" to your wifi password

   c. "front-light-sensor" to a name that makes sense for you sensor location

4. Wire up the ESP8266 module and the sensor. The components I used require 3.3 volts and do not tolerate 5 volts. It's possible to find modules that can run safely at voltages higher than 3.3 volts, I didn't use them. Be careful.

5. Program the ESP8266 module. I highly recommend not having the light sensor module connected during programming. My components are mounted in sockets, so I can do that easily. Further, socket mounting these components is a good practice.

6. Reset the ESP8266 and it should run the program. If you send it a URI of this form:

	http://IP.Address.Of.Sensor/luminance

You should see a JSON response of this form:

	{“luminance”: 00, “id”: “1”, name: “front-light-sensor”, “connected”: true}
	
where	"luminance" is the variable name
		00 will be the actual value (in lux) of the light measured by the sensor
		"id" corresponds to the ID of the device
		"1" will be whatever value you gave it in the program (e.g. rest.set_id("1"))
		name is the name of the device
		"front-light-sensor" will be whatever name you gave it in the program (e.g. rest.set_name("front-light-sensor"))

It's also possible to find out what the value of the supply voltage is (in case you decide to run it on a battery)

	http://IP.Address.Of.Sensor/voltage

NOTE: At this time, there is a bug in aREST, such that sending the URI with a browser will crash the program. The reason appears to be related to the amount of header information that is sent back by the browser. I have had no problems using libcurl in my programs or curl from the command line (see https://curl.haxx.se/).
Command line example:
	curl http://IP.Address.Of.Sensor/luminance