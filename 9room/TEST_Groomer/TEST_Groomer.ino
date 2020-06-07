#include <DFRobot_SHT20.h>
#include <WiFi.h>
#include "BLEUtils.h"
#include "BLEScan.h"
#include "BLEAdvertisedDevice.h"
#include "BLEDevice.h"
#include "BLEServer.h"
#include "SPIFFS.h"
#include "FS.h"

//-----------------Preprocessors------------------------//

/* Define for main */
// baud rate for serial monitor
#define SERIAL_PORT_BAUD_RATE      115200
//port number for HTTP communication
#define PORT_NUMBER          8090
// for WIFi COnnection setup
#define DELAY_TIME_WIFI_CONN 500
#define DELAY_LIMIT_WIFI_CLI 15000

#define DELAY_TIME_MAIN_LOOP 500
#define DELAY_TIME_BLE_SCAN  600
#define DELAY_TIME_BLE_START 2000
#define DELAY_TIME_BLE_CONN  5000

#define DELAY_INIT_SHT20     200

#define DELAY_WAIT_CONNECT   1000
#define DELAY_WAIT_RESPONSE  1000
#define DELAY_WAIT_MAIN      3000
//#define DELAY_WAIT_MAIN      300000

#define BLUE_TOOTH_SCAN_TIME 50
#define BLE_SCAN_WINDOW_SIZE 99
#define BLE_SCAN_INTERVAL    100

#define CHAR_UUID_CTL        "00001524-1212-efde-1523-785feabcd123"
#define CHAR_UUID_MEAS       "00001525-1212-efde-1523-785feabcd123"
#define CHAR_UUID_LOG        "00001526-1212-efde-1523-785feabcd123"
#define ALLOCATE_SIZE_LOG    11
#define INIT_TARGET_DEV_NAME "BLE_Gamma:0123"

/*
#define WIFI_SSID "KT_GiGA_2G_Wave2_794C"
#define WIFI_PASSWD "5de80xx381"
*/
#define WIFI_SSID "tech_room"
#define WIFI_PASSWD "techroom"

/* Define for BLE Server */
//
#define BLE_SERVER_LIFE_TIME 180000
#define BLE_SERVER_ADD_TIME  120000
#define BLE_SERVER_SUB_TIME  2000

#define D_NAME_SERVICE_UUID  "4fafc203-1fb5-459e-8fcc-c5c9c331914b" //uuid for the device name setting service
#define D_NAME_CHAR_UUID     "bec5483e-36e1-4688-b7f5-ea07361b26a8" //uuid for the device name setting characteristic

#define WIFI_SERVICE_UUID    "4fafc201-1fb5-459e-8fcc-c5c9c331914b" //uuif for wifi name service
#define WIFI_PW_SERVICE_UUID "4fafc202-1fb5-459e-8fcc-c5c9c331914b" //uuid for wifi pw service
#define WIFI_NAME_CHAR_UUID  "beb5483e-36e1-4688-b7f5-ea07361b26a8" //uuid for wifi name characteristic
#define WIFI_PW_CHAR_UUID    "bed5483e-36e1-4688-b7f5-ea07361b26a8" //uuid for wifi pw characteristic

//------------------------------------------------------//

class MyServerCallbacks;

//-----------------Global variables---------------------//

const std::string DEVICE_NUMBER = "u518";
const String DEV_NUM = String(DEVICE_NUMBER.c_str());
const std::string BLE_DEVICE_NAME = "9room-" + DEVICE_NUMBER;

// BLE server
int bleServerLifeTime;
static BLEServer *pServer = nullptr;
static BLEService *pServerService_wifi = nullptr;
static BLEService *pServerService_pw = nullptr;
static BLECharacteristic *pCharacteristic_wifi = nullptr;
static BLECharacteristic *pCharacteristic_pw = nullptr;
static BLEAdvertising *pAdvertising = nullptr;
static MyServerCallbacks *serverCallback = nullptr;

static std::string *targetName = nullptr;

