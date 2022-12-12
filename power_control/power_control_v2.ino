#include <SoftwareSerial.h>
#include <string.h>

//#define NX_BOOT_WAIT_INTERVAL 120000
#define NX_BOOT_WAIT_INTERVAL 40000
#define UART_WAIT_INTERVAL 5000
#define LOOP_INIT_INTERVAL 200
#define MAX_RETRY 10
#define LOOP_CHECK_RETRY 10

const unsigned Car_power = 2;
const unsigned Nx_power = 3;
const unsigned Uart_rx = 9;
const unsigned Uart_tx = 10;

const unsigned long Baudrate = 9600;

const char MSG_OK[3] = "ok";

unsigned retry = 0;

SoftwareSerial mySerial = SoftwareSerial(Uart_rx, Uart_tx);

void setup() {
    // define pin modes for tx, rx:
    // pinMode(Car_power, INPUT);
    pinMode(Nx_power, OUTPUT);
    pinMode(Uart_rx, INPUT);
    pinMode(Uart_tx, OUTPUT);

    // debugging serial port
    Serial.begin(Baudrate);

    Serial.println("Uart Liveness Monitor Starts");
    // set initial delay to wait until the jetson nx boots up
    delay(NX_BOOT_WAIT_INTERVAL);

    // connect with nx board
    // baudrate 115200 : unstable
    // baudrate 9600 : stable
    mySerial.begin(Baudrate);
    mySerial.setTimeout(100);
    mySerial.flush();
    delay(100);
}

void loop() {
    int count = 0;
    while (mySerial.available() <= 0) {
        // wait until the nx board sends a message
        delay(LOOP_INIT_INTERVAL);
        if (count > LOOP_CHECK_RETRY) break;
        count += 1;
    }
    Serial.print("retry: ");
    Serial.println(retry);

    // check if the nx board sends a message
    if(mySerial.available() > 0){
        String str;
        str = mySerial.readString();
        Serial.print("str: ");
        Serial.println(str);
        Serial.print("str_length: ");
        Serial.println(str.length());

        mySerial.write("ok\n");

        Serial.println("hex:");
        for (int i = 0; i < str.length(); i++) {
            Serial.print(str.charAt(i), HEX);
        }
        Serial.println();
        
        if(str.startsWith(MSG_OK)) {
//            // Serial.println(str);
//            mySerial.write("ok\n");
            retry = 0;
        } //else {
//            check_retry();
//        }
        delay(UART_WAIT_INTERVAL);
    } else {
        Serial.println("no data");
        check_retry();
    }
}

void recover_nx() {
    Serial.println("recover_nx");
    digitalWrite(Nx_power, LOW);
    delay(500);
    digitalWrite(Nx_power, HIGH);
    delay(500);
    digitalWrite(Nx_power, LOW);
}

void check_retry() {
    Serial.println("check_retry");
    if (retry < MAX_RETRY) {
        mySerial.write("ok\n");
        retry += 1;
        delay(UART_WAIT_INTERVAL);
    } else {
        retry = 0;
        recover_nx();
        delay(NX_BOOT_WAIT_INTERVAL);
        mySerial.end();
        delay(10);
        mySerial.begin(Baudrate);
        mySerial.setTimeout(100);
    }
}
