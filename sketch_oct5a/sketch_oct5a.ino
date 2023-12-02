#include <RF24Network.h> 
#include <RF24.h> 
#include <SPI.h> 
#include <Wire.h>  
#include <LiquidCrystal_I2C.h> 
 
#define CE 9 
#define CS 10 
 
#define ledPin 2      // the number of the LED pin 
 
// nRF24L01(+) radio attached using Getting Started board  
RF24 radio(CE,CS); //CE,CS
// Network uses that radio 
RF24Network network(radio); 
 
// Address of our node 
const uint16_t this_node = 0; 
 
// Address of the other node 
const uint16_t other_node = 1; 
 
// Structure of our payload 
struct payload_t 
{ 
  unsigned long sensor; 
  unsigned long alamat; 
}; 
 
int buf_alamat; 
int buf_sensor; 
 
// Set the LCD address to 0x27 for a 20 chars and 4 line display 
LiquidCrystal_I2C lcd(0x3F, 20, 4); //27 atau 3F 
 
void setup(void) 
{ 
  Serial.begin(57600); 
  Serial.println("RF24Network/examples/helloworld_rx/"); 
 
  SPI.begin(); 
  radio.begin(); 
  network.begin(/*channel*/ 90, /*node address*/ this_node); 
 
  // initialize digital pin LED_BUILTIN as an output. 
  pinMode(ledPin, OUTPUT); 
  digitalWrite(ledPin, LOW); 
 
  // initialize the LCD 
  lcd.begin(20, 4); 
 
  // Turn on the blacklight and print a message.
    // Turn on the blacklight and print a message. 
  lcd.backlight(); 
  lcd.setCursor(0, 0); 
  lcd.print("WSN"); 
  lcd.setCursor(0, 1); 
  lcd.print("Wireless"); 
  lcd.setCursor(0, 2); 
  lcd.print("Sensor"); 
  lcd.setCursor(0, 3); 
  lcd.print("Network"); 
  delay(1500); 
  lcd.clear(); 
} 

void loop(void) 
{ 
  // Pump the network regularly 
  network.update(); 
 
  // Is there anything ready for us? 
  while ( network.available() ) 
  { 
    // If so, grab it and print it out 
    RF24NetworkHeader header; 
    payload_t payload; 
    network.read(header,&payload,sizeof(payload)); 
    buf_alamat = payload.alamat; 
    buf_sensor = payload.sensor; 
    Serial.print("Terima data dari: "); 
    Serial.print(buf_alamat); 
    Serial.print(" at "); 
    Serial.println(buf_sensor); 
    digitalWrite(ledPin, HIGH);   // turn the LED on 
    delay(100);                     
    digitalWrite(ledPin, LOW);    // turn the LED off 
  } 
   
  lcd.clear(); 
  lcd.setCursor(2, 0); 
  lcd.print("A"); 
  lcd.setCursor(0, 1); 
  lcd.print(buf_alamat); 
  lcd.setCursor(0, 2); 
  lcd.print("Pot="); 
  lcd.setCursor(4, 2); 
  lcd.print(buf_sensor); 
  delay(500); 
} 