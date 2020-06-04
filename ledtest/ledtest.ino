const int ledPin = 5;
const int button_pin = 19;
const int switch_pin = 0;


static volatile bool buttonPressed = false;

void IRAM_ATTR handleInterrupt()
{
  buttonPressed = true;
}

void setup() {
  // setup pin 5 as a digital output pin
  pinMode (ledPin, OUTPUT);
  //pinMode (ledPin, INPUT);
  pinMode(switch_pin, INPUT_PULLUP);
  pinMode(button_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(switch_pin), handleInterrupt, FALLING);
}

void loop() {
  //Serial.println(buttonPressed);
  if(buttonPressed) {
    digitalWrite (ledPin, HIGH);  // turn on the LED
    buttonPressed = false;
  }
  else digitalWrite (ledPin, LOW);
  delay(1000);
  /*
  digitalWrite (ledPin, HIGH);
  delay(1000);
  digitalWrite (ledPin, LOW);
  delay(500);
  */
}