static BLEService *pServerService_deviceName = nullptr;
static BLECharacteristic *pCharacteristic_deviceName = nullptr;

const char *ssid = WIFI_SSID;
const char *pw = WIFI_PASSWD;

std::string *ssid_str = nullptr;
std::string *pw_str = nullptr;


//int LED_BUILTIN = 2;

//int scanTime = BLUE_TOOTH_SCAN_TIME;

//char *host = "ec2-15-164-218-172.ap-northeast-2.compute.amazonaws.com";
char *host = "groom.techtest.shop";
String hostStr = String(host);
String url_g = "/logoneg";
String url_th = "/logoneth";
String url_time = "/time";



uint32_t numberIntObtain;
/*
static BLEUUID WP_CTL_UUID(CHAR_UUID_CTL);
static BLEUUID WP_MEAS_UUID(CHAR_UUID_MEAS);
static BLEUUID WP_LOG_UUID(CHAR_UUID_LOG);
static BLEUUID SERVICE_UUID("00001523-0000-1000-8000-00805f9b34fb");

static BLERemoteCharacteristic* characteristic_cmd_control;
static BLERemoteCharacteristic* characteristic_cmd_measurement;
static BLERemoteCharacteristic* characteristic_cmd_log;


static uint8_t cmd_RESULT_RETURN = 0xA0;
static uint8_t cmd_BLE_STARTSTOP = 0x10;

static uint8_t cmd_BLE_Date_Time_Set = 0x20;
static uint8_t cmd_LOG_INFO_QUERY = 0x21;
static uint8_t cmd_LOG_DATA_SEND = 0xA1;

static uint8_t cmd_BLE_USER_NAME_SET = 0x30;
static uint8_t cmd_BLE_USER_NAME_QUERY = 0x31;
*/

static int serialNum_g = 0;
static int serialNum_th = 0;

static DFRobot_SHT20 sht20;


//------------------------------------------------------//

//--------------Function prototypes---------------------//
bool validate_sht20(float val);
// Wifi
void setupWiFi();
void connectWiFi();
int sendViaHTTP(String queryString, String url);
// int sendGeiger(float measuredVal, int deviceNum);
// int sendTemperatureAndHumidity(float temperature, float humidity, int deviceNum);
int sendGeiger(float measuredVal, String deviceNum);
int sendTemperatureAndHumidity(float temperature, float humidity, String deviceNum);
// SPIFFS
void readFile(fs::FS &fs, const char * path, int type);
void writeFile(fs::FS &fs, const char * path, const char * message);
void deleteFile(fs::FS &fs, const char * path);
// BLE Server
void initBLEServer();
void removeAdvertising();
//------------------------------------------------------//


bool validate_sht20(float val) {
    int i = (int) val;
    bool isValid = false;
    switch (i) {
        case 998:
            Serial.println("Error::I2C_Time_Out - Sensor not detected");
            break;
        case 999:
            Serial.println("Error::BAD_CRC - CRC bad");
            break;
        default:
            isValid = true;
    }
    return isValid;
}

// Interrupt handler for pin 36
void IRAM_ATTR gpio36_ISR() {
  numberIntObtain += 1;
}
/* Initialise things before starting the main loop */
void setup() {
//BC    targetName = new std::string(INIT_TARGET_DEV_NAME);

    Serial.begin(SERIAL_PORT_BAUD_RATE); //Start Serial monitor in 115200
  
    Serial.println("Initialise the BLE module");
    BLEDevice::init(BLE_DEVICE_NAME);

    if (!SPIFFS.begin(true)) {
        Serial.println("An error has occurred while mounting SPIFFS\n");
    } else {
// read configuration from SPIFFS
        readFile(SPIFFS, "/wifi_name.txt", 1);
        readFile(SPIFFS, "/wifi_pw.txt", 2);
        readFile(SPIFFS, "/device_name.txt", 3);
    }
    
// replace with initBLEServer()    setup_BLEServer();  now BLESever is run lifetime long 
     initBLEServer();
    setupWiFi(); //initialise and connect to the WiFi
   
//    setup_BLEClient()

   //set up DFRobot_SHT20
    sht20.initSHT20();
    delay(DELAY_INIT_SHT20);
    sht20.checkSHT20();

// set up interrupt source from Geiger Counter
// register gpio36_ISR() as interrupt handler of GPIO pin 36
  pinMode(36, INPUT_PULLUP);
  numberIntObtain = 0;
  attachInterrupt(36, gpio36_ISR, RISING);
}


