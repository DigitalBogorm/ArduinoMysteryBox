#include "SoftwareSerial.h"

//Et eller andet sted i det her rod, bliver keypad'en defineret som port D2, tror jeg nok. 
#if defined(ARDUINO_ARCH_AVR)
#define SERIAL Serial
SoftwareSerial mySerial(2,3);
#define TRANS_SERIAL  mySerial
#elif defined(ARDUINO_ARCH_SAMD)
#define SERIAL SerialUSB
#define TRANS_SERIAL  Serial
#else

#endif
//Buzzeren skal sættes i A3. Juster denne kode, hvis buzzeren forbindes til en anden port
//VIGTIGT: Buzzeren skal sidde i en analog port, for at den virker ordenligt.
#define buzzer A3

#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

//Farver til LCD-displayet
const int colorR = 0;
const int colorG = 50;
const int colorB = 0;

//En variabel til at afgøre, om keypad'en tager mod inputs
bool read = true;

//Øverste er det aktuelle password, nederste er brugerens input
const int passLength = 4;
int password[passLength] = {1, 2, 3, 4};
int inputKeypad[passLength];
int currentLength = 0;

//RX=2,TX=3(D2) Software Serial Port

void setup() {
 TRANS_SERIAL.begin(9600); 

    SERIAL.begin(9600);  // start serial for output
    SERIAL.println("Version:v1.0");

  //Her sættes buzzeren op, og sættes til at være stille. For ellers begynder den at larme, det mikrosekund den får strøm.
  pinMode(buzzer, OUTPUT);
  noTone(buzzer);

  // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);

    lcd.setRGB(colorR, colorG, colorB);
}

void loop() {
    printData();
}

//Denne funktion læser input, og sammenligner med password
void assign(int value) {

  read = false;

  //Det her ændrer værdien for et punkt i inputpasswordet
  inputKeypad[currentLength] = value;
  //buzzerens lyd fungerer her vha, "tone", fordi alternativet er en helvedes støj, der driver alle i lokalet til vanvid.
  tone(buzzer, 1000);
  delay(100);
  noTone(buzzer);
  lcd.print(value);
  currentLength ++;

  //nedenstående aktiveres først, når brugerens input er samme længde som passwordet. Det er lavet, så det ikke skal ændres, hvis passwordet bliver modificeret.
  if (currentLength == (sizeof(password)/sizeof(int))) {
    read = false;
    delay(1000);
    bool correct = true;
    for (int i : password) {
      //hvis ikke et ciffer af inputtet stemmer overens med det tilsvarende ciffer i passwordet, aktiveres fejlbeskeden.
      if (inputKeypad[i-1] != password[i-1]) {
        SERIAL.println("incorrect");
        lcd.clear();
        lcd.print("incorrect");
        correct = false;
        delay(1000);
        lcd.clear();
        break;
      }
    }
    //hvis ikke der er problemer, aktiveres nedenstående. Indtil videre, er det bare en placeholder-besked.
    if (correct == true) {
    delay(500);
    lcd.clear();
    SERIAL.println("confirmed");
    lcd.print("confirmed");
    delay(1000);
    lcd.clear();
    }
  currentLength = 0;  
  }
  //delay(500);

read = true;
}


/*
* data mapping:E1---1；E2---2；E3---3；E4---4；E5---5；E6---6；
*              E7---7；E8---8；E9---9；EA---*；EB---0；EC---#；
*/
void printData() {
    while(read == true) {
        uint8_t data = TRANS_SERIAL.read();
        switch(data) {
                case 0xE1 :
                    SERIAL.println("1");
                    assign(1);
                    break;
                case 0xE2 :
                    SERIAL.println("2");
                    assign(2);
                    break;
                case 0xE3 :
                    SERIAL.println("3");
                    assign(3);
                    break;
                case 0xE4 :
                    SERIAL.println("4");
                    assign(4);
                    break;
                case 0xE5 :
                    SERIAL.println("5");
                    assign(5);
                    break;
                case 0xE6 :
                    SERIAL.println("6");
                    assign(6);
                    break;
                case 0xE7 :
                    SERIAL.println("7");
                    assign(7);
                    break;
                case 0xE8 :
                    SERIAL.println("8");
                    assign(8);
                    break;
                case 0xE9 :
                    SERIAL.println("9");
                    assign(9);
                    break;
                case 0xEA :
                    SERIAL.println("*");
                    break;
                case 0xEB :
                    SERIAL.println("0");
                    break;
                case 0xEC :
                    SERIAL.println("#");
                    break;
                default:
                    break;
            }
    }

}
