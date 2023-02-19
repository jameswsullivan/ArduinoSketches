#include <Wire.h>
#include <SoftwareSerial.h>

//SolarTracking Calculation
#define deltaGMT -7
#define DTOR (3.14/180)
#define RTOD (180/3.14)
int LSTM, SrH, SrM, SsH, SsM;
double EoT, B, TCF, LST, LHA, Declination, Elevation, Azimuth, Ele, Azi, Sunrise, Sunset;
//End of SolarTracking Calculation

//GPS
#define MARKER "$GPRMC"
String GPS = "";
char inByte = '\0';
char NS, EW; //GPS Latitude Longitude Direction.
int Year, Month, Day, Hour, Minute, Second; //GPS Date and Time.
double Latitude, Longitude; //GPS Coordinates.
//End of GPS

//Data Verification
int checksum = 0;
#define CHECKSUM 
//End of Data Verification.

//Software Serial:
SoftwareSerial SerialGPS(13, 12); //Rx, Tx
//End of Software Serial

//Other
int preTime, curTime, SRT, SST;
#define GPSADDR 0x28
//End of Other.

void setup() 
{
  Serial.begin(9600);
  SerialGPS.begin(9600);
  Wire.begin(GPSADDR);
  initialization();
}

void loop() 
{ 
  Latitude = 39; Longitude = -121;
  Year = 2014; Month = 10; Day = 20;
  Hour = 7; Minute = 30; Second = 0;
  SrH = 7; SrM = 25; SsH = 18; SsM = 13;
  
  while (Hour <= 18)
  {
    Serial.print("Time: "); Serial.print(Hour);
    Serial.print(":"); Serial.println(Minute);
    calculateMove(Year, Month, Day, Hour, Minute, Second);
    //sendData();
    if (Hour == 18 && Minute > 10) break;
    else if (Minute == 55) { Hour++; Minute =0; }
    else Minute += 5; 
  }    
  GPS = "";
  inByte = -1;
  checksum = 0;  
  if (Hour == 18 && Minute > 10) while(1); else delay(2000);
}

void initialization()
{
  LSTM = SrH = SrM = SsH = SsM = 0;
  EoT = B = TCF = LST = LHA = Declination = 0;
  Elevation = Azimuth = Ele = Azi = Sunrise = Sunset = 0;
  
  NS = EW = '\0';
  
  Year = Month = Day = Hour = Minute = Second = 0;
  Latitude = Longitude = 0;
  
  preTime = curTime = SRT = SRT = 0;
}

void testData()
{
  
}

void getGPSData()
{
  String s = "";
  inByte = SerialGPS.read();
  if (inByte == '$')
  {
    int i = 0;
    while (i<6)
    {
      if (inByte != -1) s.concat(inByte);
      inByte = SerialGPS.read();
      if (inByte != -1) i++;
    }
  }
  if (s.equals(MARKER))
  {
    GPS.concat(s);
    GPS.concat(inByte);
    inByte = SerialGPS.read();
    while (inByte != '$')
    {
      if (inByte != -1) GPS.concat(inByte);
      inByte = SerialGPS.read();
    }      
    Serial.print(GPS);
  }
}

