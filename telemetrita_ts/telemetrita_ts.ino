#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <DHT.h>

RF24 radio(9, 8); // CE, CSN
const byte address[6] = "00001";
char pesan[32]; // Pesan yang dikirimkan
DHT dht(3, DHT22); // DHT22 sensor connected to pin 3
const int trigPin = 5; // Ultrasonic sensor trigger pin
const int echoPin = 4; // Ultrasonic sensor echo pin

void setup()
{
    Serial.begin(9600);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    if (radio.begin())
    {
        Serial.println("Koneksi Arduino -> nRF24L01 SUKSES");
    }
    else
    {
        Serial.println("Koneksi Arduino -> nRF24L01 GAGAL || Periksa Sambungan");
        while (1) {}
    }
    radio.openWritingPipe(address);
    radio.setDataRate(RF24_2MBPS);
    radio.setPALevel(RF24_PA_MIN, 0);
    radio.stopListening(); // TX Mode
    Serial.println("TX Mode");
    dht.begin();
}

void loop()
{
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // Ultrasonic sensor code
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    float duration = pulseIn(echoPin, HIGH);
    float ultrasonicDistance = (duration * 0.0343) / 2;

    // Convert float values to strings
    dtostrf(humidity, 4, 2, pesan);
    strcat(pesan, "_");
    dtostrf(temperature, 4, 2, pesan + strlen(pesan));
    strcat(pesan, "_");
    dtostrf(ultrasonicDistance, 4, 2, pesan + strlen(pesan));

    Serial.print("MENGIRIM DATA: ");
    Serial.println(pesan);
    radio.write(&pesan, sizeof(pesan));
    Serial.println("SUKSES");
    delay(1000); // Delay for a second before sending again
}
