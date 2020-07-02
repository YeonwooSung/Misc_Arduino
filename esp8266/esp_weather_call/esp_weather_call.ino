#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const int LIMIT = 100;
int count = 0;

void http_call(char* host, char* path, int port) {
  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.print("connection failed: ");
    return;
  }

  client.println("GET "+ String(path) +" HTTP/1.1"); 
  client.println("HOST: "+ String(host)); 
  client.println();

  boolean found = false;

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line.indexOf("<hour>") > -1)
      Serial.println(line);
    if (line.indexOf("<temp>") > -1)  {
      Serial.println(line);
      found = true;
    }
    //Serial.println(line);
    if (found) break;
  }
  Serial.println("Done");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin("capston1", "capston1");
  Serial.println("\nBegin");
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");

    if (count >= LIMIT) {
      Serial.println("Something goes wrong while http connection. Please try again");
      break;
    }
    
    count += 1;
  }
  Serial.println(WiFi.localIP()); 
  http_call("web.kma.go.kr", "/wid/queryDFSRSS.jsp?zone=1159068000", 80);
}

void loop() {}
