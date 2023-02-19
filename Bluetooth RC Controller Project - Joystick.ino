/*
This sketch defines the basic functions of a joystick RC controller.

This sketch was part of the Arduino RC controller project.
In this project, two Arduino boards communite with each other via bluetooth,
one board is used to control the other. This project can be used in various RC
control scenarios for smart cars or home automation projects.
*/


// Pin Setup
#define JOY_STICK_1_PIN 32
#define JOY_STICK_2_PIN 33
#define BUTTON_1 34
#define BUTTON_2 35
#define BUTTON_3 36
#define BUTTON_4 37
#define BUTTON_5 38
/* Throttle 1: A0, Throttle 2: A1, Joy Stick 1-x: A2, Joy Stick 1-y: A3,
Joy Stick 2-x: A4, Joy Stick 2-y: A5 */

int throttle1, throttle2, joyStick1x, joyStick1y, joyStick2x, joyStick2y;
int button1, button2, button3, button4, button5, joyStick1Button, joyStick2Button;

void setup()
{
  Serial.begin(9600);
  pinMode(JOY_STICK_1_PIN, INPUT);
  pinMode(JOY_STICK_2_PIN, INPUT);
  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_2, INPUT);
  pinMode(BUTTON_3, INPUT);
  pinMode(BUTTON_4, INPUT);
  pinMode(BUTTON_5, INPUT);
}

void loop()
{
   throttle1 = analogRead(A0);
   throttle2 = analogRead(A1);
   joyStick1x = analogRead(A2);
   joyStick1y = analogRead(A3);
   joyStick2x = analogRead(A4);
   joyStick2y = analogRead(A5);
   
   button1 = digitalRead(BUTTON_1);
   button2 = digitalRead(BUTTON_2);
   button3 = digitalRead(BUTTON_3);
   button4 = digitalRead(BUTTON_4);
   button5 = digitalRead(BUTTON_5);
   joyStick1Button = digitalRead(JOY_STICK_1_PIN);
   joyStick2Button = digitalRead(JOY_STICK_2_PIN);
   
   printAllValues();   
}

void printAllValues()
{
  Serial.print("Throttle 1: ");
  Serial.print(throttle1);
  Serial.print("  Throttle 2: ");
  Serial.println(throttle2);
  Serial.print("JoyStick1-X: ");
  Serial.print(joyStick1x);
  Serial.print("  JoyStick1-Y: ");
  Serial.print(joyStick1y);
  Serial.print("  JoyStick2-X: ");
  Serial.print(joyStick2x);
  Serial.print("  JoyStick2-Y: ");
  Serial.println(joyStick2y);
  Serial.print("Button 1: ");
  Serial.print(button1);
  Serial.print("  Button 2: ");
  Serial.print(button2);
  Serial.print("  Button 3: ");
  Serial.print(button3);
  Serial.print("  Button 4: ");
  Serial.print(button4);
  Serial.print("  Button 5: ");
  Serial.println(button5);
  Serial.print("JoyStick1-Button: ");
  Serial.print(joyStick1Button);
  Serial.print("  JoyStick2-Button: ");
  Serial.println(joyStick2Button);
  Serial.println();
  delay(1000);
}