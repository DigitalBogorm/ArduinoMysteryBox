#include "SoftwareSerial.h"

#if defined(ARDUINO_ARCH_AVR)
#define SERIAL Serial
SoftwareSerial mySerial(2,3);
#define TRANS_SERIAL  mySerial
#elif defined(ARDUINO_ARCH_SAMD)
#define SERIAL SerialUSB
#define TRANS_SERIAL  Serial
#else

#endif

//Øverste er det aktuelle password, nederste er brugerens input
int password[4] = {1, 2, 3, 4};
int inputKeypad[4];
int currentLength = 0;

//RX=2,TX=3(D2) Software Serial Port

void setup() {
 TRANS_SERIAL.begin(9600); 

    SERIAL.begin(9600);  // start serial for output
    SERIAL.println("Version:v1.0");


}

void loop() {
    printData();
}

//Denne funktion læser input, og sammenligner med password
void assign(int value) {
  //Det her ændrer værdien for et punkt i inputpasswordet
  inputKeypad[currentLength] = value;
  currentLength ++;

  //nedenstående aktiveres først, når brugerens input er samme længde som passwordet. Det er lavet, så det ikke skal ændres, hvis passwordet bliver modificeret.
  if (currentLength == (sizeof(password)/sizeof(int))) {
    delay(1000);
    bool correct = true;
    for (int i : password) {
      //hvis ikke et ciffer af inputtet stemmer overens med det tilsvarende ciffer i passwordet, aktiveres fejlbeskeden.
      if (inputKeypad[i-1] != password[i-1]) {
        SERIAL.println("incorrect, ya dumb fuck");
        correct = false;
        delay(1000);
        break;
      }
    }
    //hvis ikke der er problemer, aktiveres nedenstående. Indtil videre, er det bare en placeholder-besked.
    if (correct == true) {
    SERIAL.println("confirmed");
    }
  currentLength = 0;  
  }
}


/*
* data mapping:E1---1；E2---2；E3---3；E4---4；E5---5；E6---6；
*              E7---7；E8---8；E9---9；EA---*；EB---0；EC---#；
*/
void printData() {
    while(TRANS_SERIAL.available()) {
      //kort delay, for at undgå spam (både med og uden overlæg)
      delay(10);
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
