/* Test DS1307 real-time clock. */

#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>

int Year, Month, Day, Hour, Minute, Second;

void setup() {
  Serial.begin(9600);
}

void loop() {
  getTime();
  delay(1000);
}

void getTime()
{
  tmElements_t tm;

  if (RTC.read(tm)) {
    Year = tmYearToCalendar(tm.Year);
    Month = tm.Month;
    Day = tm.Day;
    Hour = tm.Hour;
    Minute = tm.Minute;
    Second = tm.Second;
  }
  Serial.print(Year);
  Serial.print(" ");
  Serial.print(Month);
  Serial.print(" ");
  Serial.print(Day);
  Serial.print(" ");
  Serial.print(Hour);
  Serial.print(" ");
  Serial.print(Minute);
  Serial.print(" ");
  Serial.println(Second);
}
