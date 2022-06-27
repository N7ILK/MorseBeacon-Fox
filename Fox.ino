#include <DTMF.h>
#include <EEPROM.h>
#include "pitches.h"

#define PTT_PIN 7
#define RADIO_MIC 6
#define RADIO_SPK A0
#define TONE_FREQ 700  // CW pitch
#define PTT_DELAY 1000 // PTT delay before xmit (in milliseconds)
#define PTT_ON LOW
#define PTT_OFF HIGH
#define INTERVAL 80 //Interval 80 is timing fo CW. Lower values = higher WPM
#define DOT 1 * INTERVAL
#define CHAR_SPACE 2 * INTERVAL
#define DASH 3 * INTERVAL
#define WORD_SPACE 4 * INTERVAL
int audioInput = RADIO_SPK, pause = 660, setCall = 0, callBufferCount;
//Pause 660 = apx 30 second delay between transmissions. Can be set with DTMF
bool sendMelody = true, xmit = false;
char thischar, callSign[16], callBuffer[32];
float sampling_rate = 8926.0, d_mags[8], n = 128.0; // From DTMF example program
DTMF dtmf = DTMF(n, sampling_rate);

int melody[] = { // Star Wars Imperial March
  NOTE_A4, NOTE_A4, NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4,
  NOTE_E5, NOTE_E5, NOTE_E5, NOTE_F5, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4
};

int noteDurations[] = {
  4, 4, 4, 5, 16, 4, 5, 16, 2, 4, 4, 4, 5, 16, 4, 5, 16, 2
};

String morseCode[] = { // in ACSII order
  "-..-.", // forward slash
  "-----", // 0
  ".----", // 1
  "..---", // 2
  "...--", // 3
  "....-", // 4
  ".....", // 5
  "-....", // 6
  "--...", // 7
  "---..", // 8
  "----.", // 9
  "---...", // :
  "-.-.-.", // ;
  "-.--.", // <
  "-...-", // =
  "-.--.-", // >
  "..--..", // ?
  " ", // @ Used as space character
  ".-", // A
  "-...", // B
  "-.-.", // C
  "-..", // D
  ".", // E
  "..-.", // F
  "--.", // G
  "....", // H
  "..", // I
  ".---", // J
  "-.-", // K
  ".-..", // L
  "--", // M
  "-.", // N
  "---",  // O
  ".--.", // P
  "--.-", // Q
  ".-.", // R
  "...",  // S
  "-", // T
  "..-", // U
  "...-", // V
  ".--", // W
  "-..-", // X
  "-.--", // Y
  "--..", // Z
  " "   //space character
};

void setup() {
  pinMode(PTT_PIN, OUTPUT);
  pinMode(RADIO_MIC, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  //Serial.begin(9600); //Only needed if we're debugging
  PTT(PTT_ON);
  sendLetter('H');
  sendLetter('I');
  PTT(PTT_OFF);
  if (EEPROM.read(0) == 'Y') // Valid call sign in EEPROM
    EEPROM.get(1, callSign); // Copy call sign from EEPROM
  xmit = false; // Transmit off initially
}

void loop() {
  for (int x = 0; x < pause; x ++)  // while waiting between transmissions, look for DTMF
  {
    dtmf.sample(audioInput); // From DTMF example program
    dtmf.detect(d_mags, 506);
    thischar = dtmf.button(d_mags, 1800.);

    if (thischar) {  // decide what to do if DTMF tone is received
      digitalWrite(LED_BUILTIN, HIGH); // Feedback that valid DTMF received
      delay(250);
      digitalWrite(LED_BUILTIN, LOW);
      if (setCall == 1) // In mode to capture digits of call sign
      {
        if (thischar == '#') // End of call sign input
        {
          int i, j;
          setCall = 0;
          if (callBufferCount % 2 == 1) // Odd number of digits received. Warn and bail out
          {
            PTT(PTT_ON);
            sendLetter('0');
            PTT(PTT_OFF);
            return;
          }
          memset(callSign, 0, sizeof(callSign));
          for (i = 0, j = 0; i < callBufferCount; i += 2)
            callSign[j++] = callBuffer[i] * 10 + callBuffer[i + 1];
          callSign[j] = '\0';
          EEPROM.write(0, 'Y');// EEPROM contains valid call sign
          EEPROM.put(1, callSign); // Store call sign into EEPROM
          // Send ack call sign set
          PTT(PTT_ON);
          sendLetter('D'); sendLetter('E'); sendLetter('@');
          for (i = 0; i < strlen(callSign); i++)
            sendLetter(callSign[i]);
          PTT(PTT_OFF);
        }
        else
          callBuffer[callBufferCount++] = thischar - '0'; // Any digit other than #, append to buffer
      }
      else // Normal operating mode
      {
        switch (thischar) {
          case '0':  // Turn off transmissions
            xmit = false;  // set the flag to disable transmissions
            break;
          case '*': // Start entering call sign
            setCall = 1;
            callBufferCount = 0;
            break;
          case '#': //Toggle melody
            sendMelody = !sendMelody;
            break;
          default: // Enable and set delay * 10 seconds
            pause = (thischar - '0') * 220; //Set delay
            xmit = true; //Enable xmit
            x = pause - 22; // Expire loop timer in one second
            break;
        }
      }
    }
    delay(1);
  } // If valid DTMF received

  if (xmit && callSign[0] != '\0') // Must have valid call sign set to xmit
  {
    PTT(PTT_ON);
    //Play the melody if enabled
    if (sendMelody)
    {
      for (int thisNote = 0; thisNote < sizeof(melody) / sizeof(int); thisNote++)
      {
        int noteDuration = 1000 / noteDurations[thisNote];
        tone(RADIO_MIC, melody[thisNote], noteDuration);
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        noTone(RADIO_MIC);
      }
      delay(1000);  //one second between melody and beaconstring
    }
    // Send ident
    sendLetter('C'); sendLetter('Q'); sendLetter('@');
    for (int i = 0; i < strlen(callSign); i++)
      sendLetter(callSign[i]);
    sendLetter('/'); sendLetter('F'); sendLetter('O'); sendLetter('X');
    delay(500);
    PTT(PTT_OFF);
  } // If xmit
}

void PTT(int onOff)
{
  digitalWrite(PTT_PIN, onOff);
  if (onOff == PTT_ON)
    delay(PTT_DELAY); // delay X seconds after PTT to allow xmit to stabilize
}

void sendLetter (char c) {
  sendMorseCode(morseCode[c - '/']);
}

void sendMorseCode (String tokens) {
  int i;
  for (i = 0; i < tokens.length(); ++i) {
    switch (tokens[i]) {
      case '-':
        sendDotDash(DASH);
        break;
      case '.':
        sendDotDash(DOT);
        break;
      case ' ':
        delay(WORD_SPACE); // pause between words
        break;
    }
  }
  delay(CHAR_SPACE); // pause between characters
}

void sendDotDash(int len) {
  digitalWrite(LED_BUILTIN, HIGH);
  tone(RADIO_MIC, TONE_FREQ, len);
  delay(len);
  noTone(RADIO_MIC);
  digitalWrite(LED_BUILTIN, LOW);  
  delay(DOT);
}
