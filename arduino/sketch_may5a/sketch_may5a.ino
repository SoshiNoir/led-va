#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoOTA.h>

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

int offset_anim = 0;
unsigned long lastUpdate = 0;
int waitMs_global = 50;

bool trianim_ativo = false;

// =======================
// BASE
// =======================

void setColor(int r, int g, int b){
  trianim_ativo = false; // 👈 para animação
  for(int i = 0; i < NUM_LEDS; i++){
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}

// =======================
// ANIMAÇÃO
// =======================

void startTrianim(int waitMs){
  waitMs_global = waitMs;
  trianim_ativo = true;

  r1 = random(0,256); g1 = random(0,256); b1 = random(0,256);
  r2 = random(0,256); g2 = random(0,256); b2 = random(0,256);
  r3 = random(0,256); g3 = random(0,256); b3 = random(0,256);
}

void updateTrianim(){

  if (!trianim_ativo) return;

  if (millis() - lastUpdate < waitMs_global) return;
  lastUpdate = millis();

  int section = NUM_LEDS / 3;

  if (random(0,100) < 5){
    r1 = random(0,256); g1 = random(0,256); b1 = random(0,256);
    r2 = random(0,256); g2 = random(0,256); b2 = random(0,256);
    r3 = random(0,256); g3 = random(0,256); b3 = random(0,256);
  }

  for(int i = 0; i < NUM_LEDS; i++){

    int pos = (i + offset_anim) % NUM_LEDS;

    if(pos < section){
      strip.setPixelColor(i, strip.Color(r1, g1, b1));
    }
    else if(pos < section * 2){
      strip.setPixelColor(i, strip.Color(r2, g2, b2));
    }
    else{
      strip.setPixelColor(i, strip.Color(r3, g3, b3));
    }
  }

  strip.show();

  offset_anim++;
  if(offset_anim >= NUM_LEDS) offset_anim = 0;
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

void handleTriAnim(){
  startTrianim(30);
  server.send(200, "text/plain", "trianim on");
}

void handleGradient(){
  trianim_ativo = false;

  for(int i = 0; i < NUM_LEDS; i++){
    float ratio = (float)i / (NUM_LEDS - 1);

    int r = 255 * (1 - ratio);
    int b = 255 * ratio;

    strip.setPixelColor(i, strip.Color(r, 0, b));
  }

  strip.show();
  server.send(200, "text/plain", "gradient");
}


// SETUP


void setup(){
  Serial.begin(115200);

  strip.begin();
  strip.setBrightness(80);
  strip.show();

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
  }

  ArduinoOTA.begin();

  server.on("/", handleRoot);

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


//animaçoes
  server.on("/grade", handleGradient);
  server.on("/vamos", handleTriAnim);
  server.begin();
}


// LOOP


void loop(){
  ArduinoOTA.handle();
  server.handleClient();
  updateTrianim(); // 👈 roda a animação SEM travar
}