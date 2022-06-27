// Arduino DTMF Decoder

#include <DTMF.h> // http://forum.arduino.cc/index.php/topic,121540.0.html https://yadi.sk/d/QwCqk-DOhS6nn

// #define PTT  7
int sensorPin = A0;

float n = 128.0;

float sampling_rate = 8926.0;

DTMF dtmf = DTMF(n, sampling_rate);

float d_mags[8];

char thischar;

int ledPins[] = { // ?????? ??? 10 PINS / ????.

2, 3, 5, 6, 7, 8, 9, 10, 11, 12 // 4-Pin, ???????????? ???????????!

};

void setup() {

// pinMode(PTT, OUTPUT);
// digitalWrite(PTT, HIGH);
Serial.begin(9600);

for (int i = 0; i <= 9; i++)pinMode(ledPins[i], OUTPUT); // Set all ledPins to OUTPUT.

}

void loop() {

dtmf.sample(sensorPin);

dtmf.detect(d_mags, 506);

thischar = dtmf.button(d_mags, 1800.);

if (thischar) {

Serial.println(thischar);

if (thischar - 48 == -13)for (int i = 0; i <= 9; i++)digitalWrite(ledPins[i], LOW); // #

else if (thischar - 48 == -6)for (int i = 0; i <= 9; i++)digitalWrite(ledPins[i], HIGH); // *

else

digitalWrite(ledPins[thischar - 48], !digitalRead(ledPins[thischar - 48])), delay(200);

}

}
