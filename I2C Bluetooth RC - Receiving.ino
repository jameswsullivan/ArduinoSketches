#include <I2C_Anything.h>
#include <LCD4884.h>
#include <Wire.h>

//LCD Initialization
#define STARTPOSX 10
#define STARTPOSY 1
#define DELAY1 100
#define MULTI 6
//End of LCD

//Throttle Initialization
char Throttle1[5], Throttle2[5], Encoder1[7], Encoder2[7];
int throttle1Pre, throttle2Pre, TH1, TH2;
//End of Throttle

//Encoder Initialization
long encoder1Pre, encoder2Pre, EN1, EN2;
boolean Encoder1Dir, Encoder2Dir;
//End of Encoder

//Joystick Initialization
int JLX, JLY, JRX, JRY, JL, JR;
#define JOYSTICKLIMITH 600
#define JOYSTICKLIMITL 400
//End of Joystick

//Keys Initialization
boolean Key1, Key2, Key3, Key4, Key5;
unsigned char on[6] = {0x00, 0x1C, 0x3E, 0x3E, 0x3E, 0x1C}, off[6] = {0x00, 0x1C, 0x22, 0x22, 0x22, 0x1C};
//End of Keys

void setup()
{
  Serial.begin(9600);
  Wire.begin(2);
  Wire.onReceive(processSensorValue);
  
  lcd.LCD_init();
  lcd.LCD_clear();
  throttle1Pre = throttle2Pre = 0;
  encoder1Pre = encoder2Pre = 0;  
  Encoder1Dir = Encoder2Dir = true;
  JL = JR = 0;
  Key1 = Key2 = Key3 = Key4 = Key5 = false;
  
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
}

void loop()
{
  displayTitle();
  displayThrottle();
  displayEncoder();
  displayJoystick();
  displayKeys();
  
  clearScreen();
}

void serialDisplayTest()
{
  /*
  Serial.print(TH1);
  Serial.print(" ");
  Serial.print(throttle1Pre);
  Serial.print(" ");
  Serial.println(TH2);
  
  Serial.print("JLX: ");
  Serial.print(JLX);
  Serial.print(" JLY: ");
  Serial.print(JLY);
  Serial.print(" JRX: ");
  Serial.print(JRX);
  Serial.print(" JRY: ");
  Serial.println(JRY);
  
  Serial.print("JL: ");
  Serial.print(JLL);
  Serial.print(" ");
  Serial.print(JLR);
  Serial.print(" ");
  Serial.print(JLF);
  Serial.print(" ");
  Serial.println(JLB);

  Serial.print(Key1);
  Serial.print(" ");
  Serial.print(Key2);
  Serial.print(" ");
  Serial.print(Key3);
  Serial.print(" ");
  Serial.print(Key4);
  Serial.print(" ");
  Serial.println(Key5);
  */
}

void clearScreen(void)
{
  if (
  (TH1 < 1000 && throttle1Pre > 1000) || (TH1 < 100 && throttle1Pre > 100) || (TH1 < 9 && throttle1Pre > 10)
  || (TH2 < 1000 && throttle2Pre > 1000) || (TH2 < 100 && throttle2Pre > 100) || (TH2 < 9 && throttle2Pre > 10) 
  || (EN1 < 10 && encoder1Pre > 10) || (EN1 < 100 && encoder1Pre > 100) || (EN1 < 1000 && encoder1Pre > 1000)
  || (EN1 < 10000 && encoder1Pre > 10000) || (EN1 < 100000 && encoder1Pre > 100000) || (EN1 < 1000000 && encoder1Pre > 1000000)
  || (EN1 < 10000000 && encoder1Pre > 10000000)
  || (EN2 < 10000 && encoder2Pre > 10000) || (EN2 < 100000 && encoder2Pre > 100000) || (EN2 < 1000000 && encoder2Pre > 1000000)
  || (EN2 < 10000000 && encoder2Pre > 10000000)
  )
  { lcd.LCD_clear(); }
  
  if (throttle1Pre != TH1 || throttle2Pre != TH2) throttle1Pre = TH1, throttle2Pre = TH2;
  if (EN1 != encoder1Pre || EN2 != encoder2Pre) encoder1Pre = EN1, encoder2Pre = EN2;
}