/* The main loop of the arduino. */
void loop() {
    int waitTime = DELAY_WAIT_MAIN;
   
    if (WiFi.status() != WL_CONNECTED) { // if WiFi is not connected, try connect again, again, and again...
      Serial.println("WiFi is not connected! Connect Again!");
      setupWiFi();
      waitTime = DELAY_TIME_MAIN_LOOP;
    } else {
   
      float humd = sht20.readHumidity();
      Serial.print("humidity: ");
      Serial.println(humd);
      bool humidityIsValid = validate_sht20(humd);

      float temp = sht20.readTemperature();
      Serial.print("temperature: ");
      Serial.println(temp);
      bool temperatureIsValid = validate_sht20(temp);

      serialNum_g += 1;
      serialNum_th += 1;
      
      // send tempearture and humidity only when both humidity value and temperature value are valid (non error).
      if (humidityIsValid && temperatureIsValid) {
        sendTemperatureAndHumidity(temp, humd, DEV_NUM);
      } 
      float measuredVal = 0.0;
      //measureVal = getGeiger()
      Serial.print("Num Interrupt : ");
      Serial.println(numberIntObtain);
      measuredVal = (float)numberIntObtain;
      sendGeiger(measuredVal, DEV_NUM);
      numberIntObtain = 0;
      waitTime = DELAY_WAIT_MAIN;
    }
    delay(waitTime);
}


/********** WiFi Communication ************/

/**
 * Function that helps the device to set up the WiFi environment.
 */

void setupWiFi() {
    Serial.println("setupWiFi()");
    
    if (WiFi.status() != WL_CONNECTED) WiFi.disconnect();
    WiFi.mode(WIFI_STA); //init wifi mode
    if (ssid_str != nullptr) {
        if (pw_str == nullptr) {
            WiFi.begin(ssid_str->c_str());
        } else {
            WiFi.begin(ssid_str->c_str(), pw_str->c_str());
        }
    } else {
        if (pw == "") {
            WiFi.begin(ssid); //Open the WiFi connection so that ESP32 could send data to server via HTTP/HTTPS protocol.
        } else {
            WiFi.begin(ssid, pw); //Open the WiFi connection so that ESP32 could send data to server via HTTP/HTTPS protocol.
        }
    }
    connectWiFi();
}

/**
 * Function that connects the device to the target WiFi.
 */
