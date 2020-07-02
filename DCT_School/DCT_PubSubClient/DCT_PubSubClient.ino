#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>


#define _LOOP_DELAY_TIME 1000
#define PUB_SUB_PORT_NUM 1883

// device id
static const int id = 2;

// the number of total devices
static const int numOfDevices = 3;

// SSID and PW for WiFi connection
const char* ssid = "dreamscometrue"; //"REPLACE_WITH_YOUR_SSID";
const char* password = "11112345"; //"REPLACE_WITH_YOUR_PASSWORD";

// IP address of the message broker
const char* mqtt_server = "192.168.0.16"; //"YOUR_MQTT_BROKER_IP_ADDRESS";

// instances for pub-sub communication
WiFiClient espClient;
PubSubClient client(espClient);


//----------------------------------------------
//   Function prototypes
//----------------------------------------------

// wifi_pubsub
void setup_wifi();
void pubsubCallback(char* topic, byte* message, unsigned int length);
void reconnect();
void publishMessage(char msgNum);

// display
void setup_display();

// led_button_crtl
void setup_led_button_ctrl();
bool isButtonPressed();
void setButtonPressed(bool b);
int getButtonNum();
void turn_on_led(int recv_msg_num);
void check_led_lifetime();
bool isLightOn();

// sound_alert
void setup_sound_alert();
void buzzer_hello_world();
void buzzer_supermario();
void buzzer_error();

//----------------------------------------------


void setup() {
    Serial.begin(115200);

    // setup for wifi_pubsub
    setup_wifi();
    client.setServer(mqtt_server, PUB_SUB_PORT_NUM);
    client.setCallback(pubsubCallback);

    // setup for led_button_ctrl
    setup_led_button_ctrl();

    // setup for sound_alert
    setup_sound_alert();

    // setup for display (LCD display)
    setup_display();

    // let the user know that the device is ready to start
    //buzzer_hello_world();
}

//#include "pin_nums.h"

void loop1() {
    delay(100);
}

void loop() {
    // turn-off the LEDs who are turned on more than 30 seconds
    if (isLightOn()) check_led_lifetime();

    // Check if the pubsub client is connected.
    // If not, try reconnection.
    if (!client.connected()) {
        reconnect();
    }

    // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
    client.loop();

    // get input or check interrupt
    if (isButtonPressed()) {
        setButtonPressed(false);
        int pressedButtonNum = getButtonNum();
        publishMessage(pressedButtonNum);
        //turn_on_led(pressedButtonNum);
    }

    delay(_LOOP_DELAY_TIME);
    //yield();
}
