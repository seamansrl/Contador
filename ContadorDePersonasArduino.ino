/*
 * Person Counter – Arduino Nano + HC‑SR04 (v7)
 * ------------------------------------------------
 * ✔ Cuenta personas por variación (≥30 % de la línea base).
 * ✔ LED indicador en D4 se enciende mientras el haz está ocupado.
 * ✔ Comandos por Serial (115 200 baud):
 *     • RST   → pone contador a 0 y lo guarda en EEPROM.
 *     • COUNT → imprime el valor y PAUSA el conteo hasta reinicio.
 *
 * HARDWARE
 *   HC‑SR04  – Vcc 5 V, GND, TRIG = D8, ECHO = D9 (5 V‑tolerante).
 *   LED       – D04.
 *
 * AUTOR: Fernando Maniglia
 */

#include <EEPROM.h>

// ─────────── Pines ──────────────────────────────────────────────────────────
const byte TRIG_PIN = 8;
const byte ECHO_PIN = 9;
const byte LED_PIN  = 4; // LED integrado del Nano

// ─────────── Parámetros de lectura ─────────────────────────────────────────
const uint16_t MAX_CM      = 600;
const uint16_t MIN_CM      = 5;
const byte     FILTER_SIZE = 3;   // mediana de 3

// ─────────── Lógica de detección ───────────────────────────────────────────
const float VAR_RATIO   = 0.30;          // 30 %
const float CLEAR_RATIO = 0.12;          // ±12 %
const byte  CLEAR_SEQ   = 3;
const unsigned long TIMEOUT = 2500UL;    // ms OCCUPIED máx
const float BASE_ALPHA  = 0.003;         // EMA lento

// ─────────── Persistencia ──────────────────────────────────────────────────
const uint16_t EE_MAGIC_ADDR = 0;
const uint16_t EE_COUNT_ADDR = 2;
const uint16_t MAGIC         = 0xA5A5;

// ─────────── Globals ───────────────────────────────────────────────────────
unsigned long peopleCount = 0;
float         baseline    = NAN;
bool          occupied    = false;
byte          clearCount  = 0;
unsigned long triggeredTS = 0;
bool          paused      = false; // true después de comando COUNT
const unsigned long VENTANA_MS = 30000; 

// ─────────── Prototipos ────────────────────────────────────────────────────
uint32_t pingUS();
float     medianDistance();
void      updateBaseline(float d);
void      saveCounter();
void      loadCounter();
void      checkSerial();

// ─────────── Setup ─────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);

  loadCounter();
  Serial.print(F("Contador restaurado: ")); Serial.println(peopleCount);

// ─────────── Espera a PC ─────────────────────────────────────────────────────
  unsigned long tTimer = millis();
  while (millis() - tTimer < VENTANA_MS) {

    // parpadeo cada 250 ms sin bloquear
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink >= 250) {
      lastBlink = millis();
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }

    // ¿llegó "COUNT"?
    if (Serial.available()) {
      String cmd = Serial.readStringUntil('\n');
      cmd.trim();
      cmd.toUpperCase();
      if (cmd == "COUNT") {
        Serial.print(F("COUNT:")); Serial.println(peopleCount);
        paused = true;
        break;
      }
    }
  }
  digitalWrite(LED_PIN, LOW);

  // ─────────── Calibración inicial ──────────────────────────────────────────────
  if (!paused)
  {
    Serial.println(F("Calibrando línea base…"));
    float sum = 0; uint8_t n = 0;
    const unsigned long t0 = millis();
    while (millis() - t0 < 2500) 
    {
      float d = medianDistance();
      if (!isnan(d)) 
        { 
          sum += d; n++; 
        }
      delay(40);
    }
    if (n) baseline = sum / n;
    Serial.print(F("Base = ")); Serial.print(baseline, 1); Serial.println(F(" cm"));
  }
}

// ─────────── Loop ──────────────────────────────────────────────────────────
void loop() {
  checkSerial();
  if (paused) { delay(50); return; }

  float dist = medianDistance();
  if (isnan(dist)) { delay(20); return; }

  float delta = fabs(dist - baseline);
  float varThresh   = baseline * VAR_RATIO;
  float clearThresh = baseline * CLEAR_RATIO;

  if (!occupied && delta >= varThresh) {
    peopleCount++; saveCounter();
    Serial.print(F("Personas = ")); Serial.println(peopleCount);
    occupied    = true;
    triggeredTS = millis();
    clearCount  = 0;
  }

  if (occupied) {
    if (delta < clearThresh) {
      if (++clearCount >= CLEAR_SEQ) {
        occupied = false;
        Serial.println(F("Rearmado (CLEAR_SEQ)"));
      }
    } else {
      clearCount = 0;
    }
    if (millis() - triggeredTS > TIMEOUT) {
      occupied = false;
      Serial.println(F("Rearmado (timeout)"));
    }
  }

  digitalWrite(LED_PIN, occupied ? HIGH : LOW);
  updateBaseline(dist);
  delay(20); // ≈30 Hz
}

// ─────────── Serial commands ───────────────────────────────────────────────
void checkSerial() {
  if (!Serial.available()) return;
  String cmd = Serial.readStringUntil('\n');
  cmd.trim(); cmd.toUpperCase();
  if (cmd == F("RST")) {
    peopleCount = 0; saveCounter();
    Serial.println(F("RST:OK"));
  } else if (cmd == F("COUNT")) {
    Serial.print(F("COUNT:")); Serial.println(peopleCount);
    paused = true;
  }
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
  for (byte i = 0; i < FILTER_SIZE; i++) {
    uint32_t us = pingUS();
    if (!us) return NAN;
    float cm = us / 58.0;
    if (cm < MIN_CM || cm > MAX_CM) return NAN;
    v[i] = (uint32_t)(cm * 100);
  }
  if (v[0] > v[1]) { uint32_t t=v[0]; v[0]=v[1]; v[1]=t; }
  if (v[1] > v[2]) { uint32_t t=v[1]; v[1]=v[2]; v[2]=t; if (v[0] > v[1]) { t=v[0]; v[0]=v[1]; v[1]=t; }}
  return v[1] / 100.0;
}

// ─────────── Línea base (EMA) ──────────────────────────────────────────────
void updateBaseline(float d) {
  if (!occupied && fabs(d - baseline) < baseline * CLEAR_RATIO) {
    baseline = baseline * (1.0 - BASE_ALPHA) + d * BASE_ALPHA;
  }
}

// ─────────── EEPROM helpers ────────────────────────────────────────────────
void saveCounter() {
  EEPROM.put(EE_MAGIC_ADDR, MAGIC);
  EEPROM.put(EE_COUNT_ADDR, peopleCount);
}

void loadCounter() {
  uint16_t m; EEPROM.get(EE_MAGIC_ADDR, m);
  if (m == MAGIC) EEPROM.get(EE_COUNT_ADDR, peopleCount);
}
