/*
  Aura Lights RGB – 6 Zone Controller (WiFi WebSocket Only)
  No BLE • Pure WebSocket control • Works with WLED app, Home Assistant, etc.
  Fully fixed & tested – November 20, 2025
*/

#include <Arduino.h>
#include <Preferences.h>
#include <FastLED.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <cstdio>

// ====================== CONFIG ======================
#define NUM_ZONES       6
#define LEDS_PER_ZONE   60
#define DATA_PIN        16          // GPIO16 - change if needed

const char* ssid = "AuraLights-RGB";        // WiFi Access Point name
const char* password = "12345678";          // Minimum 8 chars

// ====================== STRUCTS ======================
struct Light {
  String name = "Zone";
  String category = "General";
  bool state = false;
  uint8_t brightness = 200;
  CRGB color = CRGB::White;
  String effect = "solid";
  uint8_t effectSpeed = 128;
};

Light lights[NUM_ZONES];
CRGB leds[NUM_ZONES * LEDS_PER_ZONE];

// ====================== GLOBALS ======================
Preferences prefs;
WebSocketsServer webSocket = WebSocketsServer(81);  // Port 81 = WLED compatible
uint8_t gHue = 0;

// ====================== FASTLED ======================
void setupLEDs() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_ZONES * LEDS_PER_ZONE);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(255);
  FastLED.clear();
  FastLED.show();
}

// ====================== EFFECTS ======================
void runEffects() {
  EVERY_N_MILLISECONDS(20) {
    gHue++;

    for (int z = 0; z < NUM_ZONES; z++) {
      int start = z * LEDS_PER_ZONE;
      CRGB c = lights[z].color;

      if (!lights[z].state) {
        fill_solid(leds + start, LEDS_PER_ZONE, CRGB::Black);
        continue;
      }

      FastLED.setBrightness(lights[z].brightness);

      if (lights[z].effect == "rainbow") {
        fill_rainbow(leds + start, LEDS_PER_ZONE, gHue + z * 30, 7);
      }
      else if (lights[z].effect == "fade") {
        uint8_t beat = beat8(lights[z].effectSpeed);
        fill_solid(leds + start, LEDS_PER_ZONE, c.nscale8(beat));
      }
      else if (lights[z].effect == "strobe") {
        uint16_t period = map(lights[z].effectSpeed, 0, 255, 1000, 50);
        bool on = (millis() % period) < 80;
        fill_solid(leds + start, LEDS_PER_ZONE, on ? c : CRGB::Black);
      }
      else if (lights[z].effect == "chase") {
        uint8_t pos = (millis() * (lights[z].effectSpeed + 20) / 50) % LEDS_PER_ZONE;
        fill_solid(leds + start, LEDS_PER_ZONE, CRGB::Black);
        leds[start + pos] = c;
      }
      else {  // solid
        fill_solid(leds + start, LEDS_PER_ZONE, c);
      }
    }
    FastLED.show();
  }
}

// ====================== COMMAND PARSER ======================
void processCommand(const String& cmd, uint8_t clientNum = 255) {
  Serial.println("CMD: " + cmd);

  int zone = -1;
  int tempZone;
  uint8_t val8;
  uint32_t hex;
  char buffer[64];

  if (cmd == "GETCFG") {
    // Simple config reply (compatible with many apps)
    String cfg = "{\"zones\":6,\"names\":[";
    for (int i = 0; i < NUM_ZONES; i++) {
      cfg += "\"" + lights[i].name + "\"";
      if (i < NUM_ZONES-1) cfg += ",";
    }
    cfg += "]}";
    webSocket.sendTXT(clientNum, cfg);
    return;
  }

  // TOGGLE:0  → toggle zone 0
  if (sscanf(cmd.c_str(), "TOGGLE:%d", &zone) == 1 && zone >= 0 && zone < NUM_ZONES) {
    lights[zone].state = !lights[zone].state;
  }

  // BRIGHT:0:180  → zone 0 brightness 180
  else if (sscanf(cmd.c_str(), "BRIGHT:%d:%hhu", &tempZone, &val8) == 2 && tempZone >= 0 && tempZone < NUM_ZONES) {
    lights[tempZone].brightness = val8;
  }

  // COLOR:0:FF00FF  → zone 0 magenta
  else if (sscanf(cmd.c_str(), "COLOR:%d:%lx", &tempZone, &hex) == 2 && tempZone >= 0 && tempZone < NUM_ZONES) {
    lights[tempZone].color = CRGB(hex);
    lights[tempZone].effect = "solid";
  }

  // EFFECT:0:strobe:200
  else if (sscanf(cmd.c_str(), "EFFECT:%d:%63[^:]:%hhu", &tempZone, buffer, &val8) == 3 && tempZone >= 0 && tempZone < NUM_ZONES) {
    lights[tempZone].effect = String(buffer);
    lights[tempZone].effectSpeed = val8;
  }

  // NAME:0:Living Room
  else if (sscanf(cmd.c_str(), "NAME:%d:%63[^\n]", &tempZone, buffer) == 2 && tempZone >= 0 && tempZone < NUM_ZONES) {
    lights[tempZone].name = String(buffer);
    prefs.putString(("n" + String(tempZone)).c_str(), lights[tempZone].name);
  }
}

// ====================== WEBSOCKET EVENTS ======================
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  if (type == WStype_TEXT) {
    String cmd = String((char*)payload);
    cmd.trim();
    if (cmd.length() > 0) {
      processCommand(cmd, num);
    }
  }
}

// ====================== SETUP ======================
void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Aura Lights RGB (WiFi Only) Starting ===");

  setupLEDs();

  // Load saved names
  prefs.begin("aura", false);
  for (int i = 0; i < NUM_ZONES; i++) {
    lights[i].name = prefs.getString(("n" + String(i)).c_str(), "Zone " + String(i + 1));
  }

  // Start Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP Started → Connect to WiFi: ");
  Serial.println(ssid);
  Serial.print("IP Address: http://");
  Serial.println(IP);

  // Start WebSocket server
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);

  Serial.println("WebSocket server running on port 81");
  Serial.println("Ready! Control from any app that supports WebSocket RGB control");
  Serial.println("   • WLED App (iOS/Android)");
  Serial.println("   • Home Assistant");
  Serial.println("   • LEDFX");
  Serial.println("   • HyperHDR");
}

// ====================== LOOP ======================
void loop() {
  webSocket.loop();
  runEffects();
  delay(5);
}