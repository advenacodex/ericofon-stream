# Ericofono

El Ericófono
El Ericófono (nombre original: Ericofon) fue un teléfono creado por la compañía sueca Ericsson. Desarrollado a finales de la década de 1940 por un equipo que incluía a Gösta Thames, Ralph Lysell y Hugo Blomberg, destacó por estar construido a partir de una única pieza de plástico, siendo considerado un clásico en la historia del diseño de producto en plástico. La producción en serie comenzó en 1954.

Historia y comercialización
Los primeros modelos se vendían únicamente a instituciones, pero en 1956 comenzó la producción para el mercado europeo y australiano. En Suecia fue conocido como "teléfono cobra" debido a su forma semejante a una serpiente.

La Bell Telephone Laboratories inicialmente no permitió su entrada en Estados Unidos, pero rápidamente se convirtió en un best-seller tras su introducción en el mercado estadounidense. En ese momento estaba disponible en 18 colores, aunque la transferencia de producción a North Electric redujo ese número a 8.

Evolución técnica
Los primeros Ericófonos eran puramente mecánicos, mientras que las versiones posteriores incorporaron cada vez más componentes electrónicos. En 1967 se introdujo una versión con teclado y se intentó un rediseño con el modelo 700, que nunca tuvo éxito.

Legado
El Ericófono es considerado una de las piezas de diseño industrial más significativas del siglo XX y se encuentra actualmente en la colección permanente del Museo de Arte Moderno (MoMA) de Nueva York.

{{WIKIPEDIA}}



## Features

- Controlador basado en ESP32
- Audio digital I2S (MAX98357A)
- Altavoz original del Ericofono
- Decodificación de disco rotatorio
- Detección de gancho (hook switch)
- Sistema de archivos LittleFS para almacenamiento de audio
- **Integración con Music Assistant (Home Assistant)**
- **Control de playlists vía webhooks**
- Streaming desde Navidrome (Subsonic API)
- Soporte HTTPS para servidores seguros
- Reproducción MP3 local y remota


## Estado del proyecto

🚀 **v0.5 – Integración Music Assistant completada, control por webhooks funcionando**

## Hardware

### Componentes principales

- ESP32 DevKit V1 (ESP32-D0WD-V3)
- MAX98357A I2S amplifier
- Altavoz original del Ericofono (4-8Ω)
- Disco rotatorio y gancho originales del Ericofono


### Conexiones detalladas

**Audio I2S → MAX98357A:**

- ESP32 GPIO 26 → MAX98357A BCLK
- ESP32 GPIO 25 → MAX98357A LRC (WSEL)
- ESP32 GPIO 22 → MAX98357A DIN
- ESP32 3.3V → MAX98357A VIN
- ESP32 GND → MAX98357A GND
- MAX98357A OUT+ → Altavoz Terminal +
- MAX98357A OUT- → Altavoz Terminal -

Impedancia: 4-8Ω (compatible con altavoz original)
Potencia: 3.2W @ 4Ω

**Gancho telefónico (Hook Switch):**

- Switch Terminal 1 → ESP32 GPIO 32 (INPUT_PULLUP)
- Switch Terminal 2 → ESP32 GND

Funcionamiento:

- Gancho colgado → Contacto abierto → GPIO 32 = HIGH (pull-up interno ~45kΩ)
- Gancho descolgado → Contacto cerrado → GPIO 32 = LOW (conectado a GND)
- Debounce por software: 50ms

**Disco rotatorio (Rotary Dial Pulse):**

- Pulsador Terminal 1 → ESP32 GPIO 33 (INPUT_PULLUP)
- Pulsador Terminal 2 → ESP32 GND

Funcionamiento:

- Reposo → Contacto abierto → GPIO 33 = HIGH
- Cada pulso → Contacto cierra → GPIO 33 = LOW
- Número marcado = cantidad de pulsos (ej: 5 = cinco pulsos)
- Frecuencia típica: ~10 Hz
- Debounce por software: 10ms

**Alimentación general:**

- ESP32: USB 5V → regulador interno a 3.3V
- MAX98357A: 3.3V desde ESP32 (o 5V externo para mayor potencia)
- Consumo aproximado: 500mA en reproducción activa, 800mA en streaming


