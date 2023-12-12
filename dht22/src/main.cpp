#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>

const byte LEDPIN = 13;
const byte DHTPIN = 8;
const byte DHTTYPE = 22;

#define SIGNALS_IN_DAY 5760
#define SECOND 1000

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
uint32_t signalNumber = 0;

void setupSerialOutput()
{
  Serial.begin(115200);
}

void setupBuilinLed()
{
  pinMode(LEDPIN, OUTPUT);
}

void setupDhtSensor()
{
  dht.begin();
}

void tempSensorRead()
{
  digitalWrite(LEDPIN, HIGH);
  Serial.print("\nSignal number: ");
  Serial.println(signalNumber);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature))
  {
    Serial.println(F("Error reading temperature!"));
  }
  else
  {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity))
  {
    Serial.println(F("Error reading humidity!"));
  }
  else
  {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }
  signalNumber++;
  if (signalNumber == SIGNALS_IN_DAY)
    signalNumber = 0;
  digitalWrite(LEDPIN, LOW);
}

void setup()
{
  setupSerialOutput();
  setupBuilinLed();
  setupDhtSensor();
}

void loop()
{
  delay(4 * SECOND);
  tempSensorRead();
}