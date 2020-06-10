/**
 * Codes for publish-subscribe pattern with MQTT protocol.
 * 
 * @author Yeonwoo Sung
 * 
 * API document for the pubsubclient is available on <https://pubsubclient.knolleary.net/api>
 * GitHub repository for the pubsubclient library: <https://github.com/knolleary/pubsubclient>
 */

//----------------------------------------------
//  Function prototypes
//----------------------------------------------

// led_button_ctrl
void turn_on_led(int recv_msg_num);

//----------------------------------------------

#define RECONNECT_DELAY 5000
#define PUB_SUB_TOPIC   "dct/pubsub"

#define INT_TO_CHAR(X)  (X + '0')


void setup_wifi() {
    delay(10);

    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void pubsubCallback(char* topic, byte* message, unsigned int length) {
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    //String messageTemp;
    char message_buf[length + 1] = {0};

    for (int i = 0; i < length; i++) {
        char message_char = (char) message[i];
        Serial.print(message_char);
        message_buf[i] = message_char;
    }
    Serial.println();
    message_buf[length] = 0;

    // check if the topic of the sent message is the one that we are interested in.
    if (strcmp(topic, PUB_SUB_TOPIC) == 0) {

        // check the content of the received message

        // based on the assumption that the format of the sent message is '${sender},${messageNum}'
        // i.e.  '1,1' =>  sender = 1, messageNum = 1
        int sender = message_buf[0] - '0';
        char msg_num = message_buf[2];
        int msg_number = msg_num - '0';

        // check if it is ok to ignore this message
        if (sender == id) return;

        switch (msg_num) {
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
                turn_on_led(msg_number);
                break;
            case '6':
                turn_on_led(msg_number);
                //TODO do something with sender?
                break;
            case '7':
                turn_on_led(msg_number);
                //TODO do something with sender?
                break;
            default:
                Serial.print("- [Error] Invalid message number : ");
                Serial.println(msg_num);
                return;
        }

        //TODO
    }
}

void reconnect() {
    char c = (char) INT_TO_CHAR(id);
    char client_id[12] = {'E', 'S', 'P', '3', '2', '_', 'D', 'C', 'T', '_', c, 0};

    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");

        // Attempt to connect
        if (client.connect(client_id)) {
            Serial.println("connected");

            // Subscribe to the topic that we are interested in.
            //client.subscribe(PUB_SUB_TOPIC);
            client.subscribe(PUB_SUB_TOPIC, 1); // QoS = 1
        } else {
            Serial.print("- [Error] failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");

            // Wait few seconds before retrying
            delay(RECONNECT_DELAY);
        }
    }
}

void publishMessage(char msgNum) {
    /*
     * message format:
     *      "${id},${msg}"
     *      
     *      i.e. "1,1"
     */
    char id_char = (char) INT_TO_CHAR(id);
    char msg_char = (char) INT_TO_CHAR(msgNum);
    char msgToSend[4] = {id_char, ',', msg_char, 0};

    Serial.print("Publish : ");
    Serial.println(msgToSend);
    client.publish(PUB_SUB_TOPIC, msgToSend);
}