### Notas técnicas de conexionado

- Pull-ups internos: El ESP32 tiene resistencias pull-up internas de ~45kΩ, suficientes para switches mecánicos. No se requieren resistencias externas.
- Sin condensadores de debounce: El debounce se implementa por software.
- Cables cortos I2S: Mantener cables BCLK, LRC y DIN < 15cm para evitar interferencias.
- GND común: Todas las masas (ESP32, MAX98357A, switches) deben estar conectadas al mismo punto de referencia.
- WiFi: Señal fuerte recomendada (RSSI > -70 dBm) para streaming estable y webhooks.


## Firmware

### Plataforma y configuración

platformio.ini:

    [env:esp32dev]
    platform = espressif32
    board = esp32dev
    framework = arduino
    board_build.filesystem = littlefs
    board_build.partitions = huge_app.csv
    monitor_speed = 115200
    board_upload.flash_size = 4MB
    lib_deps = 
        esphome/ESP32-audioI2S@^2.3.0
    
### Librerías

- esphome/ESP32-audioI2S@^2.3.0 - Driver I2S y decodificadores de audio con soporte HTTPS
- HTTPClient - Para llamadas a webhooks de Home Assistant
- Driver I2S nativo de ESP32 (driver/i2s.h)
- LittleFS integrado en framework Arduino
- WiFiClientSecure para streaming HTTPS


### Particiones

- Esquema: huge_app.csv - Maximiza espacio para aplicación
- Flash total: 4MB
- LittleFS: Espacio disponible para archivos de audio locales


### Configuración I2S

- Sample rate: 44100 Hz
- Bits per sample: 16 bits
- Channel format: Stereo (LEFT + RIGHT)
- Communication format: I2S standard
- DMA buffers: 32KB para streaming (optimizado)


### Configuración WiFi

- Modo: Station (STA)
- Sleep mode: Desactivado para streaming
- Potencia TX: Máxima (19.5 dBm)
- Timeout de conexión: 15 segundos
- Buffer de audio: 32KB para streaming fluido


## Configuración de credenciales

### Archivo secrets.h

Crear archivo src/secrets.h (NO subir a GitHub):

    #ifndef SECRETS_H
    #define SECRETS_H
    
    // Configuración WiFi
    const char* WIFI_SSID = "TU_RED_WIFI";
    const char* WIFI_PASSWORD = "TU_PASSWORD_WIFI";
    
    // Configuración Navidrome (opcional)
    const char* NAVIDROME_URL = "https://tu-servidor.com";
    const char* NAVIDROME_USER = "tu_usuario";
    const char* NAVIDROME_PASS = "tu_password";
    
    #endif
    Plantilla incluida: src/secrets.example.h (sí se sube a GitHub como referencia)

**Importante:**

- El .gitignore incluye src/secrets.h para proteger credenciales
- La contraseña con caracteres especiales (\$, @, etc.) se codifica automáticamente en el código


## Integración con Music Assistant y Home Assistant

### Arquitectura del sistema

El Ericofono se integra con **Music Assistant** en Home Assistant mediante un sistema de **webhooks HTTP**, permitiendo controlar la reproducción de playlists directamente desde el disco rotatorio sin necesidad de streaming local en el ESP32.

**Ventajas de esta arquitectura:**

- ESP32 actúa como controlador remoto, no como reproductor
- Music Assistant gestiona toda la reproducción de audio
- Escalable: añadir playlists sin modificar firmware
- Sin buffering ni problemas de streaming en el ESP32
- Compatible con cualquier player de Music Assistant


### Sistema de webhooks

**Funcionamiento:**

1. Usuario marca número en disco rotatorio (1-9)
2. ESP32 envía HTTP POST a Home Assistant
3. Home Assistant recibe webhook y ejecuta automatización
4. Music Assistant reproduce playlist en el player configurado
5. ESP32 vuelve a estado IDLE (sin reproducir nada localmente)

**Endpoint:**

    http://IP_HOME_ASSISTANT:8123/api/webhook/ericofono-lista0X-despacho
    **Características:**

