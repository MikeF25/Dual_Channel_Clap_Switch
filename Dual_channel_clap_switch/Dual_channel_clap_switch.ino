/* Dual channel clap switch With LCD*/
// a clap switch that can independently control each channel by controlled clapping

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SOUND  LOW     // active low sound sensor, you know what to do
#define DETECT 2500    // milliseconds of silence to tolerate
#define REGISTER 1500  // miliseconds for regitering clapValue to void loop
#define debounceDelay = 30;  // for filtering the time takes between clap

const unsigned char rawSoundIn = A0;
const unsigned char indicator = 5;
const unsigned char LED1 = 6;
const unsigned char LED2 = 7;

unsigned char clapValue;
unsigned long Time; // updated at the top of loop()
unsigned long lastTimeClap = 0; // for calculating time from the last clap
bool reseting;

void setup()
{
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Clap detected :");
  lcd.setCursor(8,1);
  lcd.print("0");

  pinMode(rawSoundIn, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(indicator, OUTPUT);
}

void loop()
{
  Time = millis();   // standard time

  unsigned char clap = clapCheck();
  static bool LED1_stat;
  static bool LED2_stat;
          // Serial.println(clap);

  if (clap == 2) {  // clap 2x to turn on or off LED 1
    if (!LED1_stat) {
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.println("LED 1 ON");

      Serial.println("Turning ON LED 1 \n");
      digitalWrite(LED1, 1);
      LED1_stat = true;
      reseting = true;
    } else {
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("LED 1 OFF");

      Serial.println("Turning OFF LED 1 \n");
      digitalWrite(LED1, 0);
      LED1_stat = false;
      reseting = true;
    }
    clapValue = 0;
  }

  if (clap == 3) {  // clap 3x to turn on or off LED 2
    if (!LED2_stat) {
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("LED 2 ON");

      Serial.println("Turning ON LED 2 \n");
      digitalWrite(LED2, 1);
      LED2_stat = true;
      reseting = true;
    } else {
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("LED 2 OFF");

      Serial.println("Turning OFF LED 2 \n");
      digitalWrite(LED2, 0);
      LED2_stat = false;
      reseting = true;
    }
    clapValue = 0;
  }
}


unsigned char clapCheck()
{
  static unsigned long soundTime;
  unsigned char soundNow = digitalRead(rawSoundIn) == SOUND;

  digitalWrite(indicator, soundNow);

  if (soundNow && !reseting) {
    if (Time - lastTimeClap >= debounceDelay) {
      clapValue++;
      soundTime = Time;
      lcd.setCursor(8,1);
      lcd.print(clapValue);
    }
    lastTimeClap = Time;
  }

  if (soundTime && (Time - soundTime > DETECT)) {
    soundTime = 0;
    clapValue = 0;
    Serial.println("reseting clap value to 0");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Clap detected :");
    lcd.setCursor(8,1);
    lcd.print("0");
    reseting = false;
  }
  else if (soundTime && (Time - soundTime == REGISTER)) { // buffer
    Serial.println("delaying data transfer");
    Serial.print("clapValue = ");
    Serial.println(clapValue);
    Serial.println("");
    return clapValue;
  }
  return 0;
}
