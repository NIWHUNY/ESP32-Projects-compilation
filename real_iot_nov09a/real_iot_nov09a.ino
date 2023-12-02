#include "DHT.h"
#include "arduino_secrets.h"

#define DHTPIN 4     // Pin where the DHT11 is connected
#define DHTTYPE DHT11 // DHT type

DHT dht(DHTPIN, DHTTYPE);

#include "thingProperties.h"

void setup() {
  pinMode(2, OUTPUT);
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(100);

  // Initialize DHT sensor
  dht.begin();

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();

  // Read temperature and humidity from DHT11 sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (!isnan(temperature) && !isnan(humidity)) {
    // Update IoT Cloud variables with DHT11 readings
    suhu = temperature;
    humid = humidity;
    Serial.print("humid value: ");
    Serial.println(suhu);
    Serial.print("suhu value: ");
    Serial.println(humid);
    ArduinoCloud.update();
  }

  // Your code here
}

/*
  Since suhu is READ_WRITE variable, onsuhuChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onsuhuChange() {
  // Add your code here to act upon suhu change
  // For example, you could print the new value to Serial
  Serial.print("New suhu value: ");
  Serial.println(suhu);
}

void onhumidChange() {
  // Add your code here to act upon humid change
  // For example, you could print the new value to Serial
  Serial.print("New humid value: ");
  Serial.println(humid);
}

void onledChange() {
  // Add your code here to act upon led change
  if (led==1) {
    // Turn on LED
    digitalWrite(2 , HIGH);
  } if(led==0) {
    // Turn off LED
    digitalWrite(2 , LOW);
  }
}
