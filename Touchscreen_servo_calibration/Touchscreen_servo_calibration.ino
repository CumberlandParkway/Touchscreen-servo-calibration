/*
Written by Adrian Garnham
Adapted from too many sources to list

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x40);
int Centre = 1500;
#define MINPRESSURE 200
#define MAXPRESSURE 1000

const int XP=6, XM=A2, YP=A1, YM=7; //240x320 ID=0x9341
const int TS_LEFT=161, TS_RT=935, TS_TOP=170, TS_BOT=930;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
Adafruit_GFX_Button servo_num_btn, close_btn, throw_btn, minus_btn, plus_btn, minus50_btn, plus50_btn, print_btn;

int ServoNum = 0;
int Throw[] = {1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500};
int Close[] = {1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500};

int ButtonPressDelay = 100;

String ThrowVals = "";
String CloseVals = "";

int servo_num_btn_pressed = 0;
int close_btn_pressed = 0;
int throw_btn_pressed = 0;
int minus_btn_pressed = 0;
int plus_btn_pressed = 0;
int minus50_btn_pressed = 0;
int plus50_btn_pressed = 0;
int print_btn_pressed = 0;

int pixel_x, pixel_y;     //Touch_getXY() updates global vars

bool Touch_getXY(void)
{
TSPoint p = ts.getPoint();
pinMode(YP, OUTPUT);      //restore shared pins
pinMode(XM, OUTPUT);      //because TFT control pins
bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
if (pressed) {
pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
}
return pressed;
}

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

void setup() {
Serial.begin(9600);

uint16_t ID = tft.readID();
tft.begin(ID);
tft.setRotation(0);            //Landscape
tft.fillScreen(BLACK);
servo_num_btn.initButton(&tft, 40, 20, 70, 30, WHITE, CYAN, BLACK, "SERVO", 2);
servo_num_btn.drawButton(false);

close_btn.initButton(&tft, 40, 80, 70, 30, WHITE, CYAN, BLACK, "CLOSE", 2);
close_btn.drawButton(false);

throw_btn.initButton(&tft,40, 140, 70, 30, WHITE, CYAN, BLACK, "THROW", 2);
throw_btn.drawButton(false);

minus_btn.initButton(&tft,120, 190, 50, 30, WHITE, CYAN, BLACK, "-", 2);
minus_btn.drawButton(false);

plus_btn.initButton(&tft,215, 190, 50, 30, WHITE, CYAN, BLACK, "+", 2);
plus_btn.drawButton(false);

minus50_btn.initButton(&tft,120, 240, 50, 30, WHITE, CYAN, BLACK, "-50", 2);
minus50_btn.drawButton(false);

plus50_btn.initButton(&tft,215, 240, 50, 30, WHITE, CYAN, BLACK, "+50", 2);
plus50_btn.drawButton(false);

print_btn.initButton(&tft,120, 290, 240, 30, WHITE, CYAN, BLACK, "PRINT", 2);
print_btn.drawButton(false);

tft.setTextColor(WHITE);
tft.setTextSize(2);

tft.setCursor(140, 10);
tft.print(ServoNum);

tft.setCursor(140, 70);
tft.print(Close[0]);

tft.setCursor(140, 130);
tft.print(Throw[0]);

//centre the servo's
pwm1.begin();
pwm1.setPWMFreq(50);

for(int i=0; i<16; i++){pwm1.writeMicroseconds(i, Centre); delay(500);}
}

void PrintToSerialMonitor(){
ThrowVals = Throw[0];
for(int i=1; i<16; i++){
  ThrowVals = ThrowVals + "," + Throw[i];
  }
Serial.println("int Throw[] = {"+ThrowVals+"};");
CloseVals = Close[0];
for(int i=1; i<16; i++){
  CloseVals = CloseVals + "," + Close[i];
  }
Serial.println("int Close[] = {"+CloseVals+"};");
delay(5000);
print_btn.drawButton(false);
print_btn_pressed = 0;
}

void ShowServoValues(bool MoveServo, int Direction){
tft.setTextSize(2);
tft.setTextColor(WHITE);
tft.fillRect(130, 10, 140, 40, BLACK);
tft.setCursor(140, 10);
tft.print(ServoNum);

tft.fillRect(130, 70, 140, 40, BLACK);
tft.setCursor(140, 70);
tft.print(Close[ServoNum]);

tft.fillRect(130, 130, 140, 40, BLACK);
tft.setCursor(140, 130);
tft.print(Throw[ServoNum]);

if(MoveServo){
  if(Direction == 0){pwm1.writeMicroseconds(ServoNum, Close[ServoNum]);}
  if(Direction == 1){pwm1.writeMicroseconds(ServoNum, Throw[ServoNum]);}
  }
}

void loop(void)
{
//check for screen touch position
bool down = Touch_getXY();
servo_num_btn.press(down && servo_num_btn.contains(pixel_x, pixel_y));
close_btn.press(down && close_btn.contains(pixel_x, pixel_y));
throw_btn.press(down && throw_btn.contains(pixel_x, pixel_y));
minus_btn.press(down && minus_btn.contains(pixel_x, pixel_y));
plus50_btn.press(down && plus50_btn.contains(pixel_x, pixel_y));
minus50_btn.press(down && minus50_btn.contains(pixel_x, pixel_y));
plus_btn.press(down && plus_btn.contains(pixel_x, pixel_y));
print_btn.press(down && print_btn.contains(pixel_x, pixel_y));

//SERVO button pressed
if (servo_num_btn.justPressed() && servo_num_btn_pressed == 0) {
  servo_num_btn.drawButton(true);
  close_btn.drawButton(false);
  throw_btn.drawButton(false);
  minus_btn.drawButton(false);
  plus_btn.drawButton(false);
  servo_num_btn_pressed = 1;
  close_btn_pressed = 0;
  throw_btn_pressed = 0;
  minus_btn_pressed = 0;
  plus_btn_pressed = 0;
  minus50_btn_pressed = 0;
  plus50_btn_pressed = 0;
  }
if(servo_num_btn_pressed == 1){
  if(plus_btn.justPressed()){
    plus_btn.drawButton(true);
    minus_btn.drawButton(false);
    if(ServoNum<15){ServoNum++;}else{ServoNum = 0;}
    ShowServoValues(false,2);
    delay(ButtonPressDelay);
    }
  if(minus_btn.justPressed()){
    plus_btn.drawButton(false);
    minus_btn.drawButton(true);
    if(ServoNum>0){ServoNum--;}else{ServoNum = 15;}
    ShowServoValues(false,2);
    delay(ButtonPressDelay);
    }  
  }

//CLOSE button pressed
if (close_btn.justPressed() && close_btn_pressed == 0) {
  servo_num_btn.drawButton(false);
  close_btn.drawButton(true);
  throw_btn.drawButton(false);
  minus_btn.drawButton(false);
  plus_btn.drawButton(false);
  plus50_btn.drawButton(false);
  minus50_btn.drawButton(false);
  servo_num_btn_pressed = 0;
  close_btn_pressed = 1;
  throw_btn_pressed = 0;
  minus_btn_pressed = 0;
  plus_btn_pressed = 0;
  minus50_btn_pressed = 0;
  plus50_btn_pressed = 0;
  ShowServoValues(true,0);
  }
  
if(close_btn_pressed == 1){
  if(plus_btn.justPressed()){
    plus_btn.drawButton(true);
    minus_btn.drawButton(false);
    plus50_btn.drawButton(false);
    minus50_btn.drawButton(false);
    if(Close[ServoNum]<2000){Close[ServoNum] = Close[ServoNum] + 10;}
    ShowServoValues(true,0);
    delay(ButtonPressDelay);
    }
  if(minus_btn.justPressed()){
    plus_btn.drawButton(false);
    minus_btn.drawButton(true);
    plus50_btn.drawButton(false);
    minus50_btn.drawButton(false);
    if(Close[ServoNum]>1000){Close[ServoNum] = Close[ServoNum] - 10;}
    ShowServoValues(true,0);
    delay(ButtonPressDelay);
    }
  if(plus50_btn.justPressed()){
    plus_btn.drawButton(false);
    minus_btn.drawButton(false);
    plus50_btn.drawButton(true);
    minus50_btn.drawButton(false);
    if(Close[ServoNum]<=1950){Close[ServoNum] = Close[ServoNum] + 50;}
    ShowServoValues(true,0);
    delay(ButtonPressDelay);
    }
  if(minus50_btn.justPressed()){
    plus_btn.drawButton(false);
    minus_btn.drawButton(false);
    plus50_btn.drawButton(false);
    minus50_btn.drawButton(true);
    if(Close[ServoNum]>=1050){Close[ServoNum] = Close[ServoNum] - 50;}
    ShowServoValues(true,0);
    delay(ButtonPressDelay);
    }  
  }

//THROW button pressed
if (throw_btn.justPressed() && throw_btn_pressed == 0) {
  servo_num_btn.drawButton(false);
  close_btn.drawButton(false);
  throw_btn.drawButton(true);
  minus_btn.drawButton(false);
  plus_btn.drawButton(false);
  plus50_btn.drawButton(false);
  minus50_btn.drawButton(false);
  servo_num_btn_pressed = 0;
  close_btn_pressed = 0;
  throw_btn_pressed = 1;
  minus_btn_pressed = 0;
  plus_btn_pressed = 0;
  minus50_btn_pressed = 0;
  plus50_btn_pressed = 0;
  ShowServoValues(true,1);
  }

if(throw_btn_pressed == 1){
  if(plus_btn.justPressed()){
    plus_btn.drawButton(true);
    minus_btn.drawButton(false);
    plus50_btn.drawButton(false);
    minus50_btn.drawButton(false);
    if(Throw[ServoNum]<2000){Throw[ServoNum] = Throw[ServoNum] + 10;}
    ShowServoValues(true,1);
    delay(ButtonPressDelay);
    }
  if(minus_btn.justPressed()){
    plus_btn.drawButton(false);
    minus_btn.drawButton(true);
    plus50_btn.drawButton(false);
    minus50_btn.drawButton(false);
    if(Throw[ServoNum]>1000){Throw[ServoNum] = Throw[ServoNum] - 10;}
    ShowServoValues(true,1);
    delay(ButtonPressDelay);
    }
   if(plus50_btn.justPressed()){
    plus_btn.drawButton(false);
    minus_btn.drawButton(false);
    plus50_btn.drawButton(true);
    minus50_btn.drawButton(false);
    if(Throw[ServoNum]<=1950){Throw[ServoNum] = Throw[ServoNum] + 50;}
    ShowServoValues(true,1);
    delay(ButtonPressDelay);
    }
  if(minus50_btn.justPressed()){
    plus_btn.drawButton(false);
    minus_btn.drawButton(false);
    plus50_btn.drawButton(false);
    minus50_btn.drawButton(true);
    if(Throw[ServoNum]>=1050){Throw[ServoNum] = Throw[ServoNum] - 50;}
    ShowServoValues(true,1);
    delay(ButtonPressDelay);
    }  
  }

if(print_btn.justPressed() && print_btn_pressed == 0){
  print_btn.drawButton(true);
  print_btn_pressed = 1;
  PrintToSerialMonitor();
  }
}
