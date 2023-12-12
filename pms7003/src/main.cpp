#include "PMS.h"
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>

SoftwareSerial mySerial(8, 7); // Arudino Uno port RX, TX

PMS pms(mySerial);
PMS::DATA data;

void setup()
{
  Serial.begin(115200);
  mySerial.begin(9600);
}

void loop()
{
  if (pms.read(data))
  {
    Serial.print("PM 1.0 (ug/m3): ");
    Serial.println(data.PM_AE_UG_1_0);

    Serial.print("PM 2.5 (ug/m3): ");
    Serial.println(data.PM_AE_UG_2_5);

    Serial.print("PM 10.0 (ug/m3): ");
    Serial.println(data.PM_AE_UG_10_0);

    Serial.println();
  }
}