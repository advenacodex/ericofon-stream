// ==========================
// ERICOFONO - Main Code
// ==========================

#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include "Audio.h"
#include "secrets.h"

// Pines I2S
#define I2S_DOUT 22
#define I2S_BCLK 26
#define I2S_LRC  25

// Pines entradas
#define PIN_GANCHO 32
#define PIN_DISCO  33

// Audio object
Audio audio;

// Estados de la máquina de estados
enum Estado {
  IDLE,           // Colgado, esperando
  DIAL_TONE,      // Descolgado, tono de línea
  COUNTING,       // Contando pulsos
  PLAYING         // Reproduciendo música
};

Estado estadoActual = IDLE;

// Variables para conteo de pulsos
int contadorPulsos = 0;
unsigned long tiempoUltimoPulso = 0;
unsigned long tiempoInicioConteo = 0;
const unsigned long TIMEOUT_PULSOS = 5000; // 5 segundos
bool pulsoAnterior = HIGH;

// Debounce
unsigned long ultimoDebounceGancho = 0;
unsigned long ultimoDebouncedisco = 0;
const unsigned long DEBOUNCE_GANCHO = 50;
const unsigned long DEBOUNCE_DISCO = 10;

bool ganchoAnterior = HIGH;

// Declaración adelantada
String construirURLNavidrome(int numero);

void setup() {
  Serial.begin(115200);
  
  // Configurar pines
  pinMode(PIN_GANCHO, INPUT_PULLUP);
  pinMode(PIN_DISCO, INPUT_PULLUP);
  
  // Iniciar LittleFS
  if (!LittleFS.begin(true)) {
    Serial.println("Error montando LittleFS");
    return;
  }
  Serial.println("LittleFS montado correctamente");
  
  // Conectar WiFi con configuración optimizada
  Serial.println("Conectando WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false); // Sin ahorro energía
  WiFi.setTxPower(WIFI_POWER_19_5dBm); // Máxima potencia
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("RSSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");

  // Configurar audio optimizado para streaming
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(15); // 0-21
  audio.setConnectionTimeout(15000, 15000); // 15 segundos
  audio.setBufsize(32768, 0); // Buffer grande 32KB
  
  Serial.println("Ericofono iniciado");
}

void loop() {
  audio.loop(); // CRÍTICO: ejecutar lo más frecuente posible
  
  // Leer estado del gancho con debounce
  bool ganchoActual = digitalRead(PIN_GANCHO);
  
  if (ganchoActual != ganchoAnterior) {
    if (millis() - ultimoDebounceGancho > DEBOUNCE_GANCHO) {
      ultimoDebounceGancho = millis();
      
      // Detectar descolgado (HIGH -> LOW)
      if (ganchoAnterior == HIGH && ganchoActual == LOW) {
        Serial.println(">>> GANCHO DESCOLGADO");
        estadoActual = DIAL_TONE;
        audio.connecttoFS(LittleFS, "/dial_tone.mp3");
        Serial.println("Reproduciendo tono de línea...");
      }
      
      // Detectar colgado (LOW -> HIGH)
      if (ganchoAnterior == LOW && ganchoActual == HIGH) {
        Serial.println("<<< GANCHO COLGADO");
        estadoActual = IDLE;
        audio.stopSong();
        contadorPulsos = 0;
        Serial.println("Sistema en reposo");
      }
      
      ganchoAnterior = ganchoActual;
    }
  }
  
  // Lógica según estado actual
  if (estadoActual == DIAL_TONE || estadoActual == COUNTING) {
    // Leer pulsos del disco
    bool discoActual = digitalRead(PIN_DISCO);
    
    if (discoActual != pulsoAnterior) {
      if (millis() - ultimoDebouncedisco > DEBOUNCE_DISCO) {
        ultimoDebouncedisco = millis();
        
        // Detectar flanco descendente (pulso)
        if (pulsoAnterior == HIGH && discoActual == LOW) {
          
          // Primer pulso: pasar a modo conteo
          if (estadoActual == DIAL_TONE) {
            audio.stopSong();
            estadoActual = COUNTING;
            tiempoInicioConteo = millis();
            Serial.println("Iniciando conteo de pulsos...");
          }
          
          contadorPulsos++;
          tiempoUltimoPulso = millis();
          
          // Sonido de click (reproducir archivo corto)
          audio.connecttoFS(LittleFS, "/click.mp3");
          Serial.print("PULSO ");
          Serial.println(contadorPulsos);
        }
        
        pulsoAnterior = discoActual;
      }
    }
    
    // Timeout: terminar conteo y reproducir
    if (estadoActual == COUNTING) {
      if (millis() - tiempoInicioConteo > TIMEOUT_PULSOS) {
        Serial.print("Número marcado: ");
        Serial.println(contadorPulsos);
        
        // Construir URL de Navidrome (Subsonic API)
        String playlistUrl = construirURLNavidrome(contadorPulsos);
        
        if (playlistUrl != "") {
          Serial.print("Reproduciendo: ");
          Serial.println(playlistUrl);
          audio.connecttohost(playlistUrl.c_str());
          estadoActual = PLAYING;
        } else {
          Serial.println("Error: número no válido");
          estadoActual = DIAL_TONE;
          audio.connecttoFS(LittleFS, "/dial_tone.mp3");
        }
        
        contadorPulsos = 0;
      }
    }
  }
  
  // Delay mínimo para maximizar llamadas a audio.loop()
  delay(1);
}

// Construir URL de Navidrome usando Subsonic API
String construirURLNavidrome(int numero) {
  if (numero < 1 || numero > 9) {
    return "";
  }
  
  String songId = "X4csrjhPJh4z5fnbrwU9BP";
  
  // URL-encode de contraseña
  String passEncoded = String(NAVIDROME_PASS);
  passEncoded.replace("$", "%24");
  passEncoded.replace("@", "%40");
  
  // Añadir parámetro format y maxBitRate para reducir calidad si es necesario
  String streamUrl = String(NAVIDROME_URL) + "/rest/stream";
  streamUrl += "?id=" + songId;
  streamUrl += "&u=" + String(NAVIDROME_USER);
  streamUrl += "&p=" + passEncoded;
  streamUrl += "&v=1.16.1";
  streamUrl += "&c=ericofono";
  streamUrl += "&format=mp3"; // Forzar MP3
  streamUrl += "&maxBitRate=128"; // Limitar a 128kbps para streaming más fluido
  
  Serial.print("URL generada: ");
  Serial.println(streamUrl);
  
  return streamUrl;
}

// Callbacks opcionales de Audio
void audio_info(const char *info) {
  Serial.print("audio_info: ");
  Serial.println(info);
}

void audio_eof_mp3(const char *info) {
  Serial.println("Fin de reproducción MP3");
  if (estadoActual == PLAYING) {
    Serial.println("Reproducción finalizada");
  }
}
