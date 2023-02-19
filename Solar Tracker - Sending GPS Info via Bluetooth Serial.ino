/*Sending GPS info via Bluetooth Serial*/

#include <math.h>
#include <SoftwareSerial.h>

//SolarTracking Calculation
#define PRESETELEVATION 13.6
#define DTOR (3.14/180)
#define RTOD (180/3.14)
#define deltaGMT -7
#define AZIOFFSET 90
double Latitude, Longitude;
int Year, Month, Day, Hour, Minute, Second, LSTM, SrH, SrM, SsH, SsM;
double EoT, B, TCF, LST, LHA, Declination, Elevation, Azimuth, Azi, Sunrise, Sunset;
double PreEle, PreAzi;
//End of SolarTracking Calculation

//GPS
#define MARKER "$GPRMC"
String GPS = "";
char inByte = '\0';
char NS, EW; //GPS Latitude Longitude Direction.
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
  initialization();
}

void loop()
{
  while (true)
  {
    while (SerialGPS.available())
    {
      getGPSData();
    }

    if (verifyData())
    {
      PreEle = PRESETELEVATION;
      PreAzi = AZIOFFSET;
      ENX.write(0);
      ENY.write(0);
      SunriseSunset(Year, Month, Day);
    }
    
    GPS = "";
  }
}

void initialization()
{
  Elevation = Azimuth = 0;
  Year = Month = Day = Hour = Minute = Second = 0;
  LSTM = SrH = SrM = SsH = SsM = 0;
  EoT = B = TCF = LST = LHA = Declination = 0;
  Elevation = Azimuth = Azi = Sunrise = Sunset = 0;
  NS = EW = '\0';
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
    parseGPS();
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

void sunPos(int year, int month, int day, int hour, int minute, int second)
{
  LSTM = (double)15 * deltaGMT;
  B = 360.0/365.0 * (getDayOfYear(year, month, day) - 81);
  EoT = 9.87*sin(2*B*DTOR) - 7.53*cos(B*DTOR) - 1.5*sin(B*DTOR);
  TCF = 4*(Longitude - LSTM) + EoT;
  LST = timeConvert(hour, minute, second) + TCF/60.0;
  LHA = 15.0*(LST-12.0);
  Declination = 23.45*sin(B*DTOR);
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