void connectWiFi() {
    Serial.print("\n\nConnecting to WiFi...");

    int count = 0;

    // use while loop to wait until the device is connected to the WiFi
    while (WiFi.status() != WL_CONNECTED) {
        if (count >= DELAY_LIMIT_WIFI_CLI)
            break;

        count += DELAY_TIME_WIFI_CONN;
        delay(DELAY_TIME_WIFI_CONN);
        Serial.print(".");
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\nConnection failed :(");
    } else {
        Serial.println("\nConnected!");
        Serial.print("\nConnected to:\n\t\tIP address = ");
        Serial.println(WiFi.localIP()); //print out the ip address.
    }
}


/**
 * Send the collected data to the TEMS server via HTTP.
 */
int sendViaHTTP(String queryString, String url) {
    WiFiClient client;

    Serial.print("\nConnecting to ");
    Serial.println(hostStr);

    //check if the http client is connected.
    if (!client.connect(host, PORT_NUMBER)) {
        Serial.print("Connection failed...");
        return 0;
    }

    String header = "GET " + url + queryString + " HTTP/1.1";

    client.println(header);
    client.println("User-Agent: ESP32-groomer");
    client.println("Host: " + hostStr);
    client.println("Connection: closed");
    client.println();

    Serial.println("Data sending process success!\n"); //to debug

    client.stop();

    return 1;
}

/**
 * Send the collected geiger data to the server
 */
int sendGeiger(float measuredVal, String deviceNum) {
    String queryString = "?s=" + String(serialNum_g) + "&g=" + String(measuredVal) + "&u=" + deviceNum;
    return sendViaHTTP(queryString, url_g);
}

/**
 * Send the temperature data and humidity data to the server.
 */
int sendTemperatureAndHumidity(float temperature, float humidity, String deviceNum) {
    String queryString = "?s=" + String(serialNum_th) + "&t=" + String(temperature) + "&h=" + String(humidity) + "&u=" + deviceNum;
    return sendViaHTTP(queryString, url_th);
}

/*********** End of Wifi Communication **********************/
//
//
/*********** Configure SPIFFS  *****************************/
void readFile(fs::FS &fs, const char * path, int type) {
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);

    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");

    std::string readLine = "";

    while(file.available()){
        readLine += char(file.read());
    }

    Serial.println(readLine.c_str());

    switch (type) {
        case 1 :
            if (ssid_str != nullptr) delete ssid_str;
            ssid_str = new std::string(readLine);
            break;
        case 2 :
            if (pw_str != nullptr) delete pw_str;
            pw_str = new std::string(readLine);
            break;
        case 3 :
            if (targetName != nullptr) delete targetName;
            targetName = new std::string(readLine);
            break;
        default :
            Serial.println("Invalid type!");
    }

    Serial.println("\nFinished reading file\n\n");
}

void writeFile(fs::FS &fs, const char * path, const char * message) {
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written\n");
    } else {
        Serial.println("- frite failed\n");
    }
}

void deleteFile(fs::FS &fs, const char * path) {
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted\n");
    } else {
        Serial.println("- delete failed\n");
    }
}

/*********** End of SPIFFS **********************/

/*********** BLE Server **************************/

//--------------Class definitions ---------------------//
/**
 * The aim of this class is to implement a custom event listener to get the wifi password.
 */
class WiFiPasswordCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();

        if (value.length() > 0) {
            if (pw_str != nullptr) delete pw_str;

            pw_str = new std::string(value);

            deleteFile(SPIFFS, "/wifi_pw.txt");
            writeFile(SPIFFS, "/wifi_pw.txt", value.c_str());

            Serial.println("*********");
            Serial.print("New value: ");
            for (int i = 0; i < value.length(); i++)
                Serial.print(value[i]);

            Serial.println();
            Serial.println("*********");
        }
    }
};

/**
 * The aim of this class is to implement a custom event listener to get the wifi name.
 */
class WiFiNameCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();

        if (value.length() > 0) {
            if (ssid_str != nullptr) delete ssid_str;
            ssid_str = new std::string(value);

            deleteFile(SPIFFS, "/wifi_name.txt");
            writeFile(SPIFFS, "/wifi_name.txt", value.c_str());

            Serial.println("*********");
            Serial.print("Received Wi-Fi ssid: ");
            for (int i = 0; i < value.length(); i++)
                Serial.print(value[i]);

            Serial.println("\n*********");
        }
    }
};

/**
 * The ble server's event listener that helps the esp32 to store the target device name.
 */
class DeviceNameCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();

        if (value.length() > 0) {
            if (targetName != nullptr) delete targetName;

            targetName = new std::string(value);

            deleteFile(SPIFFS, "/device_name.txt");
            writeFile(SPIFFS, "/device_name.txt", value.c_str());

            Serial.println("*********");
            Serial.print("New BLE device name: ");
            for (int i = 0; i < value.length(); i++)
                Serial.print(value[i]);

            Serial.println("\n*********");
        }
    }
};

