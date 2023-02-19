/*LCD4884 Testing Code*/

#include "LCD4884.h"

#define STARTPOSX 10
#define STARTPOSY 1
#define DELAY1 100
#define MULTI 6
char throttle1[5];
char throttle2[5];
int throttle1Pre, throttle2Pre, TH1, TH2;

void setup()
{
  Serial.begin(9600);
  lcd.LCD_init();
  lcd.LCD_clear();
  throttle1Pre = throttle2Pre = 0;
}

void loop()
{
  TH1 = analogRead(A1);
  TH2 = analogRead(A1);
  itoa(TH1, throttle1, 10);
  lcd.LCD_write_string(0, 0, "TH:", MENU_NORMAL);
  lcd.LCD_write_string(MULTI*3,0,throttle1,MENU_NORMAL);
  lcd.LCD_write_string(MULTI*8,0,throttle1,MENU_NORMAL);
  
  clearScreen();
  
  throttle1Pre = TH1, throttle2Pre = TH2;
}

void clearScreen(void)
{
  if ( (TH1 == 999 && throttle1Pre == 1000) || (TH1 == 99 && throttle1Pre == 100) || (TH1 == 9 && throttle1Pre == 10)
    || (TH2 == 999 && throttle2Pre == 1000) || (TH2 == 99 && throttle2Pre == 100) || (TH2 == 9 && throttle2Pre == 10) )
  { lcd.LCD_clear(); }
}
