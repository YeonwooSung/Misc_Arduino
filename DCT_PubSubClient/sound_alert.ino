/**
 * Codes for buzzer outputs.
 * PWM is used for controlling the buzzer sounds.
 * 
 * reference links for pwm:
 *    <https://arsviator.blogspot.com/2018/11/esp32-pwm.html>
 *    <https://answerofgod.tistory.com/842>
 *    <https://postpop.tistory.com/63>
 * 
 * @author Yeonwoo Sung
 */
#include "pitches.h"

#define BUZZER_PIN        20
#define BUZZER_FREQ       1E5 //600
#define BUZZER_RESOLUTION 12  //10
#define BUZZER_CHANNEL    0


//---------------------------------------------------

uint8_t melody_note_mario [] = {
  NOTE_E3, NOTE_E3, 0, NOTE_E3, 
  0, NOTE_C3, NOTE_E3, 0, 
  NOTE_G3, 0, 0, 0, 
  NOTE_G2, 0, 0, 0, 
  NOTE_C3, 0, 0, NOTE_G2, 
  0, 0, NOTE_E2, 0, 
  0, NOTE_A2, 0, NOTE_B2, 
  0, NOTE_AS2, NOTE_A2, 0, 
  NOTE_G2, NOTE_E3, NOTE_G3, NOTE_A3, 
  0, NOTE_F3, NOTE_G3, 0, 
  NOTE_E3, 0, NOTE_C3, NOTE_D3, 
  NOTE_B2, 0, 0, NOTE_C3, 
  0, 0, NOTE_G2, 0, 
  0, NOTE_E2, 0, 0, 
  NOTE_A2, 0, NOTE_B2, 0, 
  NOTE_AS2, NOTE_A2, 0, NOTE_G2, 
  NOTE_E3, NOTE_G3, NOTE_A3, 0, 
  NOTE_F3, NOTE_G3, 0, NOTE_E3, 
  0, NOTE_C3, NOTE_D3, NOTE_B2, 0, 0
};

uint8_t noteDurations_mario [] = {
  12, 12, 12, 12, 
  12, 12, 12, 12, 
  12, 12, 12, 12, 
  12, 12, 12, 12, 
  12, 12, 12, 12, 
  12, 12, 12, 12, 
  12, 12, 12, 12, 
  12, 12, 12, 12, 
  9, 9, 9, 12, 
  12, 12, 12, 12, 
  12, 12, 12, 12, 
  12, 12, 12, 12, 
  12, 12, 12, 12, 
  12, 12, 12, 12, 
  12, 12, 12, 12, 
  12, 12, 12, 9, 
  9, 9, 12, 12, 
  12, 12, 12, 12, 
  12, 12, 12, 12, 
  12, 12,
};

uint8_t melody_note_hello [] = {
    NOTE_G2, NOTE_C3, NOTE_E3, NOTE_G3, 0
};

uint8_t noteDurations_hello [] = {
    8, 8, 8, 8, 12
};

uint8_t melody_note_error [] = {
    NOTE_F3, NOTE_F3, 0, NOTE_F3, NOTE_F3, 0
};

uint8_t noteDurations_error [] = {
    9, 9, 12, 9, 9, 12
};

//---------------------------------------------------


void setup_sound_alert() {
    pinMode(BUZZER_PIN, OUTPUT);
    ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RESOLUTION);
    ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
}

void buzzer_error() {
    int sizeofArr = sizeof(melody_note_error);
    unsigned long int beepTime = 0;
    int melody_num = 0;

    while (melody_num < sizeofArr) {
        if (millis() - beepTime >= 2000 / noteDurations_error[melody_num]) {
            beepTime = millis();

            ledcWrite(BUZZER_CHANNEL, 0);
            ledcWriteTone(BUZZER_CHANNEL, melody_note_error[melody_num]);

            melody_num += 1;

            if (melody_num == sizeofArr) {
                ledcWrite(BUZZER_CHANNEL, 0); // noTone() - ledcWrite(uint8_t channel, uint32_t duty);
                //melody_num = 0;
                beepTime = 0;
                break;
            }
        }
    }
}

void buzzer_hello_world() {
    int sizeofArr = sizeof(melody_note_hello);
    unsigned long int beepTime = 0;
    int melody_num = 0;

    while (melody_num < sizeofArr) {
        if (millis() - beepTime >= 2000 / noteDurations_hello[melody_num]) {
            beepTime = millis();

            ledcWrite(BUZZER_CHANNEL, 0);
            ledcWriteTone(BUZZER_CHANNEL, melody_note_hello[melody_num]);
            ledcWrite(BUZZER_CHANNEL, 250);

            melody_num += 1;

            if (melody_num == sizeofArr) {
                ledcWrite(BUZZER_CHANNEL, 0);
                //melody_num = 0;
                beepTime = 0;
                break;
            }
        }
    }
}

void buzzer_supermario() {
  int sizeofArr = sizeof(melody_note_mario);
  unsigned long int beepTime = 0;
  int melody_num = 0;

  while (melody_num <= sizeofArr) {
      if (millis() - beepTime >= 2000 / noteDurations_mario[melody_num]) {
          beepTime = millis();

          ledcWrite(BUZZER_CHANNEL, 0);
          ledcWriteTone(BUZZER_CHANNEL, melody_note_mario[melody_num]);

          melody_num += 1;

          if (melody_num == sizeofArr) {
              ledcWrite(BUZZER_CHANNEL, 0);
              beepTime = 0;
              break;
          }
      }
  }
}
