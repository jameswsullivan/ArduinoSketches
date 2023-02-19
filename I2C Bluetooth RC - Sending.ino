#include <Encoder.h>
#include <I2C_Anything.h>
#include <Wire.h>

//Encoder Initialization
#define encoder1Pin1 2
#define encoder1Pin2 40
#define encoder2Pin1 3
#define encoder2Pin2 41
#define CW "ClockWise"
#define CCW "CounterClockWise"
Encoder encoderOne(encoder1Pin1, encoder1Pin2);
Encoder encoderTwo(encoder2Pin1, encoder2Pin2);
long encoder1Current, encoder2Current, encoder1Pre, encoder2Pre;
boolean rotateDirection1, rotateDirection2;
//End of Encoder

//Joystick Initialization
int JLX, JLY, JRX, JRY;
//End of Joystick

//Throttle Initialization
int Throttle1, Throttle2;
//End of Throttle

// Keys Initialization
boolean Key1, Key2, Key3, Key4, Key5;
#define KEY1 34
#define KEY2 35
#define KEY3 36
#define KEY4 37
#define KEY5 38
//End of Keys

void setup()
{
  Wire.begin();
  encoderInit();
  keysInit();
  Serial.begin(9600);  
}

void loop()
{
  getThrottleValue();
  getEncoderValue();
  getJoystickValue();
  getKeysValue();
  
  sendSensorValue();
  delay(150);
}

void sendSensorValue()
{
  Wire.beginTransmission(2);
  I2C_writeAnything(Throttle1);
  I2C_writeAnything(Throttle2);
  I2C_writeAnything(encoder1Current);
  I2C_writeAnything(rotateDirection1);
  I2C_writeAnything(encoder2Current);
  I2C_writeAnything(rotateDirection2);
  I2C_writeAnything(JLX);
  I2C_writeAnything(JLY);
  I2C_writeAnything(JRX);
  I2C_writeAnything(JRY);
  I2C_writeAnything(Key1);
  I2C_writeAnything(Key2);
  I2C_writeAnything(Key3);
  I2C_writeAnything(Key4);
  I2C_writeAnything(Key5);
  Wire.endTransmission(); 
}

void encoderInit() //Encoder Initialization Function
{
  pinMode(encoder1Pin1, INPUT);
  pinMode(encoder1Pin2, INPUT);
  pinMode(encoder2Pin1, INPUT);
  pinMode(encoder2Pin2, INPUT);
  encoder1Pre = encoder2Pre = 0;
  rotateDirection1 = rotateDirection2 = true;
}

void keysInit()
{
  pinMode(KEY1, INPUT);
  pinMode(KEY2, INPUT);
  pinMode(KEY3, INPUT);
  pinMode(KEY4, INPUT);
  pinMode(KEY5, INPUT);
  Key1 = Key2 = Key3 = Key4 = Key5 = false;
}

void getThrottleValue()
{
  Throttle1 = analogRead(A0);
  Throttle2 = analogRead(A1);
}

void getEncoderValue()
{
  encoder1Current = encoderOne.read();
  encoder2Current = encoderTwo.read();
  
  if (encoder1Current > encoder1Pre) rotateDirection1 = true;
  if (encoder1Current < encoder1Pre) rotateDirection1 = false;
  if (encoder2Current > encoder2Pre) rotateDirection2 = true;
  if (encoder2Current < encoder2Pre) rotateDirection2 = false;
  
  encoder1Pre = encoder1Current;
  encoder2Pre = encoder2Current;

}

void getJoystickValue()
{
  JLX = analogRead(A3);
  JLY = analogRead(A2);
  JRX = analogRead(A5);
  JRY = analogRead(A4);
}

void getKeysValue()
{
  if (!digitalRead(KEY1)) Key1 = !Key1;
  if (!digitalRead(KEY2)) Key2 = !Key2;
  if (!digitalRead(KEY3)) Key3 = !Key3;
  if (!digitalRead(KEY4)) Key4 = !Key4;
  if (!digitalRead(KEY5)) Key5 = !Key5;
}

void encoderDisplayTest()
{
  Serial.print("Encoder 1: ");
  Serial.print(encoder1Current);
  Serial.print(" ");
  Serial.print(rotateDirection1);
  Serial.print(", Encoder 2: ");
  Serial.print(encoder2Current);
  Serial.print("  ");
  Serial.println(rotateDirection2);
}

void joystickDisplayTest()
{
  Serial.print("JLX: ");
  Serial.print(JLX);
  Serial.print(" JLY: ");
  Serial.print(JLY);
  Serial.print(" JRX: ");
  Serial.print(JRX);
  Serial.print("  JRY: ");
  Serial.println(JRY);
}

void keysDisplayTest()
{
  Serial.print("KEYS: ");
  Serial.print(Key1);
  Serial.print(" ");
  Serial.print(Key2);
  Serial.print(" ");
  Serial.print(Key3);
  Serial.print(" ");
  Serial.print(Key4);
  Serial.print(" ");
  Serial.println(Key5);
  
  Serial.print("BUttons: ");
  Serial.print(digitalRead(KEY1));
  Serial.print(" ");
  Serial.print(digitalRead(KEY2));
  Serial.print(" ");
  Serial.print(digitalRead(KEY3));
  Serial.print(" ");
  Serial.print(digitalRead(KEY4));
  Serial.print(" ");
  Serial.println(digitalRead(KEY5));
}
