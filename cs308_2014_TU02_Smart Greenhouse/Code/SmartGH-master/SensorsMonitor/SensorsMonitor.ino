#include "DHT.h"

const int dhtPin0 = 2;
DHT dht0(dhtPin0, DHT11);
const int lightPin0 = A0;

const int dhtPin1 = 4;
DHT dht1(dhtPin1, DHT11);
const int lightPin1 = A1;

void setup() {
  dht0.begin();
  dht1.begin();
  
  Serial.begin(9600);
  analogReference(DEFAULT);
}

void loop() {
  float l, t, h;
  
  l = analogRead(lightPin0) / 10.24;
  t = dht0.readTemperature();
  h = dht0.readHumidity();

  Serial.print(int(t));
  Serial.print("\t");
  Serial.print(int(l));
  Serial.print("\t");
  Serial.print(int(h));
  Serial.print("\t");

  l = analogRead(lightPin1) / 10.24;
  t = dht1.readTemperature();
  h = dht1.readHumidity();

  Serial.print(int(t));
  Serial.print("\t");
  Serial.print(int(l));
  Serial.print("\t");
  Serial.print(int(h));
  Serial.print("\t");
  
  Serial.println();
}
