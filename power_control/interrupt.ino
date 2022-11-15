#include<util/delay.h>
int Car_power = 2;
int Nx_power = 3;
int IO_pin = 8;
int Nx_on = 10;

void setup() {
  // put your setup code here, to run once:
//  Serial.begin(9600);
  pinMode(Nx_on, INPUT);
  pinMode(IO_pin, OUTPUT);
  pinMode(Nx_power, OUTPUT);
  pinMode(Car_power, INPUT);
  digitalWrite(Nx_power, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly: ? 0 -? 1
  digitalWrite(IO_pin, LOW);
  //d
  delay(1000);
  //  Serial.print("flag_nx:");
  //  Serial.print(digitalRead(Nx_on));
  //  Serial.print("\n");
  if (digitalRead(Car_power) != digitalRead(Nx_on)) {
//  if (t == 10) {
    delay(1000);
    if (digitalRead(Car_power) != digitalRead(Nx_on)) {
      Recovery(); 
//    t = 0;
    }
  }
}

void Recovery() {
//  Serial.print("Recovery call\n");
//  if (digitalRead(Car_power) != digitalRead(Nx_on)) {
    if (digitalRead(Car_power) == 1) {
//            Serial.print("Recovery on!\n");
      digitalWrite(Nx_power, LOW);
      delay(1000);
//      Serial.print("Recovery on done!\n");
      digitalWrite(Nx_power, HIGH);
    }
    else {
//      Serial.print("Recovery off!\n");
      digitalWrite(IO_pin, HIGH);
      digitalWrite(Nx_power, LOW);
      delay(10000);
      digitalWrite(Nx_power, HIGH);
//      Serial.print("Recovery off done\n");
    }
//  }
}
