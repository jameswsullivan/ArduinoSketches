/*
This sketch tests data received from bluetooth via Software Serial.

This sketch was part of the Arduino RC controller project.
In this project, two Arduino boards communite with each other via bluetooth,
one board is used to control the other. This project can be used in various RC
control scenarios for smart cars or home automation projects.
*/

#include <SoftwareSerial.h>

SoftwareSerial mySerial(12,11);
String receivedData, xReceived, yReceived;
char rawData[2][10];
float rawX, rawY;

void setup()
{
  mySerial.begin(9600);
}

void loop()
{
  char inchar = '\0';
  int xEnd = 0;
  int stringLength = 0;
  
  while ( mySerial.available() && (inchar != '\r') )
  {
    inchar = mySerial.read();
    receivedData.concat(inchar);
    mySerial.print(inchar);
  }
  
  xEnd = receivedData.indexOf(' ');
  stringLength = receivedData.length();
  
  xReceived = receivedData.substring(0,xEnd);
  yReceived = receivedData.substring(xEnd+1,stringLength-1);  
  
  xReceived.toCharArray(rawData[0],10);
  yReceived.toCharArray(rawData[1],10);
  
  rawX = atof(rawData[0]);
  rawY = atof(rawData[1]);
  
  if (inchar == '\r')
  {
    mySerial.println("You input: " + receivedData);
    mySerial.println("The xReceived is: " + xReceived);
    mySerial.println("The yReceived is: " + yReceived);
    mySerial.print("The rawData[0] is: "); mySerial.println(rawData[0]);
    mySerial.print("The rawData[1] is: "); mySerial.println(rawData[1]);
    mySerial.print("The converted x is : "); mySerial.println(rawX,3);
    mySerial.print("The converted y is : "); mySerial.println(rawY,3);
    receivedData = "";
    xReceived = "";
    yReceived = "";
    inchar = '\0';
    rawX = rawY = 0;
  }
}