- Sin autenticación requerida (webhooks públicos por diseño)
- Compatible con Docker modo network_mode: host
- Timeout HTTP: 5 segundos
- Código respuesta esperado: 200 (OK)


### Configuración en Home Assistant

**Requisitos previos:**

- Home Assistant instalado y accesible en red local
- Music Assistant instalado (addon o integración)
- Al menos un player configurado en Music Assistant
- Playlists creadas en Music Assistant

**Crear automatizaciones:**

Para cada número del disco (1-9), crear una automatización:

**Ejemplo para número 1:**

    alias: "Ericofono - Lista 01 Despacho"
    description: "Reproduce playlist 1 cuando se marca el número 1 en el Ericofono"
    trigger:
      - platform: webhook
        webhook_id: ericofono-lista01-despacho
    action:
      - service: media_player.play_media
        target:
          entity_id: media_player.ma_despacho
        data:
          media_content_id: "media-source://mass/database://playlist/7"
          media_content_type: "playlist"
    mode: single
    **Ejemplo para número 2:**

    alias: "Ericofono - Lista 02 Despacho"
    description: "Reproduce playlist 2 cuando se marca el número 2 en el Ericofono"
    trigger:
      - platform: webhook
        webhook_id: ericofono-lista02-despacho
    action:
      - service: media_player.play_media
        target:
          entity_id: media_player.ma_despacho
        data:
          media_content_id: "media-source://mass/database://playlist/12"
          media_content_type: "playlist"
    mode: single
    **Parámetros importantes:**


| Parámetro | Descripción | Ejemplo |
| :-- | :-- | :-- |
| webhook_id | Identificador único del webhook (debe coincidir con código ESP32) | ericofono-lista01-despacho |
| entity_id | Media player de Music Assistant donde reproducir | media_player.ma_despacho |
| media_content_id | URI de la playlist en Music Assistant | media-source://mass/database://playlist/7 |
| media_content_type | Tipo de contenido (siempre "playlist") | playlist |

**Obtener ID de playlist:**

1. Abrir Music Assistant en navegador: http://IP_HA:8095
2. Ir a **Playlists** → **Library**
3. Abrir la playlist deseada
4. Copiar ID de la URL: http://10.0.1.3:8095/\#/playlists/library/7 → ID = 7
5. Construir URI completo: media-source://mass/database://playlist/7

**Obtener entity_id del player:**

1. En Home Assistant, ir a **Configuración** → **Dispositivos y servicios** → **Entidades**
2. Buscar entidades que comiencen con media_player.ma_
3. Copiar el entity_id completo (ej: media_player.ma_despacho)

Alternativa desde Herramientas de desarrollo:

1. Ir a **Herramientas de desarrollo** → **Estados**
2. Filtrar por media_player
3. Buscar players de Music Assistant (atributo mass_player_id presente)

### Mapeo de números a playlists

El sistema soporta **9 playlists** (números 1-9):


| Número marcado | Webhook ID | Configuración |
| :-- | :-- | :-- |
| 1 | ericofono-lista01-despacho | Playlist configurable |
| 2 | ericofono-lista02-despacho | Playlist configurable |
| 3 | ericofono-lista03-despacho | Playlist configurable |
| 4 | ericofono-lista04-despacho | Playlist configurable |
| 5 | ericofono-lista05-despacho | Playlist configurable |
| 6 | ericofono-lista06-despacho | Playlist configurable |
| 7 | ericofono-lista07-despacho | Playlist configurable |
| 8 | ericofono-lista08-despacho | Playlist configurable |
| 9 | ericofono-lista09-despacho | Playlist configurable |

**Escalabilidad:** Para añadir o modificar playlists, solo editar la automatización en Home Assistant sin tocar el firmware del ESP32.

### Configuración avanzada

