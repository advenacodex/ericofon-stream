# Design notes

## Audio output
- ESP32 DevKit V1
- MAX98357A powered at 5V from ESP32
- SD pin must be HIGH (3V3) to enable amplifier
- GAIN tied to GND for low noise

## Known pitfalls
- SD tied to GND = no sound
- SPK- must NOT go to GND
- Original Ericofon speaker is ~8Ω and works correctly