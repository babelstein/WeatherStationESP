#include <secrets.h>
#include <weatherStationReport.h>

#include "PMS.h"
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

/* CONFIGURATION */
#define SECOND 1000
#define DHTTYPE DHT22
#define MSG_BUFFER_SIZE (80)
const byte DHTPIN = D1;
const byte RXPIN = D5;
const byte TXPIN = D6;
const u_int numberOfReads = 20;

/* SECRETS */
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;
const char *mqttbroker = MQTT_ADDRESS;
const u_int mqttport = MQTT_PORT;
const char *mqttuser = MQTT_USER;
const char *mqttpswd = MQTT_PSWD;

const u_int pm1_0Index = 0;
const u_int pm2_5Index = 1;
const u_int pm10_0Index = 2;
const u_int tempIndex = 3;
const u_int humidityIndex = 4;

SoftwareSerial mySerial(TXPIN, RXPIN);
DHT dht(DHTPIN, DHTTYPE);

PMS pms(mySerial);
PMS::DATA data;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

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

void setupWiFiConnection()
{
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  // handle message arrived
}

void setupMqttConnection()
{
  mqttClient.setServer(mqttbroker, mqttport);
  mqttClient.setCallback(callback);
}

void setup()
{
  Serial.begin(115200); // output
  delay(1000);
  setupPmsSensor();
  setupDhtSensor();
  setupWiFiConnection();
  setupMqttConnection();
}

void readSensors()
{
  for (u_int i = 0; i < numberOfReads; i++)
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

  u_int numberOfZeros = 0;

  for (u_int i = 0; i < numberOfReads; i++)
  {
    if (sensorsData[i][pm1_0Index] <= 0 && sensorsData[i][pm2_5Index] <= 0 && sensorsData[i][pm10_0Index] <= 0)
    {
      numberOfZeros++;
      continue;
    }

    calcPm1_0 += sensorsData[i][pm1_0Index];
    calcPm2_5 += sensorsData[i][pm2_5Index];
    calcPm10_0 += sensorsData[i][pm10_0Index];
    calcTemp += sensorsData[i][tempIndex];
    calcHumi += sensorsData[i][humidityIndex];
  }

  calcPm1_0 = calcPm1_0 / (float)(numberOfReads - numberOfZeros);
  calcPm2_5 = calcPm2_5 / (float)(numberOfReads - numberOfZeros);
  calcPm10_0 = calcPm10_0 / (float)(numberOfReads - numberOfZeros);
  calcTemp = calcTemp / (float)(numberOfReads - numberOfZeros);
  calcHumi = calcHumi / (float)(numberOfReads - numberOfZeros);

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

void reconnect()
{
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("arduinoClient", mqttuser, mqttpswd))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void sendReport(weatherStationReport report)
{
  if (!mqttClient.connected())
  {
    reconnect();
  }

  String msg = "{\"pm1_0\":" + String(report.pm1_0) + "," +
               "\"pm2_5\":" + String(report.pm2_5) + "," +
               "\"pm10_0\":" + String(report.pm10_0) + "," +
               "\"temperature\":" + String(report.temperature) + "," +
               "\"humidity\":" + String(report.humidity) + "}";

  char payload[msg.length() + 1];
  memset(payload, 0, sizeof payload);

  for (u_int i = 0; i < msg.length(); i++)
  {
    payload[i] = msg[i];
  }
  mqttClient.publish("home/temp-dust-sensor", payload);
}

void loop()
{
  pms.wakeUp();
  delay(10 * SECOND);
  pms.requestRead();

  readSensors();
  struct weatherStationReport report = calculateSensorsData();
  printReport(report);
  sendReport(report);
  Serial.println();

  pms.sleep();
  delay(29 * SECOND);
}