**Reproducir en múltiples habitaciones:**

    action:
      - service: media_player.play_media
        target:
          entity_id:
            - media_player.ma_despacho
            - media_player.ma_salon
            - media_player.ma_cocina
        data:
          media_content_id: "media-source://mass/database://playlist/7"
          media_content_type: "playlist"
    **Añadir acciones adicionales (ejemplo: encender luces):**

    action:
      - service: light.turn_on
        target:
          entity_id: light.despacho
        data:
          brightness: 128
      - service: media_player.play_media
        target:
          entity_id: media_player.ma_despacho
        data:
          media_content_id: "media-source://mass/database://playlist/7"
          media_content_type: "playlist"
    **Detener reproducción anterior:**

    action:
      - service: media_player.media_stop
        target:
          entity_id: media_player.ma_despacho
      - delay:
          milliseconds: 500
      - service: media_player.play_media
        target:
          entity_id: media_player.ma_despacho
        data:
          media_content_id: "media-source://mass/database://playlist/7"
          media_content_type: "playlist"
    
### Compatibilidad con Docker

El sistema está diseñado para funcionar con Home Assistant en Docker con network_mode: host.

**Configuración docker-compose.yml:**

    services:
      homeassistant:
        image: ghcr.io/home-assistant/home-assistant:stable
        network_mode: host
        volumes:
          - /path/to/config:/config
        restart: unless-stopped
    **Importante:**

- En modo host, Home Assistant usa directamente la red del host
- Puerto 8123 accesible en la IP de la máquina host (sin mapeo)
- Webhooks disponibles en: http://IP_HOST:8123/api/webhook/...
- No requiere configuración adicional de puertos


## Integración con Navidrome

### API Subsonic

El sistema mantiene soporte para Navidrome (actualmente deshabilitado en v0.5):

**Endpoint de streaming:**

    https://servidor/rest/stream?id=SONG_ID&u=USER&p=PASS&v=1.16.1&c=ericofono&format=mp3&maxBitRate=128
    Parámetros clave:

- id - ID de la canción en Navidrome
- format=mp3 - Fuerza formato MP3
- maxBitRate=128 - Limita bitrate a 128kbps para streaming estable

La función construirURLNavidrome() está mantenida para uso futuro.

## Flujo de funcionamiento

**v0.5 - Con Music Assistant:**

1. **Sistema en reposo (IDLE):** Gancho colgado, esperando.
2. **Descolgar gancho:**
    - Cambia a estado DIAL_TONE
    - Reproduce dial_tone.mp3 desde LittleFS en loop
3. **Marcar número en disco:**
    - Primer pulso: para el tono, cambia a estado COUNTING
    - Cada pulso: reproduce click.mp3 y cuenta
    - Timeout de 5 segundos sin pulsos: procesa número marcado
4. **Ejecutar webhook (números 1-9):**
    - Construye URL del webhook: http://10.0.1.3:8123/api/webhook/ericofono-lista0X-despacho
    - Envía petición HTTP POST a Home Assistant
    - Home Assistant dispara automatización
    - Music Assistant reproduce playlist en el player configurado
    - ESP32 vuelve a estado IDLE (sin reproducir nada localmente)
5. **Colgar gancho:**
    - Para reproducción local si hubiera
    - Vuelve a estado IDLE
    - Resetea contador de pulsos

## Archivos de audio necesarios

**En carpeta /data/ (para LittleFS):**

    /data/
    ├── dial_tone.mp3    # Tono de línea (loop continuo)
    └── click.mp3        # Sonido de pulso del disco (10-50ms)
    **Recomendaciones:**

- dial_tone.mp3: Tono telefónico estándar 425 Hz, mono, 44.1kHz
- click.mp3: Clic corto mecánico, mono, 44.1kHz
- Bitrate: 128kbps MP3 para ambos archivos


## Tests implementados

**Test mínimo de sonido (validado ✅):**

Genera un tono puro de 440 Hz (La) a través del MAX98357A:

- Amplitud: 12000 (~37% del rango para evitar distorsión)
- Salida: Stereo sincronizada
- Ubicación: /test/test_buzzer.cpp

**Test de gancho y pulsador (validado ✅):**

Detecta cambios de estado en gancho y pulsos del disco:

- Salida por Serial Monitor a 115200 baud
- Debounce implementado
- Ubicación: /test/test_inputs.cpp

**Test de streaming Navidrome (validado ✅):**

Sistema completo operativo:

- Streaming HTTPS funcionando
- Bitrate limitado a 128kbps para estabilidad
- Buffer de 32KB optimizado

