#include "PMS.h"
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <DHT.h>

#define SECOND 1000
#define DHTTYPE DHT22

const byte DHTPIN = 5;
const byte RXPIN = 21;
const byte TXPIN = 22;

const int pm1_0Index = 0;
const int pm2_5Index = 1;
const int pm10_0Index = 2;
const int tempIndex = 3;
const int humidityIndex = 4;

SoftwareSerial mySerial(RXPIN, TXPIN);
DHT dht(DHTPIN, DHTTYPE);

PMS pms(mySerial);
PMS::DATA data;

float sensorsData[10][5];

void setupPmsSensor()
{
  mySerial.begin(9600);
  pms.passiveMode();
  pms.wakeUp();
}

void setupDhtSensor()
{
  dht.begin();
}

void setup()
{
  Serial.begin(115200); // output
  setupPmsSensor();
  setupDhtSensor();
}

void readSensors()
{
  for (int i = 0; i < 10; i++)
  {
    // if (pms.readUntil(data))
    // {
    //   sensorsData[i][pm1_0Index] = data.PM_AE_UG_1_0;
    //   sensorsData[i][pm2_5Index] = data.PM_AE_UG_2_5;
    //   sensorsData[i][pm10_0Index] = data.PM_AE_UG_10_0;
    // }
    float test = dht.readTemperature();
    if (!isnan(test))
    {
      sensorsData[i][tempIndex] = test;
    }
    test = dht.readHumidity();
    if (!isnan(test))
    {
      sensorsData[i][humidityIndex] = test;
    }
  }
}

void printSensorsData()
{
  float calcPm1_0 = 0;
  float calcPm2_5 = 0;
  float calcPm10_0 = 0;
  float calcTemp = 0;
  float calcHumi = 0;

  for (int i = 0; i < 10; i++)
  {
    calcPm1_0 += sensorsData[i][pm1_0Index];
    calcPm2_5 += sensorsData[i][pm2_5Index];
    calcPm10_0 += sensorsData[i][pm10_0Index];
    calcTemp += sensorsData[i][tempIndex];
    calcHumi += sensorsData[i][humidityIndex];
  }
  calcPm1_0 = calcPm1_0 / 10.0;
  calcPm2_5 = calcPm2_5 / 10.0;
  calcPm10_0 = calcPm10_0 / 10.0;
  calcTemp = calcTemp / 10.0;
  calcHumi = calcHumi / 10.0;

  Serial.print("PM 1.0 (ug/m3): ");
  Serial.println(calcPm1_0);
  Serial.print("PM 2.5 (ug/m3): ");
  Serial.println(calcPm2_5);

  Serial.print("PM 10.0 (ug/m3): ");
  Serial.println(calcPm10_0);
  Serial.print("Temperature: ");
  Serial.print(calcTemp);
  Serial.println("°C");
  Serial.print("Humidity: ");
  Serial.print(calcHumi);
  Serial.println("%");

  memset(sensorsData, 0, sizeof sensorsData);
}

void loop()
{
  pms.wakeUp();
  pms.requestRead();

  readSensors();
  printSensorsData();
  Serial.println();

  pms.sleep();
  delay(12 * SECOND);
}