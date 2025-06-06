/*
 * Person Counter – ESP8266 + HC‑SR04  (v6.2 – desbloqueo automático)
 * 
 * HARDWARE
 *   HC‑SR04  – Vcc 5 V, GND, TRIG = D5, ECHO = D6 (5 V‑tolerante).
 *   LED       – D04.
 *
 * AUTOR: Fernando Maniglia
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <math.h>

// ─────────── Pines ──────────────────────────────────────────────────────────
const uint8_t TRIG_PIN = D5;
const uint8_t ECHO_PIN = D6;   // divisor 1 k + 2 k   → 3 V3

// ─────────── Parámetros de lectura ─────────────────────────────────────────
const uint16_t MAX_SAMPLE_CM = 600;
const uint16_t MIN_SAMPLE_CM = 5;
const uint8_t  FILTER_SIZE   = 3;   // mediana de 3 → rápido

// ─────────── Lógica basada en variaciones ──────────────────────────────────
const float   VAR_RATIO          = 0.30; // 30 % para disparo
const float   CLEAR_RATIO        = 0.12; // ±12 % para regresar
const uint8_t CLEAR_SEQ          = 3;    // lecturas necesarias
const uint32_t OCCUPIED_TIMEOUT_MS = 2500; // 2,5 s máx en OCCUPADO
const float   BASE_ALPHA         = 0.003;// 0,3 % EMA (lento)

// ─────────── Persistencia ──────────────────────────────────────────────────
const uint32_t EEPROM_MAGIC = 0xFADEBEEF;
const uint32_t EEPROM_ADDR  = 0;  // magic + counter

// ─────────── Wi‑Fi SoftAP ──────────────────────────────────────────────────
const char* AP_SSID     = "PersonCounter_AP";
const char* AP_PASSWORD = "12345678";

// ─────────── Globals ───────────────────────────────────────────────────────
ESP8266WebServer server(80);
uint32_t peopleCount = 0;
float    baseline    = NAN;
bool     triggered   = false;
uint8_t  clearCount  = 0;
uint32_t triggeredTS = 0;

// ─────────── Prototipos ────────────────────────────────────────────────────
uint32_t pingUS();
float    medianDistance();
void     updateBaseline(float d);
void     saveCounter();
void     loadCounter();
void     setupSoftAP();
void     handleRoot();
void     handleReset();
void     handleBaseline();
void     recalibrateBaseline();
String   htmlPage();

// ─────────── Setup ─────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  EEPROM.begin(16);
  loadCounter();

  setupSoftAP();
  server.on("/", handleRoot);
  server.on("/reset", handleReset);
  server.on("/baseline", handleBaseline);
  server.begin();
  Serial.println("HTTP listo (80)");

  Serial.println("Calibrando línea base…");
  recalibrateBaseline();
  Serial.printf("Base inicial = %.1f cm\n", baseline);
}

// ─────────── Loop ──────────────────────────────────────────────────────────
void loop() {
  server.handleClient();

  float dist = medianDistance();
  if (isnan(dist)) { delay(20); return; }

  float delta = fabs(dist - baseline);
  float varThresh   = baseline * VAR_RATIO;
  float clearThresh = baseline * CLEAR_RATIO;

  if (!triggered && delta >= varThresh) {
    peopleCount++; saveCounter();
    Serial.printf("Persona #%u – Δ=%.1f cm (th %.1f)\n", peopleCount, delta, varThresh);
    triggered   = true;
    triggeredTS = millis();
    clearCount  = 0;
  }

  if (triggered) {
    bool cleared = (delta < clearThresh);
    if (cleared) {
      if (++clearCount >= CLEAR_SEQ) {
        triggered = false;
        Serial.println("Rearmado por CLEAR_SEQ");
      }
    } else {
      clearCount = 0;
    }
    // Timeout de seguridad
    if (millis() - triggeredTS > OCCUPIED_TIMEOUT_MS) {
      triggered = false;
      Serial.println("Rearmado por timeout");
    }
  }

  updateBaseline(dist);
  delay(20); // ≈30 Hz
}

// ─────────── Baseline manual ───────────────────────────────────────────────
void recalibrateBaseline() {
  float sum = 0; uint8_t n = 0;
  for (uint8_t i = 0; i < 15; i++) {
    float d = medianDistance();
    if (!isnan(d)) { sum += d; n++; }
    delay(30);
  }
  if (n) baseline = sum / n;
  triggered   = false;
  clearCount  = 0;
  triggeredTS = 0;
}

void handleBaseline() {
  recalibrateBaseline();
  Serial.printf("Nueva línea base = %.1f cm\n", baseline);
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "Baseline reset");
}

// ─────────── HC‑SR04 helpers ───────────────────────────────────────────────
uint32_t pingUS() {
  digitalWrite(TRIG_PIN, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  return pulseIn(ECHO_PIN, HIGH, 20000UL);
}

float medianDistance() {
  uint32_t v[FILTER_SIZE];
  for (uint8_t i = 0; i < FILTER_SIZE; i++) {
    uint32_t us = pingUS();
    if (!us) return NAN;
    float cm = us / 58.0;
    if (cm < MIN_SAMPLE_CM || cm > MAX_SAMPLE_CM) return NAN;
    v[i] = (uint32_t)(cm * 100);
  }
  if (v[0] > v[1]) { uint32_t t=v[0]; v[0]=v[1]; v[1]=t; }
  if (v[1] > v[2]) { uint32_t t=v[1]; v[1]=v[2]; v[2]=t; if (v[0] > v[1]) { t=v[0]; v[0]=v[1]; v[1]=t; }}
  return v[1] / 100.0;
}

// ─────────── Línea base (EMA) ──────────────────────────────────────────────
void updateBaseline(float d) {
  if (!triggered && fabs(d - baseline) < baseline * CLEAR_RATIO) {
    baseline = baseline * (1.0 - BASE_ALPHA) + d * BASE_ALPHA;
  }
}

// ─────────── EEPROM ────────────────────────────────────────────────────────
void saveCounter() {
  EEPROM.put(EEPROM_ADDR, EEPROM_MAGIC);
  EEPROM.put(EEPROM_ADDR + 4, peopleCount);
  EEPROM.commit();
}

void loadCounter() {
  uint32_t m; EEPROM.get(EEPROM_ADDR, m);
  if (m == EEPROM_MAGIC) EEPROM.get(EEPROM_ADDR + 4, peopleCount);
}

// ─────────── SoftAP ────────────────────────────────────────────────────────
void setupSoftAP() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  delay(100);
  Serial.printf("SoftAP %s  IP %s\n", AP_SSID, WiFi.softAPIP().toString().c_str());
}

// ─────────── Web UI ────────────────────────────────────────────────────────
String htmlPage() {
  unsigned long ms = triggered ? OCCUPIED_TIMEOUT_MS - min<unsigned long>(OCCUPIED_TIMEOUT_MS, millis() - triggeredTS) : 0;
  String h = F("<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'><title>Contador</title><style>body{font-family:sans-serif;text-align:center;padding:2rem;}h1{font-size:3rem;}button{padding:.6rem 1.2rem;font-size:1rem;margin:.4rem;}</style><meta http-equiv='refresh' content='3'></head><body>");
  h += F("<h1>"); h += peopleCount; h += F("</h1><p>Personas contadas</p>");
  h += F("<p>Línea base: "); h += String(baseline, 1); h += F(" cm</p>");
  h += F("<p>Estado: "); h += (triggered ? "OCUPADO" : "LIBRE"); if(triggered){ h += F(" (Timeout "); h += String(ms/1000.0,1); h += F(" s)");} h += F("</p>");
  h += F("<p><a href='/reset'><button>Resetear contador</button></a></p>");
  h += F("<p><a href='/baseline'><button>Recalibrar base</button></a></p>");
  h += F("<p>IP: "); h += WiFi.softAPIP().toString(); h += F("</p></body></html>");
  return h;
}

void handleRoot()  { server.send(200, "text/html", htmlPage()); }
void handleReset() {
  peopleCount = 0; saveCounter();
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "Counter reset");
}
