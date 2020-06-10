/*
 * Codes for LED and Button control.
 *
 * @author Yeonwoo Sung
 */

//----------------------------------------------
//  Function prototypes
//----------------------------------------------

// sound_alert
void buzzer_supermario();
void buzzer_hello_world();

//----------------------------------------------

#define LED_QUEUE_LEN 7
#define LED_LIFETIME 30000


/*
 * 34, 35, 36, and 39 are INPUT ONLY pins!
 * 
 * For more information, please refer to the following web page: 
 *    url: <https://randomnerdtutorials.com/esp32-pinout-reference-gpios/>
 */
// LED Pin numbers
#define LED_PIN_1_1 25
#define LED_PIN_1_2 26

// Button Pin numbers
#define BUTTON_PIN_1 32


typedef struct ledCtrl {
    bool led_on;
    const int led1_pin_num;
    const int led2_pin_num;
    long millisec;
} LED_t;

LED_t queue[LED_QUEUE_LEN] = {
    { false, LED_PIN_1_1, LED_PIN_1_2, 0 },  // LED 1
    { false, LED_PIN_1_1, LED_PIN_1_2, 0 },  // LED 2
    { false, LED_PIN_1_1, LED_PIN_1_2, 0 },  // LED 3
    { false, LED_PIN_1_1, LED_PIN_1_2, 0 },  // LED 4
    { false, LED_PIN_1_1, LED_PIN_1_2, 0 },  // LED 5
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
    setButtonPressed(true);
    setButtonNum(1);
}

void IRAM_ATTR handleInterrupt_button2() {
    setButtonPressed(true);
    setButtonNum(2);
}

void IRAM_ATTR handleInterrupt_button3() {
    setButtonPressed(true);
    setButtonNum(3);
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

    // setup for button pins
    pinMode(BUTTON_PIN_1, INPUT_PULLUP);

    // attach interrupt to detect button_pressed event
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_1), handleInterrupt_button1, FALLING);
}

void turn_on_led(int recv_msg_num) {
    int index;
    switch (recv_msg_num) {
        case 0:
            Serial.println("recv_msg_num=0");
        case 1:
            index = 0;
            break;
        default:
            Serial.print("Invalid recv_msg_num: ");
            Serial.println(recv_msg_num);
            return;
    }

    // alert
    //TODO buzzer_hello_world();

    // turn on the corresponding LEDs
    set_led_as_turned_on(index);

    delay(1000);
}

void set_led_as_turned_on(int led_index) {
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
            }
        }

        checker = checker | queue[i].led_on;
    }

    light_on = checker;
}
