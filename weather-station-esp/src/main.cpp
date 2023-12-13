#include "PMS.h"
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <DHT.h>

#define SECOND 1000
#define DHTTYPE DHT22

const byte DHTPIN = D1;
const byte RXPIN = D5;
const byte TXPIN = D6;

const int pm1_0Index = 0;
const int pm2_5Index = 1;
const int pm10_0Index = 2;
const int tempIndex = 3;
const int humidityIndex = 4;

SoftwareSerial mySerial(TXPIN, RXPIN);
DHT dht(DHTPIN, DHTTYPE);

PMS pms(mySerial);
PMS::DATA data;

struct weatherStationReport
{
  float pm1_0;
  float pm2_5;
  float pm10_0;
  float temperature;
  float humidity;
};

const int numberOfReads = 20;
float sensorsData[numberOfReads][5];

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
  for (int i = 0; i < numberOfReads; i++)
  {
    if (pms.readUntil(data))
    {
      sensorsData[i][pm1_0Index] = data.PM_AE_UG_1_0;
      sensorsData[i][pm2_5Index] = data.PM_AE_UG_2_5;
      sensorsData[i][pm10_0Index] = data.PM_AE_UG_10_0;
    }
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

weatherStationReport calculateSensorsData()
{
  float calcPm1_0 = 0;
  float calcPm2_5 = 0;
  float calcPm10_0 = 0;
  float calcTemp = 0;
  float calcHumi = 0;

  int numberOfZeros = 0;

  for (int i = 0; i < numberOfReads; i++)
  {
    calcPm1_0 += sensorsData[i][pm1_0Index];
    calcPm2_5 += sensorsData[i][pm2_5Index];
    calcPm10_0 += sensorsData[i][pm10_0Index];
    calcTemp += sensorsData[i][tempIndex];
    calcHumi += sensorsData[i][humidityIndex];
    if (sensorsData[i][pm1_0Index] == 0 || sensorsData[i][pm2_5Index] == 0)
      numberOfZeros++;
  }
  calcPm1_0 = calcPm1_0 / (float)numberOfReads - numberOfZeros;
  calcPm2_5 = calcPm2_5 / (float)numberOfReads - numberOfZeros;
  calcPm10_0 = calcPm10_0 / (float)numberOfReads - numberOfZeros;
  calcTemp = calcTemp / (float)numberOfReads;
  calcHumi = calcHumi / (float)numberOfReads;

  if (numberOfReads == numberOfZeros)
  {
    calcPm1_0 = 0;
    calcPm2_5 = 0;
    calcPm10_0 = 0;
  }
  struct weatherStationReport report;
  report.pm1_0 = calcPm1_0;
  report.pm2_5 = calcPm2_5;
  report.pm10_0 = calcPm10_0;
  report.temperature = calcTemp;
  report.humidity = calcHumi;

  memset(sensorsData, 0, sizeof sensorsData);
  Serial.println("Zeros: " + String(numberOfZeros));
  return report;
}

void printReport(weatherStationReport report)
{
  Serial.print("PM 1.0 (ug/m3): ");
  Serial.println(report.pm1_0);
  Serial.print("PM 2.5 (ug/m3): ");
  Serial.println(report.pm2_5);
  Serial.print("PM 10.0 (ug/m3): ");
  Serial.println(report.pm10_0);
  Serial.print("Temperature: ");
  Serial.print(report.temperature);
  Serial.println("°C");
  Serial.print("Humidity: ");
  Serial.print(report.humidity);
  Serial.println("%");
}

void loop()
{
  pms.wakeUp();
  delay(5 * SECOND);
  pms.requestRead();

  readSensors();
  struct weatherStationReport report = calculateSensorsData();
  printReport(report);
  Serial.println();

  pms.sleep();
  delay(30 * SECOND);
}