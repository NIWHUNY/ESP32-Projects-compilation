#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <Wire.h>
#include <ThingsBoard.h>
#include <Arduino_MQTT_Client.h>
//PENTING, KE LIBRARY KETIK TBPUBSUBCLIENT, PILIH VERSI 2.9.1

RF24 radio(4, 5); // CE, CSN
const byte address[6] = "00001";
int SUM_DATA = 0;
int RSSI = 0;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1     // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ThingsBoard Settings
#define CURRENT_FIRMWARE_TITLE    "TEST"
#define CURRENT_FIRMWARE_VERSION  "1.0.0"
#define TOKEN               "QAnFPbg4XhyxwYUryI0c" //Masukkan Token TB 
#define THINGSBOARD_SERVER  "thingsboard.cloud"
const char* WIFI_SSID = "airi"; //Masukkan SSID WiFI
const char* WIFI_PASSWORD = "12345678"; //Masukkan Password WiFi
constexpr uint16_t MAX_MESSAGE_SIZE = 128U;


WiFiClient espClient;
//ThingsBoard tb(espClient);
Arduino_MQTT_Client mqttClient(espClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);
int status = WL_IDLE_STATUS;
;

void InitWiFi()
{
  Serial.println("Menghubungkan ke WiFi");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Menghubungkan ke WiFi");
  display.display();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    display.print(".");
    display.display();
  }
  Serial.println("Terhubung ke WiFi");
  display.setCursor(0, 20);
  display.print("[TERHUBUNG]");
  display.display();
  delay(1000);
}

void reconnect()
{
  status = WiFi.status();
  if ( status != WL_CONNECTED)
  {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Terhubung ke WiFi");
  }
}
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
    InitWiFi();
}

void loop()
{
    if (radio.available())
    {
        tb.connect(THINGSBOARD_SERVER, TOKEN);
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

        // Upload data to ThingsBoard
        tb.sendTelemetryData("humidity", humidity);
        tb.sendTelemetryData("temperature", temperature);
        tb.sendTelemetryData("ultrasonic_distance", ultrasonicDistance);
    }
}
