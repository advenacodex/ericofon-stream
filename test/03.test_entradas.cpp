// ==========================
// TEST GANCHO Y PULSADOR
// ==========================

#include <Arduino.h>

#define PIN_GANCHO 32
#define PIN_DISCO  33

void setup() {
  Serial.begin(115200);
  
  pinMode(PIN_GANCHO, INPUT_PULLUP);
  pinMode(PIN_DISCO, INPUT_PULLUP);
  
  Serial.println("Test Gancho y Pulsador iniciado");
  Serial.println("Gancho: GPIO32 | Pulsador: GPIO33");
}

void loop() {
  static bool estadoGanchoAnterior = HIGH;
  static bool estadoDiscoAnterior = HIGH;
  
  bool estadoGancho = digitalRead(PIN_GANCHO);
  bool estadoDisco = digitalRead(PIN_DISCO);
  
  // Detectar cambio en gancho
  if (estadoGancho != estadoGanchoAnterior) {
    delay(50); // Debounce
    estadoGancho = digitalRead(PIN_GANCHO);
    
    if (estadoGancho == LOW) {
      Serial.println(">>> GANCHO DESCOLGADO");
    } else {
      Serial.println("<<< GANCHO COLGADO");
    }
    estadoGanchoAnterior = estadoGancho;
  }
  
  // Detectar pulso en disco
  if (estadoDisco != estadoDiscoAnterior) {
    delay(10); // Debounce corto para pulsos
    estadoDisco = digitalRead(PIN_DISCO);
    
    if (estadoDisco == LOW) {
      Serial.println("PULSO DISCO");
    }
    estadoDiscoAnterior = estadoDisco;
  }
  
  delay(10);
}
