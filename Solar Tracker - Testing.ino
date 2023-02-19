#include <Servo.h>
#include <Encoder.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>
#include <DS1307RTC.h>
#include <Time.h>

//Encoder
#define ENYPIN1 3
#define ENYPIN2 5
#define ENXPIN1 2
#define ENXPIN2 4
Encoder ENX(ENXPIN1, ENXPIN2);
Encoder ENY(ENYPIN1, ENYPIN2);
long ENXPre, ENYPre, ENXNow, ENYNow;
//Increment = 4, 360 degrees = 80, 360/20 = 18 degrees / step.
//End of Encoder

//EndLimitSwitch
#define ELS 6
#define EndSwitch digitalRead(ELS)
//End of EndLimitSwitch

//Servo
#define SVX 8
#define SVY 7
#define SVXMID 92
#define SVYMID 90
Servo xServo, yServo;
//End of Servo

//SolarTracking Calculation
#define PRESETELEVATION 21
#define DTOR (3.14/180)
#define RTOD (180/3.14)
#define deltaGMT -8
#define AZIOFFSET 90
#define Latitude 39
#define Longitude -121.84
int Year, Month, Day, Hour, Minute, Second, LSTM, SrH, SrM, SsH, SsM;
double EoT, B, TCF, LST, LHA, Declination, Elevation, Azimuth, Azi, Sunrise, Sunset;
double PreEle, PreAzi;
//End of SolarTracking Calculation

//Other
#define COMPADDR 0x21
#define STEP 0.22
#define FUNCTIONKEY 9
#define FUNCTION digitalRead(FUNCTIONKEY)
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE); //LCD I2C ADDR: 0x27
byte compData[2];
String compassString = "";
String solarpos = "";
int COMP1, COMP2;
int raw_x, raw_y;
float adjustedX, adjustedY;
//End of Other.

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  lcd.begin(16,2);
  lcd.setBacklight(HIGH);
  pinMode(ELS, INPUT); //End Limit Switch.
  pinMode(FUNCTIONKEY, INPUT); //Function Key.
  xServo.attach(SVX); //Attach ServoX
  yServo.attach(SVY); //Attach ServoY
  //Encoder Initialization.  
  pinMode(ENXPIN1, INPUT); //Encoder X Pin1
  pinMode(ENXPIN2, INPUT); //Encoder X Pin2
  pinMode(ENYPIN1, INPUT); //Encoder Y Pin1
  pinMode(ENYPIN2, INPUT); //Encoder Y Pin2
  //Variables Initialization.
  PreEle = PRESETELEVATION;
  PreAzi = AZIOFFSET;
  ENXPre = ENYPre = ENXNow = ENYNow = 0;
  
  getTime();
  SunriseSunset(Year, Month, Day);
  sunPos(Year, Month, Day, Hour, Minute, Second);
}

void loop()
{
  String temp = "", temp2 = "";
  while (FUNCTION) { manualMode(); delay(200); }
  initialReset();
  lcd.clear(); lcd.setCursor(0,0); lcd.print("Reset Done."); lcd.setCursor(0,1); lcd.print("Press Key.");
  while (FUNCTION); delay(500);
  while (true)
  {
    getTime();
    SunriseSunset(Year, Month, Day);
    delay(1000);
    if ( ((Hour*100+Minute)>=(SrH*100+SrM)) && ((Hour*100+Minute)<(SsH*100+SsM)) ) break;
   } 
  while (true)
  {
    if ( ((Hour*100+Minute)>=(SrH*100+SrM)) && ((Hour*100+Minute)<(SsH*100+SsM)) )
    {
      temp.concat(Elevation); temp.concat(" "); temp.concat(Azimuth);
      temp2.concat(Hour); temp2.concat(":"); temp2.concat(Minute);
      lcd.clear(); lcd.setCursor(0,1); lcd.print(temp); temp="";
      lcd.setCursor(0,0); lcd.print(temp2); temp2="";
      
      sunPos(Year, Month, Day, Hour, Minute, Second);
      if ( Elevation > PRESETELEVATION && (abs(Elevation - PreEle))/STEP > 4 )
      {
        ENYNow = ENY.read();
        while ( abs(ENYNow - ENYPre)/4 < (abs(Elevation - PreEle))/STEP )
        { 
          if ( Elevation > PreEle ) yServo.write(180);
          if ( Elevation < PreEle ) yServo.write(0);
          if ( Elevation < PreEle && (Elevation - PRESETELEVATION)/STEP < 4 ) while ( EndSwitch == 1) yServo.write(0);
          ENYNow = ENY.read();
        }
        yServo.write(SVYMID);
        PreEle = Elevation;
        ENYPre = ENYNow;
      }
      delay(500);
      if ( abs(Azimuth-PreAzi)/STEP > 4 )
      {
        ENXNow = ENX.read();
        while ( abs(ENXNow - ENXPre)/4 < (abs(Azimuth - PreAzi))/STEP )
        {
          if ( Azimuth < PreAzi ) xServo.write(180);
          if ( Azimuth > PreAzi ) xServo.write(0);
          ENXNow = ENX.read();
        }
        xServo.write(SVXMID);
        ENXPre = ENXNow;
        delay(500);
        PreAzi = Azimuth;
      }
    }
    getTime();
    if ( (Hour*100+Minute)>(SsH*100+SsM) ) break;
  }
  
  lcd.clear(); lcd.setCursor(0,0); lcd.print("Finished!"); lcd.setCursor(0,1); lcd.print("Now Reset."); delay(2000);
  resetPosition();
}