/**
 * The aim of this class is to implement a custom event listener for the local BLE server.
 */
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        Serial.println("- MyServerCallbacks::onConnect() executed");
        bleServerLifeTime += BLE_SERVER_ADD_TIME;
    };

    void onDisconnect(BLEServer* pServer) {
        Serial.println("- MyServerCallbacks::onDisconnect() executed");
    }
};

/**
 * Initialise the BLE server on ESP32.
 * To obtain WiFi name, WiFi Passwd, Device Name.
 */
void initBLEServer() {
    Serial.println("init BLE Server");
    if (pServer != nullptr) delete pServer;   // Initialize pServer pointer
    pServer = BLEDevice::createServer();
    serverCallback = new MyServerCallbacks();
    pServer->setCallbacks(serverCallback);

    if (pServerService_wifi != nullptr) delete pServerService_wifi;
    if (pServerService_pw != nullptr) delete pServerService_pw;
    if (pServerService_deviceName != nullptr) delete pServerService_deviceName;

    pServerService_wifi = pServer->createService(WIFI_SERVICE_UUID);
    pServerService_pw = pServer->createService(WIFI_PW_SERVICE_UUID);
    pServerService_deviceName = pServer->createService(D_NAME_SERVICE_UUID);

    if (pCharacteristic_wifi != nullptr) delete pCharacteristic_wifi;
    if (pCharacteristic_pw != nullptr) delete pCharacteristic_pw;
    if (pCharacteristic_deviceName != nullptr) delete pCharacteristic_deviceName;

    pCharacteristic_wifi = pServerService_wifi->createCharacteristic(
                      WIFI_NAME_CHAR_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
    pCharacteristic_pw = pServerService_pw->createCharacteristic(
                      WIFI_PW_CHAR_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
    pCharacteristic_deviceName = pServerService_deviceName->createCharacteristic(
                      D_NAME_CHAR_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

    pCharacteristic_wifi->setCallbacks(new WiFiNameCallbacks());
    pCharacteristic_pw->setCallbacks(new WiFiPasswordCallbacks());
    pCharacteristic_deviceName->setCallbacks(new DeviceNameCallbacks());

    pServerService_wifi->start();
    pServerService_pw->start();
    pServerService_deviceName->start();

    pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
    Serial.println("Local BLE server created!!\n");
}

// Will not be called
void removeAdvertising() {
    pAdvertising->stop();

    if (pAdvertising != nullptr) delete pAdvertising;

    if (pCharacteristic_wifi != nullptr) delete pCharacteristic_wifi;
    if (pCharacteristic_pw != nullptr) delete pCharacteristic_pw;
    if (pCharacteristic_deviceName != nullptr) delete pCharacteristic_deviceName;

    if (pServerService_wifi != nullptr) delete pServerService_wifi;
    if (pServerService_pw != nullptr) delete pServerService_pw;
    if (pServerService_deviceName != nullptr) delete pServerService_deviceName;

    if (serverCallback != nullptr) delete serverCallback;

// This is not a member in standard BLE library    BLEDevice::removeServer();
}

// Dumped from main setup()
/*
setup_BLEServer()
{
// initialize BLE Server
        initBLEServer();
// Turn on/off LED and serial print while BLE server lifetime        
        bleServerLifeTime = BLE_SERVER_LIFE_TIME;
        pinMode (LED_BUILTIN, OUTPUT);
        while (bleServerLifeTime > 0) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(1000);
            digitalWrite(LED_BUILTIN, LOW);
            delay(1000);
            bleServerLifeTime -= BLE_SERVER_SUB_TIME;
            Serial.printf("BLE server mode remaining time: %d sec\n", bleServerLifeTime / 1000);
        }
        Serial.println("\nTerminating BLE server mode");
//terminate BLE server
        removeAdvertising();
        Serial.println("BLE server mode terminated!\nBLE client mode start!\n\n");

        digitalWrite(LED_BUILTIN, HIGH);
        delay(3000);
        digitalWrite(LED_BUILTIN, LOW);
}
*/
/******************* End of BLE Server **************************/
