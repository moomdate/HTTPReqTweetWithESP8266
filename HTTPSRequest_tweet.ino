#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#define pin1 D0
#define pin2 D1
#define pin3 D2
const char* ssid = "Ollydbg";
const char* password = "11223344";
String thingKey = "xxxxxx";
const char* host = "api.thingspeak.com";
const int httpsPort = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "CF 05 98 89 CA FF 8E D8 5E 5C E0 C2 E4 F7 E6 C3 C7 50 DD 5C";
int stat = 0;
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
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

  // Use WiFiClientSecure class to create TLS connection
  pinMode(pin1, INPUT_PULLUP);
  pinMode(pin2, INPUT_PULLUP);
  pinMode(pin3, INPUT_PULLUP);
}

void loop() {
  while (digitalRead(pin1) == 1 && digitalRead(pin2) == 1 && digitalRead(pin3) == 1) {   
    Serial.println("waiting.....");
   // break;
    delay(10);
  }
  stat = digitalRead(pin1) == 0 ? 1 : digitalRead(pin2) == 0 ? 2 : digitalRead(pin3) == 0 ? 3 : 0;
  reque(message(stat));
}
String message(int statu_) {
   int randNumber = random(999);
  switch(statu_){
    case 0: return ""; break;
    case 1: return "Over%20Heat%20Alarm%20%23" + (String)randNumber; break;
    case 2: return "Door%20Open%20Alarm%20%23" + (String)randNumber; break;
    case 3: return "Intruders%20Alarm%20%23" + (String)randNumber; break;
  }
}
void reque(String text) {
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }

  String url = "/apps/thingtweet/1/statuses/update?api_key=" + thingKey + "&status="; //NFI1KHPEB1XNWTIP
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + text + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
}

