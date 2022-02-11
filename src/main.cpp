
#include <printf.h>
#include <RF24_config.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "..\..\joystick\src\joystick_str.h"

#define dataRate RF24_2MBPS //could be lower for more distance
#define paLevel RF24_PA_MIN // could be Lower as well

#define pin_MFL_E 9
#define pin_MFL_B1 18
#define pin_MFL_B2 19

#define pin_MFR_E 6
#define pin_MFR_B1 20
#define pin_MFR_B2 21


#define pin_MBR_E 5
#define pin_MBR_B1 14
#define pin_MBR_B2 15

#define pin_MBL_E 3
#define pin_MBL_B1 16
#define pin_MBL_B2 17

#define pin_latch 4
#define pin_clock 2
#define pin_data 7

#define pin_CE 8
#define pin_CSN 10

#define max_speed 150
#define min_speed 50

RF24 radio(pin_CE, pin_CSN); // CE, CSN

void move_wheel(int speed, pin_size_t pin_f, pin_size_t pin_b, pin_size_t pin_s)
{
  int wheel_rotation_speed;
  if (speed < 0)
  {
    wheel_rotation_speed = -1 * speed;
    digitalWrite(pin_f, LOW);
    digitalWrite(pin_b, HIGH);
  }
  else
  {
    wheel_rotation_speed = speed;
    digitalWrite(pin_f, HIGH);
    digitalWrite(pin_b, LOW);
  }
  if (wheel_rotation_speed < min_speed)
  {
    wheel_rotation_speed = 0;
  }
  analogWrite(pin_s, wheel_rotation_speed);
}

void setup()
{
  Serial.begin(9600);
  pinMode(pin_MBL_E, OUTPUT);
  pinMode(pin_MBL_B1, OUTPUT);
  pinMode(pin_MBL_B2, OUTPUT);

  pinMode(pin_MBR_E, OUTPUT);
  pinMode(pin_MBR_B1, OUTPUT);
  pinMode(pin_MBR_B2, OUTPUT);

  pinMode(pin_MFL_E, OUTPUT);
  pinMode(pin_MFL_B1, OUTPUT);
  pinMode(pin_MFL_B2, OUTPUT);

  pinMode(pin_MFR_E, OUTPUT);
  pinMode(pin_MFR_B1, OUTPUT);
  pinMode(pin_MFR_B2, OUTPUT);

  move_wheel(0, pin_MFL_B1, pin_MFL_B2, pin_MFL_E);
  move_wheel(0, pin_MFR_B1, pin_MFR_B2, pin_MFR_E);
  move_wheel(0, pin_MBL_B1, pin_MBL_B2, pin_MBL_E);
  move_wheel(0, pin_MBR_B1, pin_MBR_B2, pin_MBR_E);


pinMode(pin_latch, OUTPUT);
pinMode(pin_clock, OUTPUT);
pinMode(pin_data, OUTPUT);
  /*

  pinMode(pin_led_B,OUTPUT);
  pinMode(pin_led_F,OUTPUT);
digitalWrite(pin_led_B,HIGH);
digitalWrite(pin_led_F,HIGH);

*/
  Serial.begin(9600);
  printf_begin();
  radio.begin();

  radio.setRetries(15, 15);
  radio.setPayloadSize(payload_size);
  radio.setDataRate(dataRate);
  radio.setPALevel(paLevel);

  radio.openReadingPipe(0, address);
  radio.startListening();
  radio.printDetails();

}

void car_move(struct joystick i_joystick)
{



long y = -map(i_joystick.JL_X, 0, 1024, -max_speed, max_speed);
long x = -map(i_joystick.JL_Y, 0, 1024, -max_speed, max_speed);

int speed = (int) sqrt((x*x) + (y*y));

if (speed>max_speed){speed = max_speed;}
if (speed>10){speed = map(speed,0, max_speed,min_speed, max_speed); } else{speed = 0;}


speed = (-1)*speed;

double alfa = atan2(y,x);

double BL_FR;
double BR_FL;

if      (alfa >= 0 && alfa < PI/2 ){
  BL_FR = 1;
  BR_FL = sin((2*alfa)-(PI/2));
}else if (alfa >= PI/2 && alfa < PI ){
  BL_FR = sin((2*alfa)-(PI/2));
  BR_FL = 1;
}else if (alfa >= -PI/2 && alfa < 0 ){
  BL_FR = -sin((2*alfa)-(PI/2));
  BR_FL = -1;
}else if (alfa >= -PI && alfa < PI/2 ){
  BL_FR = -1;
  BR_FL = -sin((2*alfa)-(PI/2));
}
  
  move_wheel(speed*BR_FL, pin_MFL_B1, pin_MFL_B2, pin_MFL_E);
  move_wheel(speed*BR_FL, pin_MBR_B1, pin_MBR_B2, pin_MBR_E);


  move_wheel(speed*BL_FR, pin_MBL_B1, pin_MBL_B2, pin_MBL_E);
  move_wheel(speed*BL_FR, pin_MFR_B1, pin_MFR_B2, pin_MFR_E);





}

void light(int beep)
{
  int numberToDisplay;
 
 numberToDisplay = 32+16+8+4;
 if (beep == 0 ){
 numberToDisplay += 2;
 }else{
   numberToDisplay += 0;
 }

digitalWrite(pin_latch, LOW);
// shift out the bits:
shiftOut(pin_data, pin_clock, MSBFIRST, numberToDisplay);
//take the latch pin high so the LEDs will light up:
digitalWrite(pin_latch, HIGH);
}

void loop()
{
  if (radio.available())
  {
    struct joystick l_joystick;
    radio.read(&l_joystick, sizeof(l_joystick));

        char str[100];
    sprintf(str, "Joystic left:  X: %i, Y: %i, B: %i  ", l_joystick.JL_X, l_joystick.JL_Y, l_joystick.JL_B);
    Serial.print(str);
    sprintf(str, "Joystic right: X: %i, Y: %i, B: %i", l_joystick.JR_X, l_joystick.JR_Y, l_joystick.JR_B);
    Serial.println(str);

    light(l_joystick.JR_B);
    car_move(l_joystick);
  }
  else{
    light(1);
  }
}