void initialReset()
{
  String resetString = "";
  
  while (true)
  {
    resetString = ""; resetString.concat("EndSwitch:"); resetString.concat(EndSwitch);
    lcd.clear(); lcd.setCursor(0,0); lcd.print("Reset ELE"); lcd.setCursor(0,1); lcd.print(resetString);
    if ( EndSwitch == 1 ) yServo.write(0);
    else if ( EndSwitch == 0 ) { yServo.write(SVYMID); break; } 
    delay(200);
  }
  
  lcd.clear(); lcd.setCursor(0,1); lcd.print("Rst ELE Done!"); delay(2000);
  
  while (true)
  {
    getCompassValue();
    if ( COMP1 > 2 && COMP1 <=180 ) xServo.write(180);
    else if ( COMP1 > 180 && COMP1 < 358 ) xServo.write(0);
    else { xServo.write(SVXMID); break; }
    lcd.clear(); lcd.setCursor(0,0); lcd.print("Reset AZI"); lcd.setCursor(0,1); lcd.print(compassString);
    delay(200);
  }
  
}

void resetPosition()
{
  while (true)
  {
    getCompassValue();
    xServo.write(180);
    lcd.clear(); lcd.setCursor(0,0); lcd.print("Reset AZI"); lcd.setCursor(0,1); lcd.print(compassString);
    delay(200);
    if ( COMP1 <4  ) { xServo.write(SVXMID); break; }
  }
}

int getDayOfYear(int year, int month, int day)
{
  int feb;
  double otherMonth;
  otherMonth = 30.6;
  if ( (year%100 == 0 && year%400 == 0) || year%4 == 0 ) feb = 29;
  else feb = 28;

  if ( month == 1) return day;
  else if ( month == 2) return (31+day);
  else return (round((month-3)*otherMonth + 31 + feb + day));  
}

double timeConvert(int hour, int minute, int second)
{
  return hour + minute/60.0 + second/3600.0;
}

void sunPos(int year, int month, int day, int hour, int minute, int second)
{
  LST = timeConvert(hour, minute, second) + TCF/60.0;
  LHA = 15.0*(LST-12.0);
  Elevation = asin( sin(Declination*DTOR)*sin(Latitude*DTOR) + cos(Declination*DTOR)*cos(Latitude*DTOR)*cos(LHA*DTOR) ) * RTOD;
  Azi = acos( (sin(Declination*DTOR)*cos(Latitude*DTOR) - cos(Declination*DTOR)*sin(Latitude*DTOR)*cos(LHA*DTOR))/cos(Elevation*DTOR) ) * RTOD;
  if (LST > 12) Azimuth = 360 - Azi; else Azimuth = Azi;
}

void SunriseSunset(int year, int month, int day)
{
  LSTM = (double)15 * deltaGMT;
  B = 360.0/365.0 * (getDayOfYear(year, month, day) - 81);
  EoT = 9.87*sin(2*B*DTOR) - 7.53*cos(B*DTOR) - 1.5*sin(B*DTOR);
  TCF = 4*(Longitude - LSTM) + EoT;
  Declination = 23.45*sin(B*DTOR);
  Sunrise = 12 - acos(-tan(Latitude*DTOR)*tan(Declination*DTOR)) * RTOD / 15.0 - TCF/60.0;
  Sunset = 12 + acos(-tan(Latitude*DTOR)*tan(Declination*DTOR)) * RTOD / 15.0 - TCF/60.0;
  SrH = (int)Sunrise;
  SrM = (int)round((Sunrise-SrH) * 60);
  SsH = (int)Sunset;
  SsM = (int)round((Sunset-SsH) * 60);
}

void getCompassValue()
{
  int i, compValue;
  String s = "";
  
  Wire.beginTransmission(COMPADDR);
  Wire.write("A");
  Wire.endTransmission();
  delay(10);
  
  Wire.requestFrom(COMPADDR, 2, true);
  i=0;
  while(Wire.available() && i<2)
  {
    compData[i] = Wire.read();
    i++;
  }
  
  compValue = compData[0]*256 + compData[1];
  COMP1 = compValue/10;
  COMP2 = compValue%10;
  
  compassString = "";
  compassString.concat("CP:");
  compassString.concat(COMP1);
  compassString.concat('.');
  compassString.concat(COMP2);
  compassString.concat(" ES:");
  compassString.concat(EndSwitch);
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

void manualMode()
{
  raw_x = analogRead(A0); raw_y = analogRead(A1);
  adjustedX = raw_x * 5.0 / 1023; adjustedY = raw_y * 5.0 / 1023;
  
  if ( (adjustedX>=2.2)&&(adjustedX<=2.8) ) yServo.write(SVYMID);
  else if ((adjustedX<2.2)|| (adjustedX>2.8)) yServo.write(map(adjustedX,0,5,0,179));
  
  if ((adjustedY>=2.2)&&(adjustedY<=2.8)) xServo.write(SVXMID);
  else if ((adjustedY<2.2)|| (adjustedY>2.8)) xServo.write(map(adjustedY,0,5,0,179));
  
  getCompassValue();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Manual Mode");
  lcd.setCursor(0,1);
  lcd.print(compassString);
}
