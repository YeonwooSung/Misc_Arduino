/*
  Dreams Come True Wireless Message Sender

  Author : Yeonwoo Sung


  This code comed from below project.
  
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-two-way-communication-esp32/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  TODO :
  (1) slave manage --> esp_err_t addStatus = esp_now_add_peer(&slaves[i]);
  (2) create send_msg(int peer, void *pData, int data_len)
      peer setup
      send , getConfitm, resend, timeout mechanism
  (2-1) OnDataRecv
      send back confirm packet.
      
*/

#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define NUM_OF_PEER 3

#define MY_ID_NUM 1
#define MY_NEXT_PEER  2



typedef struct struct_peer_id_addr {
    int id;
    uint8_t mac_addr[6];
} struct_peer_id_addr;


// mac address for broadcasting
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// peer mac addresses
uint8_t sta_1_mac_addr[]= { 0x10, 0x52, 0x1c, 0x50, 0x34, 0x14};
uint8_t sta_2_mac_addr[]= { 0x30, 0xae, 0xa4, 0x99, 0xa5, 0x70};
uint8_t sta_3_mac_addr[]= { 0x24, 0x6f, 0x28, 0x02, 0xba, 0xfc};

struct_peer_id_addr dct_peer_container[NUM_OF_PEER] = {
  { 1,  { 0x10, 0x52, 0x1c, 0x50, 0x34, 0x14} },
  { 2,  { 0x30, 0xae, 0xa4, 0x99, 0xa5, 0x70} },
  { 3,  { 0x24, 0x6f, 0x28, 0x02, 0xba, 0xfc} }
};


// Variable to store if sending data was successful
String success;
bool receive_my_turn;
int loop_counter;

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
    int my_id;
    int peer_id;
} struct_message;

// Create a struct_message called BME280Readings to hold sensor readings
struct_message dct_wmt_msg;

// Create a struct_message to hold incoming sensor readings
struct_message incomingReadings;


// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");

  if (status == 0) {
    success = "Delivery Success :)";
  } else {
    success = "Delivery Fail :(";
  }

  Serial.println(success);
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);

  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));

  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Sender ID: ");
  Serial.println(incomingReadings.my_id);
  Serial.print("Target ID: ");
  Serial.println(incomingReadings.peer_id);

  if (incomingReadings.peer_id == MY_ID_NUM) receive_my_turn = true;
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Init OLED display
  /*
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  */

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  for (int i = 0; i < NUM_OF_PEER; i++) {
      Serial.print(dct_peer_container[i].id);
      for(int j = 0; j < 6; j++) Serial.print(dct_peer_container[i].mac_addr[j]);
      Serial.println(" ");
  }

  // Once ESPNow is successfully Init, we will register for Send CB to get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  //memcpy(peerInfo.peer_addr, sta_1_mac_addr, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);

  receive_my_turn = false;
  loop_counter = 0;

  dct_wmt_msg.my_id = MY_ID_NUM;
  dct_wmt_msg.peer_id = MY_NEXT_PEER;

  // Send message via ESP-NOW. 
  // This part is only in #1
  if(MY_ID_NUM == 1) {
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &dct_wmt_msg, sizeof(dct_wmt_msg));
    result = esp_now_send(broadcastAddress, (uint8_t *) &dct_wmt_msg, sizeof(dct_wmt_msg));
  }
  //esp_err_t result = esp_now_send(sta_2_mac_addr, (uint8_t *) &dct_wmt_msg, sizeof(dct_wmt_msg));
}



void loop() {

  //esp_err_t result;
  while (receive_my_turn == false) yield();

  delay(1000);

  // Send message via ESP-NOW
  //if(MY_ID_NUM == 1) result = esp_now_send(sta_2_mac_addr, (uint8_t *) &dct_wmt_msg, sizeof(dct_wmt_msg));
  //else result = esp_now_send(sta_1_mac_addr, (uint8_t *) &dct_wmt_msg, sizeof(dct_wmt_msg));
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &dct_wmt_msg, sizeof(dct_wmt_msg));
  result = esp_now_send(broadcastAddress, (uint8_t *) &dct_wmt_msg, sizeof(dct_wmt_msg));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  Serial.print("count : ");
  Serial.println(loop_counter++);
  receive_my_turn = false;

  //updateDisplay();
  //delay(100);
}
