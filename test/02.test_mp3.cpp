#include <Arduino.h>
#include "Audio.h"
#include "LittleFS.h"

#define I2S_DOUT 22
#define I2S_BCLK 26
#define I2S_LRC 25

Audio audio;

void audio_info(const char *info){
  Serial.print("INFO: "); 
  Serial.println(info);
}

void audio_eof_mp3(const char *info){
  Serial.print("FIN: "); 
  Serial.println(info);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  if(!LittleFS.begin()){
    Serial.println("ERROR: No se pudo montar LittleFS");
    while(1);
  }
  Serial.println("LittleFS montado");
  
  File file = LittleFS.open("/dial_tone.mp3");
  if(!file){
    Serial.println("ERROR: dial_tone.mp3 no encontrado");
    while(1);
  } else {
    Serial.printf("Archivo: %d bytes\n", file.size());
    file.close();
  }
  
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(21);
  audio.connecttoFS(LittleFS, "/dial_tone.mp3");
  Serial.println("Reproduciendo...");
}

void loop() {
  audio.loop();
}