**Test de webhooks Music Assistant (validado ✅ v0.5):**

Integración completa con Home Assistant:

- Webhooks HTTP POST funcionando
- Compatible con Docker modo host
- Código HTTP 200 confirmado
- Reproducción en Music Assistant operativa


## Estructura del proyecto

    ericofono/
    ├── src/
    │   ├── main.cpp           # Código principal (v0.5 con webhooks)
    │   ├── secrets.h          # Credenciales (NO subir a Git)
    │   └── secrets.example.h  # Plantilla de credenciales
    ├── test/
    │   ├── test_buzzer.cpp    # Test tono 440Hz
    │   └── test_inputs.cpp    # Test gancho y disco
    ├── data/
    │   ├── dial_tone.mp3      # Tono de línea
    │   └── click.mp3          # Sonido de click
    ├── platformio.ini
    ├── .gitignore             # Incluye src/secrets.h
    └── README.md
    
## Roadmap

- [x] Audio output via MAX98357A
- [x] Integración altavoz original
- [x] Sistema de archivos LittleFS
- [x] Reproducción MP3 desde flash
- [x] Test mínimo de sonido (tono 440Hz)
- [x] Conexionado gancho y disco
- [x] Test de entradas digitales
- [x] Máquina de estados para hook switch
- [x] Decodificación de disco rotatorio (contador de pulsos)
- [x] Integración streaming Navidrome (HTTPS)
- [x] Reproducción de tonos telefónicos (dial tone, click)
- [x] Optimización de buffers para streaming estable
- [x] **Integración Music Assistant vía webhooks**
- [x] **Control de playlists desde disco rotatorio (1-9)**
- [x] **Sistema escalable sin modificar código ESP32**
- [ ] Mapeo avanzado: playlists diferentes por habitación
- [ ] Feedback visual/sonoro tras ejecutar webhook
- [ ] Cola de reproducción automática en Music Assistant
- [ ] Integración con otros servicios de HA (escenas, scripts)
- [ ] Carcasa y ajuste acústico final


## Estado actual (v0.5)

- ✅ Salida de audio validada con MAX98357A
- ✅ Altavoz original del Ericofono probado exitosamente
- ✅ Sistema LittleFS implementado
- ✅ Reproducción MP3 local funcionando
- ✅ Test mínimo de sonido (tono 440Hz) validado
- ✅ Entradas de gancho (GPIO32) y disco (GPIO33) configuradas con pull-ups internos
- ✅ Test de detección de gancho y pulsos funcionando
- ✅ Máquina de estados completa (IDLE → DIAL_TONE → COUNTING)
- ✅ Streaming HTTPS desde Navidrome operativo (mantenido pero deshabilitado)
- ✅ Optimización WiFi y buffers para comunicación fluida
- ✅ Sistema de credenciales seguro (secrets.h)
- ✅ **Integración Music Assistant completa**
- ✅ **Webhooks HTTP funcionando (código 200)**
- ✅ **Control de 9 playlists desde disco rotatorio**
- ✅ **Compatible con Home Assistant en Docker modo host**
- 🚧 Expansión a múltiples habitaciones/players en progreso


## Uso rápido

### Primera configuración

1. **Copiar plantilla de credenciales:**

cp src/secrets.example.h src/secrets.h
2. **Editar src/secrets.h con tus datos reales:**
    - SSID y contraseña WiFi
    - URL de tu servidor Navidrome (opcional)
    - Usuario y contraseña de Navidrome (opcional)
