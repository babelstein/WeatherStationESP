# WeatherStationESP

Arduino/ESP8266 weather sensors built with DHT22 and PMS7003 sensors which passes data over MQTT (Home Assistant integration).

## Wellcome everybody!

Since i was bored with buying (or getting as present) another chinese weather stations which were always problematic to use, mostly because of problems with reading values from distance (LCD display view angles) or higher price for units with PM dust measure, I decided to go with DIY approach and create weather station which will be more usefull in terms of passing information from weather station to other systems.

Assumptions that I had:
- create weather station sensors that can be placed wherever I want and in quantity that fits my needs.
- power consumption low enough to have battery powered units
- use data from sensors in other ways than just displaying data on screen
- have one code for PM sensor unit or dust sensor unit or both at the same time
- learn more about Arduino and ESP8266 devices in practice when resolving issues

## Preparation

I've bought follwing hardware:

- 3x ESP8266 units (LoLin NodeMcu v3 - to be precise)
- 3x DHT22 temperature/humidity sensor
- 1x PMS7003 PM dust laser sensor
- 1x PMS7003 JST datapter (very handy as it will turn out later)
- some jump cables
- 3x 5V 0.8A power supplies
- 3x sockets matching to power supply plug
- 3x plasic cases

When it comes to arduino I used it's clone Funduino Uno.

### Coding part

To have more posibility to play arround with arduino and esp I've decided to go with following approach:

- separate project for DHT22 for arduino
- separate project for PMS7003 for arduino
- separate project for both sensors for ardunio
- final project for weather station sensor on esp8266 with all communication features

Because I like VS Code IDE more than Arduino IDE I was using VS Code with Platformio extension. All of libraries (github urls), baud rates and board names used in mentioned projects are listed in `platformio.ini` file.

### DHT22 and PMS7003 separately

Both projects are placed in:
- `dht22` folder
- `pms7003` folder

First of all I wanted to see how DHT22 and PMS7003 sensors works separately with arduino and have some practice with it. With DHT22 it's no brainer. Just connect 3V and GND to decribed slots on board and connect sensor data pin to some input pins. 
Avoid PWM output pins described with `~` on board! Not quite sure why (mostly because my lack of knowledge).
If you know why please make PR to this documentation with explanation :)

### PMS7003 story 
With PMS7003 it's a different story. First of all I had problem with connecting sensor to board with included to sensor SMD 10 pin female plug 1,27mm raster. It's very tiny and precise stuff to play with. After some work with soldering calbes to plug it turned out that plug should be soldered to board first to make it more rigid. Since borad raster is also smaller than usual 2,54mm raster so it was hard to get as well in my local electronics store.

After another tries with this plug i decided to buy JST adapter for this sensor and it was correct decision since plug had to be well placed in socked since even small moves could break connection between microcontroller board and sensor.

Pinout for PMS7003 was pins 8 and 7 for RX and TX serial communication with usage of SoftwareSerial library. This sensor can run with 5V and 3V perfectly well. I wasn't using any capacitor for voltage line.

### Make it work together

Project folder: `weather-station-uno`

Next step was to put both of sensors together in arduino. Basicaly I've used 2nd pin for DHT22 and pins 8 and 7 for RX, TX serial communication as before. This time I tried to use pasive mode for PMS7003 which can save some energy since sensor will consume bigger ammount of energy only between `wakeUp()` and `sleep()` function calls.'

Also I've learned that PMS sensor works better when reads are not interupted with `delay()` function. So delays are only used when sensor is asleep and after waking it up for initial fan spin.

After wake up delay I'm taking 10 reads of all sensors and calculating average result.

### WiFi connection and MQTT with ESP8266

Project folder: `weather-station-esp`

This time I wanted to improve arduino code and migrate it to ESP8266.

#### Problems with uploading code

First problem I had was the issue with uploading code to one of my ESPs. On one everything worked just fine with board name set to `nodemcu` but on another there were problems.

I've seen some strange output on serial monitor and started to play with baud rate. Once I've set it to `74880` i've received some "human friendly" error like:

```
load 0x4010f000, len 1392, room 16 
tail 0
chksum 0xd0
csum 0xd0
v3d128e5c
~ld
```

Since this error message did not told me much (again, lack of knowledge) I've decided to play arround in Arduino IDE and after I've changed board type it started to work! Board name I am using in platformio now is `nodemcuv2`.

Just for a record. Boards looked exactly the same way. I could not see any visible differences between them.

#### Changes in code in comparison to arduino version

TODO

#### WiFi connection and MQTT communication

TODO

## Problems after assembly

TODO