void processSensorValue(int numBytes)
{
  I2C_readAnything(TH1);
  I2C_readAnything(TH2);
  I2C_readAnything(EN1);
  I2C_readAnything(Encoder1Dir);
  I2C_readAnything(EN2);
  I2C_readAnything(Encoder2Dir);
  I2C_readAnything(JLX);
  I2C_readAnything(JLY);
  I2C_readAnything(JRX);
  I2C_readAnything(JRY);
  I2C_readAnything(Key1);
  I2C_readAnything(Key2);
  I2C_readAnything(Key3);
  I2C_readAnything(Key4);
  I2C_readAnything(Key5);
}

void displayTitle()
{
  lcd.LCD_write_string(MULTI*4, 0, "MONITOR", MENU_NORMAL);
}

void displayThrottle()
{
  itoa(TH1, Throttle1, 10);
  itoa(TH2, Throttle2, 10);
  lcd.LCD_write_string(0, 1, "TH:", MENU_NORMAL);
  lcd.LCD_write_string(MULTI*3,1,Throttle1,MENU_NORMAL);
  lcd.LCD_write_string(MULTI*8,1,Throttle2,MENU_NORMAL);
}

void displayEncoder()
{
  itoa(EN1, Encoder1, 10);
  itoa(EN2, Encoder2, 10);
  lcd.LCD_write_string(0, 2, "EN1:", MENU_NORMAL);
  if (Encoder1Dir) lcd.LCD_write_string(MULTI*4, 2, "+", MENU_NORMAL);
  else lcd.LCD_write_string(MULTI*4, 2, "-", MENU_NORMAL);
  lcd.LCD_write_string(MULTI*5, 2, Encoder1, MENU_NORMAL);
  
  lcd.LCD_write_string(0, 3, "EN2:", MENU_NORMAL);
  if (Encoder2Dir) lcd.LCD_write_string(MULTI*4, 3, "+", MENU_NORMAL);
  else lcd.LCD_write_string(MULTI*4, 3, "-", MENU_NORMAL);
  lcd.LCD_write_string(MULTI*5, 3, Encoder2, MENU_NORMAL);
}