void parseGPS()
{
  int iIndex, jIndex;
  String s = "";
  char ch[15] = {'\0'};
  
  //Parse Timestamp.
  iIndex = 7;
  jIndex = GPS.indexOf(',', iIndex); 
  for (int i = iIndex; i<jIndex; i++) s.concat(GPS.charAt(i));
  Hour = s.toInt()/10000;
  if (Hour < 7) Hour = Hour + 17;
  else Hour = Hour + deltaGMT;
  Minute = s.toInt()/100%100;
  Second = s.toInt()%100;
  s = "";
  for (int i = 0; i<15; i++) ch[i]='\0';
  Serial.print("Time: ");
  Serial.print(Hour); Serial.print(":");
  Serial.print(Minute); Serial.print(":");
  Serial.println(Second);

  //Parse Latitude.
  iIndex = jIndex + 3;
  jIndex = GPS.indexOf(',', iIndex);
  for (int i = iIndex; i<jIndex; i++) ch[i-iIndex] = GPS.charAt(i);
  Latitude = (double)atof(ch)/100;
  NS = GPS.charAt(jIndex+1);
  if (NS == 'S') Latitude = -Latitude;
  Serial.print("Latitude: "); Serial.println(Latitude,2);
  for (int i = 0; i<15; i++) ch[i]='\0';
  
  //Parse Longitude.
  iIndex = jIndex + 3;
  jIndex = GPS.indexOf(',', iIndex);
  for (int i = iIndex; i<jIndex; i++) ch[i-iIndex] = GPS.charAt(i);
  Longitude = (double)atof(ch)/100;
  EW = GPS.charAt(jIndex+1);
  if (EW == 'W') Longitude = -Longitude;
  Serial.print("Longitude: "); Serial.println(Longitude,2);
  for (int i = 0; i<15; i++) ch[i]='\0';
  
  //Parse Date.
  iIndex = jIndex + 3;
  jIndex = GPS.indexOf(',', iIndex);
  iIndex = jIndex + 2;
  jIndex = GPS.indexOf(',', iIndex);
  for (int i = iIndex; i<jIndex; i++) s.concat(GPS.charAt(i));
  Serial.println(s.toInt());
  Day = s.toInt()/10000;
  if (Hour >= 17) Day = Day -1;
  Month = s.toInt()/100%100;
  Year = s.toInt()%100 + 2000;
  s = "";
  for (int i = 0; i<15; i++) ch[i]='\0';
  Serial.print("Date: ");
  Serial.print(Month); Serial.print("/");
  Serial.print(Day); Serial.print("/");
  Serial.println(Year);
}

boolean verifyData()
{
    if (Year > 2000 && Year < 2099) checksum++;
    if (Month > 0 && Month < 12) checksum++;
    if (Day >0 && Day < 32) checksum++;
    if (Hour >= 0 && Hour < 24) checksum++;
    if (Minute >= 0 && Minute <= 60) checksum++;
    if (Second >= 0 && Second <= 60) checksum++;
    if (floor(Latitude) == 39) checksum++;
    if (floor(abs(Longitude)) == 121) checksum++;
    
    if (checksum == 8) return true;
    else return false;
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

void calculateMove(int year, int month, int day, int hour, int minute, int second)
{
  LSTM = (double)15 * deltaGMT;
  B = 360.0/365.0 * (getDayOfYear(year, month, day) - 81);
  EoT = 9.87*sin(2*B*DTOR) - 7.53*cos(B*DTOR) - 1.5*sin(B*DTOR);
  TCF = 4*(Longitude - LSTM) + EoT;
  LST = timeConvert(hour, minute, second) + TCF/60.0;
  LHA = 15.0*(LST-12.0);
  Declination = 23.45*sin(B*DTOR);
  Ele = asin( sin(Declination*DTOR)*sin(Latitude*DTOR) + cos(Declination*DTOR)*cos(Latitude*DTOR)*cos(LHA*DTOR) ) * RTOD;
  Elevation = abs(Ele);
  Azi = acos( (sin(Declination*DTOR)*cos(Latitude*DTOR) - cos(Declination*DTOR)*sin(Latitude*DTOR)*cos(LHA*DTOR))/cos(Elevation*DTOR) ) * RTOD;
  if (LST > 12) Azimuth = 360 - Azi; else Azimuth = Azi;

  Serial.print("Day of Year: "); Serial.println(getDayOfYear(year, month, day));
  Serial.print("EoT:  "); Serial.println(EoT);
  Serial.print("Time Correction:  "); Serial.println(TCF);
  Serial.print("Declination:  "); Serial.println(Declination);
  Serial.print("Hour Angle:  "); Serial.println(LHA);
  Serial.print("Altitude/Elevation:  "); Serial.println(Elevation);
  Serial.print("Azimuth:  "); Serial.println(Azimuth);

  double Sunrise = 12 - acos(-tan(Latitude*DTOR)*tan(Declination*DTOR)) * RTOD / 15.0 - TCF/60.0;
  double Sunset = 12 + acos(-tan(Latitude*DTOR)*tan(Declination*DTOR)) * RTOD / 15.0 - TCF/60.0;
  int SrH, SrM, SsH, SsM;
  SrH = (int)Sunrise;
  SrM = (int)round((Sunrise-SrH) * 60);
  SsH = (int)Sunset;
  SsM = (int)round((Sunset-SsH) * 60);
  
  Serial.print("Sunrise: "); Serial.print(SrH); Serial.print(":"); Serial.println(SrM);
  Serial.print("Sunset: "); Serial.print(SsH); Serial.print(":"); Serial.println(SsM);
}
