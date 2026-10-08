# WeatherStationESP

A DIY weather station project for Arduino and ESP8266, built around the DHT22 temperature/humidity sensor and the PMS7003 particulate matter (PM) laser sensor. Data is sent over MQTT (for Home Assistant integration).

## Welcome, everyone!

I got tired of buying (or receiving as gifts) off-the-shelf Chinese weather stations that were always hard to use — mostly because of problems with reading values from a distance (LCD viewing angles) or the higher price of units that include PM dust measurement. So I decided on a DIY approach and built my own weather station that would be more useful for sharing its data with other systems.

My goals were:

- create sensor units that can be placed wherever I want, in whatever quantity fits my needs
- keep power consumption low enough to run the units on battery
- use the sensor data in ways beyond just displaying it on a screen
- have one codebase that works with the PM sensor alone, the dust sensor alone, or both at the same time
- learn more about Arduino and ESP8266 devices in practice by solving real problems

## Preparation

Hardware I bought:

- 3x ESP8266 units (LoLin NodeMCU v3, to be precise)
- 3x DHT22 temperature/humidity sensors
- 1x PMS7003 PM dust laser sensor
- 1x PMS7003 JST data adapter (very handy, as it turns out later)
- some jumper cables
- 3x 5V 0.8A power supplies
- 3x sockets matching the power supply plugs
- 3x plastic enclosures

For Arduino I used its clone: a Funduino Uno.

### Coding part

To give myself more room to experiment with Arduino and ESP, I decided on the following approach:

- separate project for the DHT22 on Arduino
- separate project for the PMS7003 on Arduino
- separate project for both sensors on Arduino
- final project: weather station sensor on ESP8266 with all communication features

Since I prefer VS Code to the Arduino IDE, I used VS Code with the PlatformIO extension. All libraries (GitHub URLs), baud rates, and board names used in the mentioned projects are listed in the `platformio.ini` file.

### DHT22 and PMS7003 separately

The two projects live in:

- `dht22` folder
- `pms7003` folder

First, I wanted to see how the DHT22 and PMS7003 sensors work on their own with Arduino, and get some hands-on practice. The DHT22 is a no-brainer: just connect 3V3 and GND to the labeled pads on the board, and wire the sensor's data pin to any digital input pin.
Avoid the PWM-capable output pins marked with `~` on the board! I'm not entirely sure why (largely due to my own lack of knowledge).
If you know why, feel free to open a PR to this documentation with an explanation! :)

### PMS7003 story

The PMS7003 is a different story. First, I had trouble connecting the sensor to the board using its included 10-pin SMD female header with a 1.27 mm pitch. It's very small and fiddly to work with. After some experience soldering cables to the plug, I found that the plug needs to be soldered to the board first to make the connection more rigid. The board's pitch is also smaller than the standard 2.54 mm, which made it hard to source locally at my electronics store either.

After a few more attempts with this plug, I bought the JST adapter for the sensor — a decision that paid off, because the plug has to sit snugly in its socket: even tiny movements can break the connection between the microcontroller board and the sensor.

For wiring, I used pins 8 and 7 for the RX/TX serial connection, handled by the SoftwareSerial library. The sensor runs perfectly on both 5 V and 3 V. I didn't use any capacitors on the power line.

### Make it work together

Project folder: `weather-station-uno`

The next step was to combine both sensors on the Arduino. Basically, I used digital pin 2 for the DHT22 and pins 8 and 7 for the RX/TX serial connection, as before. This time I used passive mode for the PMS7003 to save power — in that mode the sensor draws more current only between `wakeUp()` and `sleep()`.

I also learned that the PMS sensor works best when its readings aren't interrupted by `delay()` calls, so delays are only used while the sensor is sleeping and right after wake-up for the initial fan spin-up.

After the wake-up delay, I take 10 readings from all sensors and calculate the average.

### WiFi connection and MQTT with ESP8266

Project folder: `weather-station-esp`

At this point I wanted to clean up the Arduino code and port it over to the ESP8266.

#### Problems with uploading code

My first problem was uploading code to one of my ESPs. On one board everything worked fine with the board type set to `nodemcu`, but on another I ran into issues.

I saw some strange output in the serial monitor, so I started experimenting with the baud rate. At one point I set it to `74880` and got a more "human-readable" error message:

```
load 0x4010f000, len 1392, room 16 
tail 0
chksum 0xd0
csum 0xd0
v3d128e5c
~ld
```

The error message didn't really tell me much (again, a lack of knowledge on my part), so I went back to the Arduino IDE — and once I changed the board type there, it started working! The board type I now use in PlatformIO is `nodemcuv2`.

Just for the record: the boards looked exactly identical — I couldn't spot any visible difference between them.

#### Changes in code in comparison to arduino version

TODO

#### WiFi connection and MQTT communication

TODO

## Problems after assembly

TODO
