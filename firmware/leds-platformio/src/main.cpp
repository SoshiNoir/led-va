#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoOTA.h>
#include <Arduino.h>

const char* ssid = "asdf";
const char* password = "rockroll";

#define LED_PIN D1
#define NUM_LEDS 300

ESP8266WebServer server(80);
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// =======================
// ESTADO GLOBAL
// =======================

int r1, g1, b1;
int r2, g2, b2;
int r3, g3, b3;

int animationOffset = 0;
unsigned long lastUpdateMs = 0;
int globalWaitMs = 50;


// =======================
// BASE
// =======================

void setColor(int r, int g, int b){
  for(int i = 0; i < NUM_LEDS; i++){
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}

// HANDLERS


void handleRoot(){
  server.send(200, "text/plain", "ESP8266 online");
}

void handleRed(){ setColor(255,0,0); server.send(200,"text/plain","red"); }
void handleGreen(){ setColor(0,255,0); server.send(200,"text/plain","green"); }
void handleBlue(){ setColor(0,0,255); server.send(200,"text/plain","blue"); }
void handleOff(){ setColor(0,0,0); server.send(200,"text/plain","off"); }
void handleWhite(){ setColor(255,255,255); server.send(200,"text/plain","white"); }
void handleYellow(){ setColor(255,255,0); server.send(200,"text/plain","yellow"); }
void handlePurple(){ setColor(128,0,128); server.send(200,"text/plain","purple"); }
void handleCyan(){ setColor(0,255,255); server.send(200,"text/plain","cyan"); }
void handleOrange(){ setColor(255,165,0); server.send(200,"text/plain","orange"); }
void handlePink(){ setColor(255,20,147); server.send(200,"text/plain","pink"); }
void handleGold(){ setColor(255,215,0); server.send(200,"text/plain","gold"); }
void handleLavender(){ setColor(126,21,255); server.send(200,"text/plain","lavender"); }
void handleIceBlue(){ setColor(120,200,255); server.send(200,"text/plain","iceblue"); }
void handleFire(){ setColor(228, 138, 0); server.send(200,"text/plain","fire"); }

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



// SETUP


void setup(){
  Serial.begin(115200);

  strip.begin();
  strip.show();

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
  }

  ArduinoOTA.begin();

  server.on("/", handleRoot);

  server.on("/bright", handleBright);

  server.on("/red", handleRed);
  server.on("/green", handleGreen);
  server.on("/blue", handleBlue);
  server.on("/off", handleOff);

  server.on("/white", handleWhite);
  server.on("/yellow", handleYellow);
  server.on("/purple", handlePurple);
  server.on("/cyan", handleCyan);
  server.on("/orange", handleOrange);

  server.on("/pink", handlePink);
  server.on("/gold", handleGold);
  server.on("/lavender", handleLavender);
  server.on("/iceblue", handleIceBlue);
  server.on("/fire", handleFire);

  server.begin();
}


// LOOP


void loop(){
  ArduinoOTA.handle();
  server.handleClient();
}
