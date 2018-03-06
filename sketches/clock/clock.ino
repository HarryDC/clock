// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "RTClib.h"
#include "Adafruit_LEDBackpack.h"

#include "stdlib.h"

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
#define Serial SerialUSB
#endif

RTC_PCF8523 rtc;
Adafruit_7segment matrix = Adafruit_7segment();

const uint8_t buttonPin = 4;
int buttonState = 0;
uint16_t pushCount = 0;

const char* ataturk = "    You the mothers who sent their sons from faraway countries wipe away your tears your sons are now lying in our bosom and are in peace  After having lost their lives on this land they have become our sons as well";
const char* happyBirthday = "   Happy Birthday    ";
const char* happyNewYear = "    Happy New Year    ";
const char* merryChristmas = "    Merry Chrismas    ";

void writeLetter(Adafruit_7segment* display, char letter, int pos)
{
  if (pos > 4) return;

  uint8_t raw_letter;

  switch (letter)
  {
    case 'a': case 'A': raw_letter = 0b1110111; break;
    case 'b': case 'B': raw_letter = 0b1111100; break;
    case 'c': case 'C': raw_letter = 0b0111001; break;
    case 'd': case 'D': raw_letter = 0b1011110; break;
    case 'e': case 'E': raw_letter = 0b1111001; break;
    case 'f': case 'F': raw_letter = 0b1110001; break;
    case 'g': case 'G': raw_letter = 0b1101111; break;
    case 'h': case 'H': raw_letter = 0b1110100; break;
    case 'i': case 'I': raw_letter = 0b0110000; break;
    case 'j': case 'J': raw_letter = 0b0011110; break;
    case 'k': case 'K': raw_letter = 0b0000000; break;
    case 'l': case 'L': raw_letter = 0b0111000; break;
    case 'm': case 'M': raw_letter = 0b0010101; break;
    case 'n': case 'N': raw_letter = 0b1010100; break;
    case 'o': case 'O': raw_letter = 0b1011100; break;
    case 'p': case 'P': raw_letter = 0b1110011; break;
    case 'q': case 'Q': raw_letter = 0b1100111; break;
    case 'r': case 'R': raw_letter = 0b1010000; break;
    case 's': case 'S': raw_letter = 0b1101101; break;
    case 't': case 'T': raw_letter = 0b1111000; break;
    case 'u': case 'U': raw_letter = 0b0111110; break;
    case 'v': case 'V': raw_letter = 0b0011100; break;
    case 'w': case 'W': raw_letter = 0b0101010; break;
    case 'x': case 'X': raw_letter = 0b1110110; break;
    case 'y': case 'Y': raw_letter = 0b1101110; break;
    case 'z': case 'Z': raw_letter = 0b1011011; break;
    case '0': raw_letter = 0b0111111; break; case '1': raw_letter = 0b0000110; break;
    case '2': raw_letter = 0b1011011; break; case '3': raw_letter = 0b1001111; break;
    case '4': raw_letter = 0b1100110; break; case '5': raw_letter = 0b1101101; break;
    case '6': raw_letter = 0b1111101; break; case '7': raw_letter = 0b0000111; break;
    case '8': raw_letter = 0b1111111; break; case '9': raw_letter = 0b1101111; break;
    case '.': raw_letter = 0xff; break; case ':': raw_letter = 0xff; break;
    case ' ': case '\0': default: raw_letter = 0b0000000;
  
  }

  // Write to buffer
  display->writeDigitRaw(pos, raw_letter);
}

// Writes a string to the buffer
void writeString(Adafruit_7segment* display, char str[5])
{
  for (int i = 0; i < 6; i++)
  {
    if (i == 2)
    {
      writeLetter(display, '.', i);
    }
    else
    {
      writeLetter(display, (i > 2) ? str[i - 1] : str[i], i);
    }
  }
}

void setup () {

  //#ifndef ESP8266
  //  while (!Serial); // for Leonardo/Micro/Zero
  //#endif

  Serial.begin(57600);
  Serial.println("Starting!");
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  Serial.println("RTC Begun");
  matrix.begin(0x70);
  Serial.println("Matrix Begun");

  if (! rtc.initialized()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  else {
    Serial.println("RTC is Running");
  }
 
  pinMode(buttonPin, INPUT);
}

void showDate(const DateTime& now) {
  uint16_t num = now.month() * 100 + now.day();
  matrix.println(num);
  matrix.drawColon(false);
  matrix.writeDisplay();
}

void showYear(const DateTime& now) {
  matrix.println(now.year());
  matrix.drawColon(false);
  matrix.writeDisplay();
}

void showDay(const DateTime& now) {
  const char days[7][5] = {" sun  ", " mon  ", " tue  ", " wed  ", " thu  ", " fri  ", " sat  "};
  writeString(&matrix, days[now.dayOfTheWeek()]);
  matrix.drawColon(false);
  matrix.writeDisplay();
}

void showTime(const DateTime& now) {
  int num = now.hour() % 12;
  num = (num == 0) ? 12 : num;
  num = (num * 100) + now.minute();

  matrix.println(num);
  matrix.drawColon(now.second() % 2 == 0);
  matrix.writeDisplay();
}

void printDate(DateTime& now) {
  showDay(now);
  delay(2000);
  showDate(now);
  delay(2000);
  showYear(now);
  delay(2000);
  showTime(now);
  delay(2000);
}

void loopString(const char* string) {
  int len = strlen(string);

  int index = 0;
  for(int i = 0; i<len-4;++i) {
    writeLetter(&matrix, string[i], 0);
    writeLetter(&matrix, string[i+1], 1);
    writeLetter(&matrix, string[i+2], 3);
    writeLetter(&matrix, string[i+3], 4);
    matrix.writeDisplay();
    delay(500);  
  }
}

bool handleButton(DateTime& now) {
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH)
  {
    ++pushCount;
    int minute = now.minute();
    if (pushCount < 10)
    {
      ++minute;
    }
    else
    {
      minute = ((minute / 10) + 1) * 10;
    }
    int hour = now.hour();
    if (minute == 60)
    {
      minute = 0;
      hour++;
    }
    if (hour == 24) hour = 0;

    rtc.adjust(DateTime(now.year(), now.month(), now.day(), hour, minute, now.second()));
    showTime(now);    
    return true;
  }
  else
  {
    pushCount = 0;
    return false;
  }
}

void loop () {
  DateTime now = rtc.now();

  if (!handleButton(now))
  {
    if (now.minute() < 2 && now.second() < 2) {
      if (now.month() == 12 && (now.day() == 24 || now.day() == 25)) {
        loopString(merryChristmas);
      }
      else if (now.month() == 1 && (now.day() == 1)) {
        loopString(happyNewYear);
      }
      else if (now.month() == 4 && (now.day() == 25)) {
        if (now.hour() != 12) {
          loopString(happyBirthday);      
        } else {
          loopString(ataturk);
        }
      }
      else {
        printDate(now);
      }
    }
  }
  
  showTime(now);

  delay(200);
}
