
#define RotSens1 A0
#define RotSens2 A1
#define LED 3  //the Grove - LED is connected to PWM pin D3 of Arduino
#define ADC_REF 5 //reference voltage of ADC is 5v.If the Vcc switch on the seeeduino
                    //board switches to 3V3, the ADC_REF should be 3.3
#define GROVE_VCC 5 //VCC of the grove interface is normally 5v
#define FULL_ANGLE 300 //full value of the rotary angle is 300 degrees

//#include <cmath>
#include "TM1637.h"
const int CLK = 2;
const int DIO = 3;
TM1637 tm1637(CLK, DIO);

int num;

void setup()
{
    Serial.begin(9600);
    pinMode(RotSens1, INPUT);
    pinMode(RotSens2, INPUT);   
   tm1637.init();
    tm1637.set(BRIGHT_TYPICAL);
}

void loop()
{   
    float voltage1;
    int sensor_value1 = analogRead(RotSens1);
    voltage1 = (float)sensor_value1*ADC_REF/1023;
    float degrees1 = (voltage1*FULL_ANGLE)/GROVE_VCC;

  float voltage2;
    int sensor_value2 = analogRead(RotSens2);
    voltage2 = (float)sensor_value2*ADC_REF/1023;
    float degrees2 = (voltage2*FULL_ANGLE)/GROVE_VCC;  

  /*if (degrees2 == degrees1) {
    Serial.println("match");
    Serial.println(degrees1);
  } else {
    Serial.println("Not matching");
  }*/
  num=floor(degrees1/3.3)*100+floor(degrees2/3.3);

  tm1637.displayNum(10000+num);
  delay(10);

}