void displayJoystick()
{
  if (JLX < JOYSTICKLIMITH && JLX > JOYSTICKLIMITL && JLY < JOYSTICKLIMITH && JLY > JOYSTICKLIMITL) JL = 0;
  else if (JLX > JOYSTICKLIMITL && JLX < JOYSTICKLIMITH && JLY < JOYSTICKLIMITL) JL = 1;
  else if (JLX > JOYSTICKLIMITH && JLY < JOYSTICKLIMITL) JL = 2;
  else if (JLX > JOYSTICKLIMITH && JLY > JOYSTICKLIMITL && JLY < JOYSTICKLIMITH) JL = 3;
  else if (JLX > JOYSTICKLIMITH && JLY > JOYSTICKLIMITH) JL = 4;
  else if (JLX > JOYSTICKLIMITL && JLX < JOYSTICKLIMITH && JLY > JOYSTICKLIMITH) JL = 5;
  else if (JLX < JOYSTICKLIMITL && JLY > JOYSTICKLIMITH) JL = 6;
  else if (JLX < JOYSTICKLIMITL && JLY > JOYSTICKLIMITL && JLY < JOYSTICKLIMITH) JL = 7;
  else if (JLX < JOYSTICKLIMITL && JLY < JOYSTICKLIMITL ) JL = 8;
  
  if (JRX < JOYSTICKLIMITH && JRX > JOYSTICKLIMITL && JRY < JOYSTICKLIMITH && JRY > JOYSTICKLIMITL) JR = 0;
  else if (JRX > JOYSTICKLIMITL && JRX < JOYSTICKLIMITH && JRY < JOYSTICKLIMITL) JR = 1;
  else if (JRX > JOYSTICKLIMITH && JRY < JOYSTICKLIMITL) JR = 2;
  else if (JRX > JOYSTICKLIMITH && JRY > JOYSTICKLIMITL && JRY < JOYSTICKLIMITH) JR = 3;
  else if (JRX > JOYSTICKLIMITH && JRY > JOYSTICKLIMITH) JR = 4;
  else if (JRX > JOYSTICKLIMITL && JRX < JOYSTICKLIMITH && JRY > JOYSTICKLIMITH) JR = 5;
  else if (JRX < JOYSTICKLIMITL && JRY > JOYSTICKLIMITH) JR = 6;
  else if (JRX < JOYSTICKLIMITL && JRY > JOYSTICKLIMITL && JRY < JOYSTICKLIMITH) JR = 7;
  else if (JRX < JOYSTICKLIMITL && JRY < JOYSTICKLIMITL ) JR = 8;
  
  switch(JL)
  {
    case 0: lcd.LCD_write_string(0, 4 , "JY1:CE", MENU_NORMAL); break;
    case 1: lcd.LCD_write_string(0, 4, "JY1:FW", MENU_NORMAL); break;
    case 2: lcd.LCD_write_string(0, 4, "JY1:LF", MENU_NORMAL); break;
    case 3: lcd.LCD_write_string(0, 4, "JY1:LT", MENU_NORMAL); break;
    case 4: lcd.LCD_write_string(0, 4, "JY1:LB", MENU_NORMAL); break;
    case 5: lcd.LCD_write_string(0, 4, "JY1:BW", MENU_NORMAL); break;
    case 6: lcd.LCD_write_string(0, 4, "JY1:RB", MENU_NORMAL); break;
    case 7: lcd.LCD_write_string(0, 4, "JY1:RT", MENU_NORMAL); break;
    case 8: lcd.LCD_write_string(0, 4, "JY1:RF", MENU_NORMAL); break;
    default: break;
  }
  
  switch(JR)
  {
    case 0: lcd.LCD_write_string(MULTI*7, 4, "JY2:CE", MENU_NORMAL); break;
    case 1: lcd.LCD_write_string(MULTI*7, 4, "JY2:FW", MENU_NORMAL); break;
    case 2: lcd.LCD_write_string(MULTI*7, 4, "JY2:LF", MENU_NORMAL); break;
    case 3: lcd.LCD_write_string(MULTI*7, 4, "JY2:LT", MENU_NORMAL); break;
    case 4: lcd.LCD_write_string(MULTI*7, 4, "JY2:LB", MENU_NORMAL); break;
    case 5: lcd.LCD_write_string(MULTI*7, 4, "JY2:BW", MENU_NORMAL); break;
    case 6: lcd.LCD_write_string(MULTI*7, 4, "JY2:RB", MENU_NORMAL); break;
    case 7: lcd.LCD_write_string(MULTI*7, 4, "JY2:RT", MENU_NORMAL); break;
    case 8: lcd.LCD_write_string(MULTI*7, 4, "JY2:RF", MENU_NORMAL); break;
    default: break;
  }
  
}

void displayKeys()
{
  lcd.LCD_write_string(0, 5, "K: ", MENU_NORMAL);
  if (Key1) OnOff(MULTI*3, 5, on); else OnOff(MULTI*3, 5, off);
  if (Key2) OnOff(MULTI*5, 5, on); else OnOff(MULTI*5, 5, off);
  if (Key3) OnOff(MULTI*7, 5, on); else OnOff(MULTI*7, 5, off);
  if (Key4) OnOff(MULTI*9, 5, on); else OnOff(MULTI*9, 5, off);
  if (Key5) OnOff(MULTI*11, 5, on); else OnOff(MULTI*11, 5, off);
}

void OnOff(unsigned char X, unsigned char Y, unsigned char *s)
{
  int i;
  for (i=0; i<6; i++) 
  {
    lcd.LCD_set_XY(X+i, Y);
    lcd.LCD_write_byte(s[i], 1);
  }
}

//http://www.gammon.com.au/forum/?id=10896
