#include <Adafruit_MPR121.h>
#include <Wire.h>

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

#define _SHARP_CHAR   8
#define _ASTROID_CHAR 0
#define INTERVAL      5000


// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;
uint16_t numberIntObtain = 0;


// default key = (5, 3, 7, 11, 5, 3, 7, 11, 8), which is identical with (8,1,2,3,8,1,2,3)
char KEY_VAL[33] = {5, 3, 7, 11, 5, 3, 7, 11, 8};

unsigned int input_index = 0;
unsigned char inputVal[33] = {};
unsigned long limit = 0;


void compareInputWithKey() {
    boolean checker = false;

    for (int i = 0; i < input_index; i+=1) {
        if (KEY_VAL[i] != inputVal[i]) {
            // error message for debugging
            Serial.print("i = ");
            Serial.print(i);
            Serial.print("  =>  KEY_VAL[i] = ");
            Serial.print(((int) KEY_VAL[i]));
            Serial.print(", inputVal[i] = ");
            Serial.println(((int) inputVal[i]));

            checker = true;
            break;
        }
    }

    handleAstroidKey();

    // check if the input did not match with the pattern.
    if (checker) {
        Serial.println("Invalid input!!");
        return;
    }

    Serial.println("Valid input!!");
}

void handleAstroidKey() {
    Serial.println("Initialise the input!");

    // reinitialise the array and index value
    input_index = 0;
    inputVal[33];

    limit = 0; // change the value of time limit to 0
}

void setUpTimeLimit() {
    limit = millis() + INTERVAL;
}

void buildInput(uint16_t touchedKey) {
    unsigned char keyChar = (unsigned char) touchedKey;

    inputVal[input_index] = keyChar;
    input_index += 1;

    switch (touchedKey) {
        case _ASTROID_CHAR :
            handleAstroidKey();
            break;
        case _SHARP_CHAR :
            compareInputWithKey(); //compare the input values with the key
            break;
        default:
            Serial.print("Input = ");
            Serial.println(keyChar);
            setUpTimeLimit();
    }
}

// Interrupt handler for pin 36
void IRAM_ATTR gpio36_ISR() {
    // Interrupt flag on
    numberIntObtain = 1;
    // Get the currently touched pads
    //currtouched = cap.touched();
}

void setup() {
  Serial.begin(115200);
  Serial.println("System started");

  Serial.println("Adafruit MPR121 Capacitive Touch sensor test");

  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");

  // set up interrupt source from MPR121 Touch IRQ
  // register gpio36_ISR() as interrupt handler of GPIO pin 36
  pinMode(36, INPUT);
  attachInterrupt(36, gpio36_ISR, FALLING);
}

void loop() {
/*
  if(digitalRead(36) == 0 ) {
    Serial.print("pin 36 low\n");
  } 
*/

  if (limit != 0 && millis() > limit) {
      Serial.println("Timeout!!");
      handleAstroidKey();
  }


  if (numberIntObtain == 0) return;


//  if (numberIntObtain == 0 ) {
//    Serial.print("Number Interrupt Obtained : ");
//    Serial.println(numberIntObtain);
//    currtouched = cap.touched();
//    numberIntObtain = 0;
//  } else return;

//  Serial.print("Number Interrupt Obtained : ");
//  Serial.println(numberIntObtain);

  // Get the currently touched pads => move into gpio36_ISR
  currtouched = cap.touched();

  /*
   if(digitalRead(36) == 0 ) {
        //Serial.println("pin 36 low\n");
        Serial.println("0");
        currtouched = cap.touched();
    } else Serial.println("1");
  */
  Serial.print("currTouched : ");
  Serial.println(currtouched);

  for (uint8_t i = 0; i < 12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
//      Serial.print(i);
//      Serial.println(" touched");

      buildInput(i); //TODO - need to test
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
//      Serial.print(i);
//      Serial.println(" released");
    }
  }

  // reset our state
  lasttouched = currtouched;
  numberIntObtain = 0;

  // comment out this line for detailed data from the sensor!
  return;

  // debugging info, what
  Serial.print("\t\t\t\t\t\t\t\t\t\t\t\t\t 0x"); Serial.println(cap.touched(), HEX);
  Serial.print("Filt: ");
  for (uint8_t i = 0; i < 12; i++) {
    Serial.print(cap.filteredData(i)); Serial.print("\t");
  }
  Serial.println();
  Serial.print("Base: ");
  for (uint8_t i = 0; i < 12; i++) {
    Serial.print(cap.baselineData(i)); Serial.print("\t");
  }
  Serial.println();

  // put a delay so it isn't overwhelming
  delay(100);
}
