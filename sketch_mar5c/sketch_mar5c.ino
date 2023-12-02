#define HOT 6
#define NORMAL 4
#define COLD 2
#define sensorPin A1

float voltage = 0;
float celsius = 0;
float hotTemp = 26;
float coldTemp = 20;
float sensor = 0;

void setup()
{
  pinMode(HOT, OUTPUT);
  pinMode(NORMAL, OUTPUT);
  pinMode(COLD, OUTPUT);
  Serial.begin(9600);
}


void loop()
{
  sensor = analogRead(sensorPin);
  voltage = sensor*0.004882; 
  celsius = voltage*100; 
  Serial.println(celsius);

  if ( celsius < coldTemp )
  {
    digitalWrite(COLD, HIGH);
    delay(100);
    digitalWrite(COLD, LOW);
  }
  else if ( celsius > coldTemp && celsius <= hotTemp )
  {
    digitalWrite(NORMAL, HIGH);
    delay(100);
    digitalWrite(NORMAL, LOW);
  }
  else
  {
    digitalWrite(HOT, HIGH);
    delay(100);
    digitalWrite(HOT, LOW);
  }
}