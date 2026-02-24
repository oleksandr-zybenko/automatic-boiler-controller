#include <OneWire.h>
#include <DallasTemperature.h>

#define Relay1 4  // OPEN
#define Relay2 5  // CLOSE

OneWire oneWire(7);
DallasTemperature ds(&oneWire);

DeviceAddress sensor1 = {0x28, 0x6A, 0xD0, 0x71, 0x33, 0x20, 0x01, 0xCE};
DeviceAddress sensor2 = {0x28, 0x38, 0xC9, 0x90, 0x32, 0x20, 0x01, 0x6C};

int8_t BasicTemp = 60;

int voda;
int cotel;

unsigned long tempTimer = 0;
unsigned long valveTimer = 0;

bool flag = 0;        // 0 = closed, 1 = opened
bool a = 0;           // desired direction
bool valveActive = 0; // movement in progress

const unsigned long TEMP_INTERVAL = 1000;
const unsigned long VALVE_TIME = 25000;

void setup()
{
  Serial.begin(9600);

  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);

  digitalWrite(Relay1, HIGH);
  digitalWrite(Relay2, HIGH);

  ds.begin();
}

void loop()
{
  Temp();
  Function();
  RelayControl();

  Serial.print("Water: ");
  Serial.println(voda);
  Serial.print("Boiler: ");
  Serial.println(cotel);
}

void Temp()
{
  if (millis() - tempTimer >= TEMP_INTERVAL)
  {
    tempTimer = millis();
    ds.requestTemperatures();
    voda = ds.getTempC(sensor1);
    cotel = ds.getTempC(sensor2);
  }
}

void Function()
{
  if (cotel > voda)
    a = HIGH;
  else
    a = LOW;

  if (voda > BasicTemp)
    a = LOW;
}

void RelayControl()
{
  // START OPEN
  if (a == HIGH && flag == 0 && !valveActive)
  {
    digitalWrite(Relay1, LOW);
    valveTimer = millis();
    valveActive = true;
  }

  // START CLOSE
  if (a == LOW && flag == 1 && !valveActive)
  {
    digitalWrite(Relay2, LOW);
    valveTimer = millis();
    valveActive = true;
  }

  // STOP MOVEMENT
  if (valveActive && millis() - valveTimer >= VALVE_TIME)
  {
    digitalWrite(Relay1, HIGH);
    digitalWrite(Relay2, HIGH);

    flag = a;          // update state
    valveActive = false;
  }
}
