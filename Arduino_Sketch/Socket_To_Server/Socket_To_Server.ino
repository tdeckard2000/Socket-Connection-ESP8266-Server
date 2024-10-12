/*
 * Ground D2 to see updates
 * LED On: Not connected to server
 * LED Off: Connected to server
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#define D2 4
#define D4 2
unsigned long previousMillis = 0;
const long interval = 100;
int previousPinState = 0;
const char *serverUrl = ""; //ex 10.20.109.20
const int serverPort = 4000;
const char *ssid = "";  //Wifi Name
const char *password = ""; //Wifi Password
WiFiClient client;

void setup() {
  Serial.begin(115200);
  pinMode(D2, INPUT_PULLUP);
  pinMode(D4, OUTPUT);
  connectToWifi();
  connectToServer();
}

void loop() {
  if (client.connected()) {
    checkForIncomingData();
    checkForUpdatedPinState();
  } else if (!client.connected()) {
    toggleLED(1);
    Serial.println("Lost connection");
    Serial.println("Attempting to reconnect in 5 seconds..");
    delay(5000);
    connectToWifi();
    connectToServer();
  }
}

void checkForIncomingData() {
  if (client.available()) {
    Serial.println("Waiting...");
    String data = client.readStringUntil('\n');
    Serial.println("Not waiting anymore");
    Serial.println(data);
  }
}

void checkForUpdatedPinState() {
  int pinState = getPinState(D2);
  if (pinState != previousPinState) {
    previousPinState = pinState;
    Serial.println(pinState);
    client.println(pinState);
  }
}

void connectToServer() {
  if (client.connect(serverUrl, serverPort)) {
    toggleLED(0);
    Serial.println("Connected to server");
    client.println("Hello from ESP8266");
    while (client.available()) {  //waiting for first messages
      String response = client.readStringUntil('\n');
      Serial.println(response);
    }
  } else {
    toggleLED(1);
    Serial.println("Error connecting to server. Trying again in 5 seconds..");
    delay(5000);
    connectToServer();
  }
}

void connectToWifi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }
  Serial.println();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void toggleLED(int state) {
  digitalWrite(D4, state == 1 ? LOW : HIGH);
}

int getPinState(int pin) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    int state = digitalRead(pin);
    if (state == LOW) {
      return 0;
    } else if (state == HIGH) {
      return 1;
    }
  }
  return previousPinState;
}