# Ericofon wiring – v0.1

## Power
- ESP32 powered via USB (5V)
- MAX98357A powered from ESP32 5V pin

## I2S Audio
ESP32 → MAX98357A
- GPIO26 → BCLK
- GPIO25 → LRC
- GPIO22 → DIN

## Amplifier control
- SD → 3V3 (amplifier enabled)
- GAIN → GND (low noise)

## Speaker
- SPK+ → Ericofon speaker
- SPK− → Ericofon speaker
⚠️ SPK− must NOT be connected to GND

## Notes
- Original Ericofon speaker (~8Ω) works correctly
- SD tied to GND will completely mute audio
