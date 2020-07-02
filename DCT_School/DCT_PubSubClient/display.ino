/**
 * Codes for displaying message on the qapass LCD board.
 * As I mentioned below (as a comment), the LiquidCrystal_I2C uses the I2C.
 * The pin 21 is used for SDA, and the pin 22 is used for SCL.
 *
 * For more information:
 *    @reference <http://www.esp32learning.com/code/esp32-and-i2c-lcd-example.php>
 * 
 * @author Yeonwoo Sung
 */
#include <LiquidCrystal_I2C.h>
#include "pin_nums.h"


// ESP32 uses the pin 21 and 22 for I2C.
// 21 for SDA, and 22 for SCL
LiquidCrystal_I2C lcd(0x27,16,2);


void init_lcd();

void setup_display() {
    Serial.println("Start setup_display()");
    lcd.init();
    lcd.init();
    lcd.clear();
    lcd.backlight();

    init_lcd();
    Serial.println("Done!");
}

void init_lcd() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("DCT project");
}

void display_sender(int sender_id) {
    lcd.clear();
    lcd.setCursor(0,0);
    // "Mystery World" = 메이커 스페이스
    // "Vegan Life"    = 교장실
    // "Tanning Room"  = 교무실
    switch (sender_id) {
        case 1:
            lcd.print("Mystery World");
            break;
        case 2:
            lcd.print("Vegan Life");
            break;
        case 3:
            lcd.print("Tanning Room");
            break;
        default:
            lcd.print("Unknown");
    }
}

void display_yes(int sender_id) {
    display_sender(sender_id);
    lcd.setCursor(0, 1);
    lcd.print("Yes");
}

void display_no(int sender_id) {
    display_sender(sender_id);
    lcd.setCursor(0, 1);
    lcd.print("No");
}
