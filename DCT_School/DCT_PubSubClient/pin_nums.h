/*
 * 34, 35, 36, and 39 are INPUT ONLY pins!
 * Furthremore, those pins do not have pull up resistors.
 * Thus, you need to
 * 
 * 21 and 22 are used for I2C.
 * 
 * For more information, please refer to the following web page: 
 *    url: <https://randomnerdtutorials.com/esp32-pinout-reference-gpios/>
 */
// sound_alert
#define BUZZER_PIN  19

// LED Pin numbers
#define LED_PIN_1_1 2
#define LED_PIN_1_2 4

#define LED_PIN_2_1 16   // RX2
#define LED_PIN_2_2 17   // TX2

#define LED_PIN_3_1 5
#define LED_PIN_3_2 18

#define LED_PIN_4_1 13
#define LED_PIN_4_2 12

#define LED_PIN_5_1 14
#define LED_PIN_5_2 27

#define LED_PIN_6_1 26
#define LED_PIN_6_2 25

#define LED_PIN_7_1 33
#define LED_PIN_7_2 32

// Button Pin numbers
#define BUTTON_PIN_1 15
#define BUTTON_PIN_2 39
#define BUTTON_PIN_3 36
#define BUTTON_PIN_4 34
#define BUTTON_PIN_5 35
#define BUTTON_PIN_6 23
#define BUTTON_PIN_7 3
