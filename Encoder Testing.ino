/*Encoder testing*/

#include <Encoder.h>

#define encoder1Pin1 2
#define encoder1Pin2 40
#define encoder2Pin1 3
#define encoder2Pin2 41
#define CW "ClockWise"
#define CCW "CounterClockWise"

Encoder encoderOne(encoder1Pin1, encoder1Pin2);
Encoder encoderTwo(encoder2Pin1, encoder2Pin2);
long encoder1Current, encoder2Current, encoder1Pre, encoder2Pre;
String rotateDirection1, rotateDirection2;

void setup()
{
  Serial.begin(9600);
  encoderInit();  
}

void loop()
{
  encoder1Current = encoderOne.read();
  encoder2Current = encoderTwo.read();
  
  if (encoder1Current > encoder1Pre) rotateDirection1 = CW;
  if (encoder1Current < encoder1Pre) rotateDirection1 = CCW;
  if (encoder2Current > encoder2Pre) rotateDirection2 = CW;
  if (encoder2Current < encoder2Pre) rotateDirection2 = CCW;
  
  encoder1Pre = encoder1Current;
  encoder2Pre = encoder2Current;
  
  Serial.print("Encoder 1: ");
  Serial.print(encoder1Current);
  Serial.print(" ");
  Serial.print(rotateDirection1);
  Serial.print(", Encoder 2: ");
  Serial.print(encoder2Current);
  Serial.print("  ");
  Serial.println(rotateDirection2);
}

void encoderInit()
{
  pinMode(encoder1Pin1, INPUT);
  pinMode(encoder1Pin2, INPUT);
  pinMode(encoder2Pin1, INPUT);
  pinMode(encoder2Pin2, INPUT);
  encoder1Pre = encoder2Pre = 0;
  rotateDirection1 = rotateDirection2 = "";
//https://www.pjrc.com/teensy/td_libs_Encoder.html
}
