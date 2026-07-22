#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoOTA.h>
#include <Arduino.h>
#include <ArduinoJson.h>

const char* ssid = "asdf";
const char* password = "rockroll";

#define LED_PIN D1
#define NUM_LEDS 300

ESP8266WebServer server(80);
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// ==============
// ESTADO GLOBAL
// ==============

int r1, g1, b1;
int r2, g2, b2;
int r3, g3, b3;

int animationOffset = 0;
unsigned long lastUpdateMs = 0;
int globalWaitMs = 50;
unsigned long lastReconnectAttemptMs = 0;


// =======
// BASE
// =======

void setColor(int r, int g, int b){
  for(int i = 0; i < NUM_LEDS; i++){
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}

void showTriadBands() {
  int section = NUM_LEDS / 3;

  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < section) {
      strip.setPixelColor(i, strip.Color(r1, g1, b1));
    } else if (i < section * 2) {
      strip.setPixelColor(i, strip.Color(r2, g2, b2));
    } else {
      strip.setPixelColor(i, strip.Color(r3, g3, b3));
    }
  }

  strip.show();
}

// HANDLERS

void handleApply(){
  if (!server.hasArg("plain")){
    server.send(400, "application/json", "{\"error\":\"Body missing\"}");
    return;
  }

  String body = server.arg("plain");
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, body);

  if (error) {
    server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }

  const char* mode = doc["mode"];
  int brightness = doc["brightness"];
  JsonArray colors = doc["colors"];

  r1 = colors[0][0];
  g1 = colors[0][1];
  b1 = colors[0][2];

  r2 = colors[1][0];
  g2 = colors[1][1];
  b2 = colors[1][2];

  r3 = colors[2][0];
  g3 = colors[2][1];
  b3 = colors[2][2];

  strip.setBrightness(brightness);


  if (mode && String(mode) == "triad") {
    showTriadBands();
  } else {
    setColor(r1, g1, b1);
  }

  server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleRoot(){
  server.send(200, "text/plain", "ESP8266 online");
}

void handleBright(){
  if (!server.hasArg("value")) {
    server.send(400, "text/plain", "missing value");
    return;
  }

  int brightness = server.arg("value").toInt();
  brightness = constrain(brightness, 0, 255);

  strip.setBrightness(brightness);
  strip.show();

  server.send(200, "text/plain", "bright");
}

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
}

void ensureWiFiConnection() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  unsigned long now = millis();
  if (now - lastReconnectAttemptMs < 10000) {
    return;
  }

  lastReconnectAttemptMs = now;
  Serial.println("Wi-Fi disconnected. Reconnecting...");
  WiFi.disconnect();
  WiFi.begin(ssid, password);
}



// SETUP


void setup(){
  Serial.begin(115200);
  strip.begin();
  strip.show();
  connectWiFi();
  ArduinoOTA.begin();

  server.on("/", handleRoot);
  server.on("/bright", handleBright);
  server.on("/apply", handleApply);
  server.begin();
}

// LOOP

void loop(){
  ensureWiFiConnection();
  ArduinoOTA.handle();
  server.handleClient();
}
