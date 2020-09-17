/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini

   Port http connection example
*/

#include "WiFi.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>


const char* ssid     = "KT_GiGA_2G_Wave2_794C";    // 연결할 SSID
const char* password = "5de80xx381";     // 연결할 SSID의 비밀번호
const char* server_url = "http://52.78.8.180/api/test/plant/ctrl/";
char    str_MacAddress[20];
uint8_t plantMacAddress[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void get_and_set_AP()
{
   // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("scan start");
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }
    Serial.print("Now we going to connect to ");
    Serial.println(ssid);

     WiFi.begin(ssid, password);
    // 와이파이망에 연결
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
}

void setup()
{
    Serial.begin(115200);
//
    get_and_set_AP();
    //WiFi.macAddress(plantMacAddress);
    Serial.print("My Mac Address ; ");
    //Serial.println(plantMacAddress); 
    Serial.println(WiFi.macAddress());
    //
    WiFi.macAddress(plantMacAddress);
    sprintf(str_MacAddress,"%x%x%x%x%x%x", 
        plantMacAddress[0], plantMacAddress[1], plantMacAddress[2], 
        plantMacAddress[3], plantMacAddress[4], plantMacAddress[5]);
    Serial.print("Mac Addr : ");
    Serial.println(str_MacAddress);
}

void getCommand()
{
    HTTPClient http;
    char  strHttpURL[256];
    //String  strHttpURL =  server_url + myMacAddress;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    //http.begin("https://www.howsmyssl.com/a/check", ca); //HTTPS
    //http.begin("http://example.com/index.html"); //HTTP

    strcpy(strHttpURL, server_url);
    strcat(strHttpURL, str_MacAddress);

    Serial.println(strHttpURL);
    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    http.begin(strHttpURL);
    int httpCode = http.GET();

    Serial.println(httpCode);

    // httpCode will be negative on error
    if(httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if(httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            Serial.println(payload);

            parsePayloadToJson(payload);
        }
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
}

boolean parsePayloadToJson(String payload) {
    // ArduinoJson v5와 v6 사이에 큰 변화가 있었음. 자세한 내용은 아래 링크들 참고
    //
    //    <https://stackoverflow.com/a/61420255/9012940https://stackoverflow.com/a/61420255/9012940>
    //    <https://arduinojson.org/v6/doc/upgrade/https://arduinojson.org/v6/doc/upgrade/>

    StaticJsonDocument<1024> doc; // Memory pool
    DeserializationError error = deserializeJson(doc, payload);

    // Check for errors in parsing
    if (error) {
        Serial.println("Parsing failed!");
        return false;
    }

    // ArduinoJson 웹페이지에서는 arduino json assistant라는 서비스를 제공하는데, 주어진 json에서 각 멤버 변수들을 꺼내는 예제 코드를 제공한다.
    // 해당 서비스는 아래 링크를 통해서 접근 가
    //  <https://arduinojson.org/v6/assistant/>

    const char* plant_status = doc["status"];
    // check if status == "success"
    if (strcmp(plant_status, "success")) {
        Serial.print("Invalid status: ");
        Serial.println(plant_status);
        return false;
    } else {
        Serial.print("Valid status: ");
        Serial.println(plant_status);
    }

    // get json array from the document
    JsonArray data_list = doc["list"];
    int size_of_list = data_list.size();

    Serial.print("Size of data list: ");
    Serial.println(size_of_list);

    for (int i = 0; i < size_of_list; i++) {
        const char* id = data_list[i]["id"];
        const char* type = data_list[i]["type"];
        const char* code = data_list[i]["code"];

        Serial.print("index: ");
        Serial.print(i);
        Serial.print(", id: ");
        Serial.print(id);
        Serial.print(", type: ");
        Serial.print(type);
        Serial.print(", code: ");
        Serial.println(code);
    }
    Serial.println();

    return true;
}


void loop() {

    getCommand();
    // Wait a bit before scanning again
    delay(5000);
}
