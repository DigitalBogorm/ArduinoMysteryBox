//Her opsættes rotary encoders
#define RotSens1 A0
#define RotSens2 A1
//Ved ikke hvad det gør, men koden kan ikke lide, at man fjerner det.
#define ADC_REF 5      //reference voltage of ADC is 5v.If the Vcc switch on the seeeduino \
                        //board switches to 3V3, the ADC_REF should be 3.3
#define GROVE_VCC 5    //VCC of the grove interface is normally 5v
#define FULL_ANGLE 99  //full value of the rotary angle is 300 degrees
//Her opsættes 7-segment-display
#include "TM1637.h"
//Porte til 4-digit-display defineres her. juster efter behov.
const int CLK = 3;
const int DIO = 4;
TM1637 tm1637(CLK, DIO);
//Bruges til Rotary Encoders + 4-digit-display.
int num;
//Ved ikke hvad det her er, men det virker vigtigt.
#include "SoftwareSerial.h"

#define LED 4

//Et eller andet sted i det her rod, bliver keypad'en defineret som port D2, tror jeg nok.
#if defined(ARDUINO_ARCH_AVR)
#define SERIAL Serial
SoftwareSerial mySerial(2, 3);
#define TRANS_SERIAL mySerial
#elif defined(ARDUINO_ARCH_SAMD)
#define SERIAL SerialUSB
#define TRANS_SERIAL Serial
#else

#endif
//Buzzeren skal sættes i A3. Juster denne kode, hvis buzzeren forbindes til en anden port
//VIGTIGT: Buzzeren skal sidde i en analog port, for at den virker ordenligt.
#define buzzer A3
//Bruges til LCD'en
#include <Wire.h>
#include "rgb_lcd.h"
//Sætter LCD'en til at hedde "LCD" i koden.
rgb_lcd lcd;

//Farver til LCD-displayet
const int colorR = 0;
const int colorG = 50;
const int colorB = 0;
//En variabel til at afgøre, om keypad'en tager mod inputs (Test-stadie, do not touch)
bool read = true;

//Password-længder. Hvis passwordet ændres, skal den øverste også justeres. Den nederste må gerne være i fred.
const int passLength = 4;
int currentLength = 0;
//Øverste er det aktuelle password, nederste er brugerens input
int password[passLength] = {6, 8, 4, 4};
int inputKeypad[passLength];



void setup() {
  //Kører "serial", som er der keypad'en outputter til.
  TRANS_SERIAL.begin(9600);

  //Fastsætter LED'en
  pinMode(LED, OUTPUT);

  SERIAL.begin(9600);  // start serial for output
  SERIAL.println("Version:v1.0");

  //Her sættes buzzeren op, og sættes til at være stille. For ellers begynder den at larme, det mikrosekund den får strøm.
  pinMode(buzzer, OUTPUT);
  noTone(buzzer);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  lcd.setRGB(colorR, colorG, colorB);

  digitalWrite(LED, HIGH);

  //Rotations-sensorerne er sat til at være input
  pinMode(RotSens1, INPUT);
  pinMode(RotSens2, INPUT);
  //Tænder for 7-segment-display
  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);
  lcd.print("Online");
  delay(2000);
  lcd.clear();
}

void loop() {
  //Kører den funktion, der får keypad'en til at virke. Do not delete
  printData();
  //Måler vinklen på den ene rotations-sensor (Den i A0)
  float voltage1;
  int sensor_value1 = analogRead(RotSens1);
  voltage1 = (float)sensor_value1 * ADC_REF / 1023;
  float degrees1 = (voltage1 * FULL_ANGLE) / GROVE_VCC;
  //Her måles vinklen på sensoren i A1
  float voltage2;
  int sensor_value2 = analogRead(RotSens2);
  voltage2 = (float)sensor_value2 * ADC_REF / 1023;
  float degrees2 = (voltage2 * FULL_ANGLE) / GROVE_VCC;
  //Num danner et tal, baseret på inputtet fra rotations-sensorer
  num = round(degrees1) * 100 + round(degrees2);
  //4-digit-display viser et tal, baseret på num (+10000-delen, er for at den viser '0000', hvis alt er på 0)
  tm1637.displayNum(10000 + num);
  if (num == 2456) {
    //SERIAL.println("TestSuccess");
    //digitalWrite(LED, HIGH);
    tone(buzzer, 1000);
  } else {
    //SERIAL.println("TestFailure");
    //digitalWrite(LED, LOW);
    noTone(buzzer);
  }
}

//Denne funktion sørger for at læse keypad'ets input, og tjekker om koden stemmer overens
void assign(int value) {

  read = false;

  //Det her ændrer værdien for et punkt i inputpasswordet
  inputKeypad[currentLength] = value;
  //buzzerens lyd fungerer her vha, "tone", fordi alternativet er en helvedes støj, der driver alle i lokalet til vanvid.
  /*tone(buzzer, 1000);
  delay(100);
  noTone(buzzer);*/
  lcd.print(value);
  currentLength++;

  //nedenstående aktiveres først, når brugerens input er samme længde som passwordet. Det er lavet, så det ikke skal ændres, hvis passwordet bliver modificeret.
  if (currentLength == (sizeof(password) / sizeof(int))) {
    read = false;
    delay(1000);
    bool correct = true;
    for (int i = 0; i < passLength; i++) {
      //hvis ikke et ciffer af inputtet stemmer overens med det tilsvarende ciffer i passwordet, aktiveres fejlbeskeden.
      if (inputKeypad[i] != password[i]) {
        SERIAL.println("incorrect");
        lcd.clear();
        colouring(255, 0, colorB);
        lcd.print("incorrect");
        correct = false;
        delay(1000);
        colouring(colorR, colorG, colorB);
        lcd.clear();
        break;
      }
    }
    //hvis ikke der er problemer, aktiveres nedenstående. Indtil videre, er det bare en placeholder-besked.
    if (correct == true) {
      delay(250);
      lcd.clear();
      SERIAL.println("confirmed");
      colouring(colorR, 255, colorB);
      lcd.print("2456");
      delay(3000);
      colouring(colorR, colorG, colorB);
      lcd.clear();
    }
    currentLength = 0;
  }
  //delay(500);

  read = true;
}
//Læser inputtet fra keypad
void printData() {

  uint8_t data = TRANS_SERIAL.read();
  //SERIAL.println(data);
  switch (data) {
    case 0xE1:
      SERIAL.println("1");
      assign(1);
      break;
    case 0xE2:
      SERIAL.println("2");
      assign(2);
      break;
    case 0xE3:
      SERIAL.println("3");
      assign(3);
      break;
    case 0xE4:
      SERIAL.println("4");
      assign(4);
      break;
    case 0xE5:
      SERIAL.println("5");
      assign(5);
      break;
    case 0xE6:
      SERIAL.println("6");
      assign(6);
      break;
    case 0xE7:
      SERIAL.println("7");
      assign(7);
      break;
    case 0xE8:
      SERIAL.println("8");
      assign(8);
      break;
    case 0xE9:
      SERIAL.println("9");
      assign(9);
      break;
    case 0xEA:
      SERIAL.println("*");
      break;
    case 0xEB:
      SERIAL.println("0");
      assign(0);
      break;
    case 0xEC:
      SERIAL.println("#");
      break;
    default:
      //SERIAL.println("L + ratio");
      break;
  }
}

void colouring(int R, int G, int B) {
  lcd.setRGB(R, G, B);
}
