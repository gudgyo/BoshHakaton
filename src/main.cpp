#include <Arduino.h>

#define potPin 36
#define dataPin 13
#define clockPin 14
#define latchPin 15
#define buttonPin 33

#define printTime 100
#define displayTime 50
#define buttonTime 500

int potValue = 0;
int count = 0;
unsigned long lastReporttime = 0;
unsigned long lastDisplaytime = 0;
unsigned long lastButtontime = 0;

char mode = 65;
int moddedValue;

byte valueTobyte(int);
void ledDisplay(byte, byte);
int moddedValuecalc(int, int);

void setup()
{

  Serial.begin(115200);
  pinMode(potPin, INPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(latchPin, LOW);
}

void loop()
{

  if (millis() - lastReporttime > printTime)
  {
    lastReporttime = millis();
    potValue = analogRead(potPin);
    moddedValue = moddedValuecalc(potValue, mode);
    Serial.printf("%d, %d, %d, %c,\n", potValue, moddedValue, digitalRead(buttonPin), mode);
  }

  if (millis() - lastDisplaytime > displayTime)
  {
    lastDisplaytime = millis();
    potValue = analogRead(potPin);
    ledDisplay(valueTobyte(potValue), valueTobyte(moddedValue));
  }

  if (millis() - lastButtontime > buttonTime)
  {
    if (digitalRead(buttonPin) == 0)
    {
      lastButtontime = millis();
      mode += 1;
      if (mode > 67)
        mode = 65;
    }
  }
}

byte valueTobyte(int x)
{
  byte y = 0b11111111;
  int shift;
  shift = map(x, 0, 4095, 8, 0);
  y >>= shift;
  return y;
}

void ledDisplay(byte red, byte green)
{
  shiftOut(dataPin, clockPin, MSBFIRST, green);
  shiftOut(dataPin, clockPin, MSBFIRST, red);
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
}

int moddedValuecalc(int potVal, int mode)
{
  switch (mode)
  {
  case 65:
    return potVal;

  case 66:
    if (potVal < 0.2 * 4095)
      return map(potVal, 0, 0.2 * 4095, 0, 0.45 * 4095);
    else if (potVal < 0.8 * 4095)
      return map(potVal, 0.2 * 4095, 0.8 * 4095, 0.45 * 4095, 0.55 * 4095);
    else
      return map(potVal, 0.8 * 4095, 4095, 0.55 * 4095, 4095);

  case 67:
    if (potVal < 500)
      return 0;
    potVal = potVal + random(-1000, 1000);
    if (potVal < 0)
      return 0;
    if (potVal > 4095)
      return 4095;
    return potVal;
  default:
    return 0;
    break;
  }
  return 0;
}