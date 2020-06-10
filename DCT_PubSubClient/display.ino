#include <LiquidCrystal.h>


#define DISPLAY_TIME_ON_BOOT 3000

LiquidCrystal lcd(22,23,5,18,19,21);


void setup_display() {
    lcd.begin(16, 2);
    lcd.clear();
    lcd.print("DCT project");

    // go to row 1 column 0, note that this is indexed at 0
    lcd.setCursor(0,1);
    lcd.print ("LCD with ESP32");

    delay(DISPLAY_TIME_ON_BOOT);

    lcd.clear();
}

void display_sender(int sender_id) {
    lcd.setCursor(0,0);
    //TODO check the sender_id
    lcd.print("Sender ID");
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
