#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

RF24 radio(4, 5); // CE, CSN
const byte address[6] = "00001";
int SUM_DATA = 0;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1     // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{
    Serial.begin(9600);
    if (radio.begin())
    {
        Serial.println("Koneksi ESP32 -> nRF24L01 SUKSES");
    }
    else
    {
        Serial.println("Koneksi ESP32 -> nRF24L01 GAGAL || Periksa Sambungan");
        while (1) {}
    }
    radio.openReadingPipe(0, address);
    radio.setDataRate(RF24_2MBPS);
    radio.setPALevel(RF24_PA_MIN, 1);
    radio.startListening(); // RX Mode
    Serial.println("RX Mode");

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }
    display.clearDisplay();
    display.setTextSize(1.2);
    display.setTextColor(WHITE);
}

void loop()
{
    if (radio.available())
    {
        SUM_DATA++;
        char text[32] = "";
        radio.read(&text, sizeof(text));
        Serial.println(text);

        // Parse received data
        char *token;
        float humidity, temperature, ultrasonicDistance;

        // Split the string into tokens using "_" as delimiter
        token = strtok(text, "_");
        humidity = atof(token);

        token = strtok(NULL, "_");
        temperature = atof(token);

        token = strtok(NULL, "_");
        ultrasonicDistance = atof(token);

        // Display on OLED
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("DATA:");
        display.print(SUM_DATA);

        display.setCursor(0, 10);
        display.print("Humidity:    ");
        display.print(humidity);
        display.print("%");

        display.setCursor(0, 30);
        display.print("Temperature: ");
        display.print(temperature);
        display.print("°C");

        display.setCursor(0, 50);
        display.print("Ultrasonic:  ");
        display.print(ultrasonicDistance);
        display.print(" cm");

        display.display();
    }
}
