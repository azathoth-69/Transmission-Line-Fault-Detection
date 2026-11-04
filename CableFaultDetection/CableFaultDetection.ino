#include <LiquidCrystal.h>
// LCD: RS, E, D4, D5, D6, D7. R/W is grounded.
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
const byte phasePins[] = {8, 9, 10};
const char *phaseNames[] = {"Blue", "Yellow", "Red"};
const byte buzzerPin = 13;
const byte sensePin = A0;
// A 1k pull-up and a 100-ohm outbound + 100-ohm return per section.
// The expected 10-bit ADC readings are approximately 171, 292, 384, 455.
const int expected[] = {171, 292, 384, 455};
const int tolerance = 30;
int readFault() {
  // Average several readings after the relay and ADC have settled.
  analogRead(sensePin);
  long total = 0;
  for (byte i = 0; i < 8; ++i) {
    total += analogRead(sensePin);
    delay(2);
  }
  int reading = total / 8;
  int best = -1;
  int error = tolerance + 1;
  for (byte i = 0; i < 4; ++i) {
    int difference = abs(reading - expected[i]);
    if (difference < error) {
      error = difference;
      best = i;
    }
  }
  // -1 means no recognized fault, not necessarily a healthy cable.
  return best;
}
void setup() {
  for (byte i = 0; i < 3; ++i) {
    pinMode(phasePins[i], OUTPUT);
    digitalWrite(phasePins[i], LOW);
  }
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
  lcd.begin(16, 2);
  lcd.print("Cable fault test");
  delay(1000);
}
void loop() {
  for (byte phase = 0; phase < 3; ++phase) {
    digitalWrite(phasePins[phase], HIGH);
    delay(100); // allow the relay to operate
    int fault = readFault();
    digitalWrite(buzzerPin, fault >= 0 ? HIGH : LOW);
    lcd.clear();
    lcd.print(phaseNames[phase]);
    lcd.print(" line");
    lcd.setCursor(0, 1);
    if (fault >= 0) {
      lcd.print("Fault: ");
      lcd.print(fault + 1);
      lcd.print(" km");
    } else {
      lcd.print("No recognized");
    }
    delay(1200);
    digitalWrite(phasePins[phase], LOW);
    digitalWrite(buzzerPin, LOW);
    delay(100); // break before selecting the next phase
  }
}