3. **Añadir archivos de audio a carpeta /data/**
4. **Configurar automatizaciones en Home Assistant:**
    - Crear webhook para cada número (1-9)
    - Configurar entity_id de tu Music Assistant player
    - Configurar IDs de playlists

### Compilar y subir

    pio run --target upload
    
### Subir sistema de archivos

    pio run --target uploadfs
    
### Monitor serial

    pio device monitor -b 115200
    
### Verificar webhooks

Al marcar un número, el Serial Monitor mostrará:

    Número marcado: 1
    http://10.0.1.3:8123/api/webhook/ericofono-lista01-despacho
    Código HTTP: 200
    Webhook ejecutado, sistema en reposo
    Código 200 = éxito. Otros códigos indican problema de red o configuración.

## Troubleshooting

### Webhook no responde (código != 200)

1. **Verificar IP de Home Assistant:**
    - En el código: http://10.0.1.3:8123/...
    - Cambiar por IP correcta de tu HA
2. **Verificar que HA esté en modo host (Docker):**
    - Puerto 8123 accesible directamente en IP del host
    - No requiere mapeo de puertos
3. **Verificar automatización en HA:**
    - webhook_id debe ser exactamente ericofono-lista0X-despacho
    - Revisar logs de HA para ver si recibe el webhook
4. **Verificar conectividad WiFi:**
    - RSSI > -70 dBm recomendado
    - Ping desde ESP32 a HA debe funcionar

### Music Assistant no reproduce

1. **Verificar entity_id del player:**
    - Ir a Estados en HA
    - Buscar media_player.ma_ y copiar nombre exacto
2. **Verificar ID de playlist:**
    - URL de Music Assistant: http://IP:8095/\#/playlists/library/7
    - Extraer número final (7 en este ejemplo)
    - Formato en automatización: media-source://mass/database://playlist/7
3. **Verificar que Music Assistant esté corriendo:**
    - Addon iniciado en HA
    - Player disponible y online

### Streaming entrecortado (Navidrome, si se reactiva)

1. Verificar señal WiFi (RSSI > -70 dBm)
2. Reducir bitrate en Navidrome (ya configurado a 128kbps)
3. Comprobar que WiFi.setSleep(false) esté activo
4. Aumentar buffer si hay memoria disponible

### Error SSL/TLS

- El código está optimizado para HTTPS
- La librería esphome/ESP32-audioI2S soporta HTTPS sin configuración adicional
- La contraseña se codifica automáticamente (URL-encode)


### No reproduce archivos locales

- Verificar que /data/dial_tone.mp3 y /data/click.mp3 existan
- Ejecutar pio run --target uploadfs para subir archivos
- Comprobar en Serial Monitor: "LittleFS montado correctamente"


### Gancho/disco no responden

- Verificar conexiones físicas a GPIO 32 y 33
- Comprobar que switches estén conectados a GND
- Los pull-ups internos se activan automáticamente con INPUT_PULLUP


## Notas importantes

- Todos los archivos .cpp deben incluir \#include <Arduino.h> al inicio (obligatorio en PlatformIO)
- Usar INPUT_PULLUP para switches mecánicos sin resistencias externas
- El cableado I2S debe ser corto y con masa común para evitar ruido
- Partición huge_app.csv necesaria para aplicaciones con audio embebido
- NUNCA subir secrets.h a GitHub - ya está en .gitignore
- Para servidores HTTPS, la contraseña se codifica automáticamente en el código
- Buffer de 32KB optimizado para streaming - no aumentar más sin PSRAM
- **Webhooks no requieren autenticación** - son públicos por diseño en HA
- **Home Assistant en Docker modo host** expone puerto 8123 directamente en IP del host
- **Sistema escalable:** añadir playlists creando automatizaciones sin tocar código ESP32


## Logros v0.5 (Febrero 2026)

✅ **Integración Music Assistant completada:**

- Sistema de webhooks HTTP implementado
- Control de 9 playlists desde disco rotatorio
- Compatible con Home Assistant en Docker
- Sin necesidad de autenticación
- Código HTTP 200 confirmado en producción

✅ **Arquitectura escalable:**

- Añadir playlists sin modificar firmware
- Solo crear automatizaciones en HA
- Formato de webhook consistente y predecible

✅ **Optimización de flujo:**

- Sin reproducción de dial_tone tras webhook
- Estado IDLE inmediato tras ejecutar acción
- Feedback en Serial Monitor para debugging


## Licencia

Open source – ver LICENSE

## Créditos

- Hardware: Ericofon original (LM Ericsson, 1956)
- ESP32: Espressif Systems
- Librería audio: esphome/ESP32-audioI2S
- Music Assistant: Music Assistant Team
- Home Assistant: Home Assistant Community
- Servidor música: Navidrome (Subsonic API)

