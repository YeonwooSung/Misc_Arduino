/*
 * Codes for LED and Button control.
 *
 * @author Yeonwoo Sung
 */
#include "pin_nums.h"

//----------------------------------------------
//  Function prototypes
//----------------------------------------------

// sound_alert
void buzzer_supermario();
void buzzer_hello_world();

//display
void init_lcd();

//----------------------------------------------

#define LED_QUEUE_LEN 7
#define LED_LIFETIME 10000


typedef struct ledCtrl {
    bool led_on;
    const int led1_pin_num;
    const int led2_pin_num;
    long millisec;
} LED_t;

LED_t queue[LED_QUEUE_LEN] = {
    { false, LED_PIN_1_1, LED_PIN_1_2, 0 },  // LED 1
    { false, LED_PIN_2_1, LED_PIN_2_2, 0 },  // LED 2
    { false, LED_PIN_3_1, LED_PIN_3_2, 0 },  // LED 3
    { false, LED_PIN_4_1, LED_PIN_4_2, 0 },  // LED 4
    { false, LED_PIN_5_1, LED_PIN_5_2, 0 },  // LED 5
    { false, LED_PIN_6_1, LED_PIN_6_2, 0 },  // LED 6
    { false, LED_PIN_7_1, LED_PIN_7_2, 0 },  // LED 7
};


static volatile bool light_on = false;
static volatile bool buttonPressed = false;
static int buttonNum = false;

//----------------------------------------------
// setters and getters
//----------------------------------------------

bool isLightOn() {
    return light_on;
}

bool isButtonPressed() {
    return buttonPressed;
}

void setButtonPressed(bool b) {
    buttonPressed = b;
}

void setButtonNum(int i) {
    buttonNum = i;
}

int getButtonNum() {
    return buttonNum;
}

// interrupt handler

void IRAM_ATTR handleInterrupt_button1() {
    Serial.println("button 1 pressed");
    setButtonPressed(true);
    setButtonNum(1);
}

void IRAM_ATTR handleInterrupt_button2() {
    Serial.println("button 2 pressed");
    setButtonPressed(true);
    setButtonNum(2);
}

void IRAM_ATTR handleInterrupt_button3() {
    setButtonPressed(true);
    setButtonNum(3);
    Serial.println("button 3 pressed");
}

void IRAM_ATTR handleInterrupt_button4() {
    setButtonPressed(true);
    setButtonNum(4);
}

void IRAM_ATTR handleInterrupt_button5() {
    setButtonPressed(true);
    setButtonNum(5);
}

void IRAM_ATTR handleInterrupt_button6() {
    setButtonPressed(true);
    setButtonNum(6);
}

void IRAM_ATTR handleInterrupt_button7() {
    setButtonPressed(true);
    setButtonNum(7);
}

//----------------------------------------------
// functions
//----------------------------------------------

void setup_led_button_ctrl() {
    // setup for led pins
    pinMode (LED_PIN_1_1, OUTPUT);
    pinMode (LED_PIN_1_2, OUTPUT);
    pinMode (LED_PIN_2_1, OUTPUT);
    pinMode (LED_PIN_2_2, OUTPUT);
    pinMode (LED_PIN_3_1, OUTPUT);
    pinMode (LED_PIN_3_2, OUTPUT);
    pinMode (LED_PIN_4_1, OUTPUT);
    pinMode (LED_PIN_4_2, OUTPUT);
    pinMode (LED_PIN_5_1, OUTPUT);
    pinMode (LED_PIN_5_2, OUTPUT);
    pinMode (LED_PIN_6_1, OUTPUT);
    pinMode (LED_PIN_6_2, OUTPUT);
    pinMode (LED_PIN_7_1, OUTPUT);
    pinMode (LED_PIN_7_2, OUTPUT);

    // setup for button pins
    pinMode(BUTTON_PIN_1, INPUT_PULLUP);
    pinMode(BUTTON_PIN_2, INPUT);
    pinMode(BUTTON_PIN_3, INPUT);
    pinMode(BUTTON_PIN_4, INPUT);
    pinMode(BUTTON_PIN_5, INPUT);
    pinMode(BUTTON_PIN_6, INPUT_PULLUP);
    pinMode(BUTTON_PIN_7, INPUT_PULLUP);

    // attach interrupt to detect button_pressed event
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_1), handleInterrupt_button1, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_2), handleInterrupt_button2, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_3), handleInterrupt_button3, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_4), handleInterrupt_button4, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_5), handleInterrupt_button5, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_6), handleInterrupt_button6, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_7), handleInterrupt_button7, FALLING);
}

void turn_on_led(int recv_msg_num) {
    int index;
    switch (recv_msg_num) {
        case 0:
            Serial.println("recv_msg_num=0");
        case 1:
            index = 0;
            break;
        case 2:
            index = 1;
            break;
        case 3:
            index = 2;
            break;
        case 4:
            index = 3;
            break;
        case 5:
            index = 4;
            break;
        case 6:
            index = 5;
            break;
        case 7:
            index = 6;
            break;
        default:
            Serial.print("Invalid recv_msg_num: ");
            Serial.println(recv_msg_num);
            return;
    }

    // alert
    buzzer_hello_world();

    // turn on the corresponding LEDs
    set_led_as_turned_on(index);

    delay(1000);
}

void set_led_as_turned_on(int led_index) {
    Serial.print("[DEBUG] led_index = ");
    Serial.println(led_index);
    // turn the light on
    digitalWrite(queue[led_index].led1_pin_num, HIGH);
    digitalWrite(queue[led_index].led2_pin_num, HIGH);

    // set up the attributes with suitable values
    queue[led_index].led_on = true;
    queue[led_index].millisec = millis();

    // change the flag value
    light_on = true;
}

void check_led_lifetime() {
    bool checker = false;

    // iterate a list of LEDs
    for (int i = 0; i < LED_QUEUE_LEN; i++) {
        if (queue[i].led_on) {
            long millisec = queue[i].millisec;
            if ((millis() - millisec) >= LED_LIFETIME) {
                // turn off the corresponding leds
                digitalWrite(queue[i].led1_pin_num, LOW);
                digitalWrite(queue[i].led2_pin_num, LOW);

                queue[i].led_on = false;

                Serial.print("[LOG] Turn off the LED num ");
                Serial.println((i + 1));

                // init LCD if the led index is either 5 or 6
                // don't forget that the index starts from 0
                if (i > 4) {
                    init_lcd();
                }
            }
        }

        checker = checker | queue[i].led_on;
    }

    light_on = checker;
}
