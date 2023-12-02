#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include "thingProperties.h"

#define SS_PIN    5   // SDA pin for MFRC522
#define RST_PIN   4   // Reset pin for MFRC522
#define SERVO_PIN 25  // Servo motor control pin
#define MOTOR_PIN1 33 // Motor control pin 1
#define MOTOR_PIN2 32 // Motor control pin 2
#define MAG_SWITCH 14 // GPIO pin for magnetic switch
#define SCREEN_RESET 16 // Reset pin for OLED display
double frequency = 200;
uint8_t resolution_bits=10;

// Define the correct RFID card NUID
byte correctNUID[] = {0x92, 0xA6, 0xB8, 0x89};

MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo myservo;
ESP32PWM PWMSET;

// OLED display setup
Adafruit_SSD1306 display(128, 64, &Wire, SCREEN_RESET);

bool doorLocked = true; // Variable to track the door lock state

void setup() {
  Serial.begin(115200);
  SPI.begin();
  delay(1500);
  initProperties();
  mfrc522.PCD_Init();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  pinMode(SERVO_PIN, OUTPUT);
  pinMode(MOTOR_PIN1, OUTPUT);
  pinMode(MOTOR_PIN2, OUTPUT);
  pinMode(MAG_SWITCH, INPUT_PULLUP);

  PWMSET.attachPin(SERVO_PIN, frequency);

  myservo.attach(SERVO_PIN);
  
  // OLED display initialization
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();

  // Print initial status
  updateDisplay();
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();
  // Continuously check if the door is open or closed
  if (digitalRead(MAG_SWITCH) == HIGH) {
    if (!doorLocked) {
      unlockDoor(); // Unlock the door if it's not locked
    }
    displayStatus("Door Opened");
    myservo.write(90); // Unlock position
    delay(1000);
  } else {
    displayStatus("Door Locked");
    myservo.write(0);
    // Check RFID card only if the door is closed
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      if (compareNUID(mfrc522.uid.uidByte, correctNUID, mfrc522.uid.size)) {
        displayStatus("Card Verified");
        unlockDoor();
        delay(5000); // Keep the door unlocked for 5 seconds
        lockDoor();
      } else {
        displayStatus("Access Denied");
        delay(2000);
      }
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
    }
  }
}

bool compareNUID(byte *nuid1, byte *nuid2, byte size) {
  for (byte i = 0; i < size; i++) {
    if (nuid1[i] != nuid2[i]) {
      return false;
    }
  }
  return true;
}

void unlockDoor() {
  doorLocked = false;
  updateDisplay();
  displayStatus("Door Unlocked");
  myservo.write(90); // Unlock position
  delay(1000);
  digitalWrite(MOTOR_PIN1, HIGH); // Motor rotates to open the door
  digitalWrite(MOTOR_PIN2, LOW);
  delay(3000); // Adjust this delay according to the door opening time
  digitalWrite(MOTOR_PIN1, LOW);

  // Send "door unlocked" to IoT Cloud
  doorlog = "Door Unlocked";
  ArduinoCloud.writePropertyValue(doorlog);
}

void lockDoor() {
  if (!doorLocked) {
    doorLocked = true;
    updateDisplay();
    displayStatus("Door Locked");
    myservo.write(0); // Lock position
    delay(1000);
    digitalWrite(MOTOR_PIN1, LOW); // Motor stops
    digitalWrite(MOTOR_PIN2, LOW);

    // Send "door locked" to IoT Cloud
    doorlog = "Door Locked";
    ArduinoCloud.writePropertyValue(doorlog);
  }
}

void onDoorlockedChange() {
  // Add your code here to act upon Doorlocked change
  if (doorlocked) {
    lockDoor();
  } else {
    unlockDoor();
  }
}

void displayStatus(const char *status) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(status);
  display.display();
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  if (doorLocked) {
    display.println("Door Locked");
    display.setCursor(0, 20);
    display.println("Scan your RFID");
  } else {
    display.println("Door Unlocked");
    display.setCursor(0, 20);
    display.println("Door is open");
  }
  display.display();
}
