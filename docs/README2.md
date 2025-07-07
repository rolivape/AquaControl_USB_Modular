
# AquaControl USB v3.5 — Sistema Modular para Acuarios

> Arquitectura local-first, modular y escalable, diseñada para acuarios marinos avanzados. Comunicación basada en USB CDC o NCM y gestionado por una Raspberry Pi (RPi).

---

## 📌 Objetivo General

Automatizar un acuario marino mediante módulos inteligentes (ESP32-S3) conectados por USB a una Raspberry Pi (RPi), que actúa como cerebro central. La comunicación se realiza con mensajes JSON sobre MQTT (inicialmente CDC Serial, luego USB-NCM).

---

## 🧱 Arquitectura Modular

**Componentes físicos y lógicos:**

- **RPi Master:**
  - Broker MQTT (Mosquitto)
  - CLI `menu_master`
  - Motor de reglas
  - Base de datos `schemas.db`
  - Validador JSON

- **ESP32-S3 Módulos:**
  - `PANEL_AC`: Relés de 110V, sensores temperatura
  - `PANEL_DC`: Válvulas 12V, PWM, ventiladores
  - `PANEL_IO`: Sensores de nivel, alarmas, flujo

Conexión por USB (CDC Serial en Fase 0, NCM en Fase 1+).

---

## 🔁 Fases del Proyecto

### 🧪 Fase 0 – Comunicación CDC Serial

- RPi envía JSON vía USB Serial (CDC).
- Comandos tipo `{ "cmd": "bios/show" }`.
- ESP responde con configuración cargada desde NVS.
- Se edita `.config` localmente y se reenvía con `bios/save`.
- Ideal para testeo y debugging sin stack completo MQTT.

### 🌐 Fase 1 – Migración a MQTT sobre USB-NCM

- TinyUSB-NCM habilita conexión Ethernet-over-USB.
- ESP32 adquiere IP y se conecta al broker MQTT local.
- Publicación/suscripción por tópicos estándar.
- Configuración y comandos ahora se manejan vía MQTT.

---

## 📁 Estructura del Proyecto

**ESP32 (firmware):**
- `main/`: punto de entrada (`main.c`)
- `components/`:
  - `bios_config_aq`: NVS y configuración persistente
  - `json_parser_aq`: validación y parsing de JSON
  - `mqtt_client_aq`: cliente MQTT embebido
  - `ncm_aq`: USB NCM wrapper (TinyUSB)
  - `system_aq`: init y shell BIOS
  - `panel_ac/dc/io`: lógica específica por módulo

**RPi (herramientas):**
- `rpi-tools/`:
  - `scripts/`: CLI para interacción (ej. `menu_master.py`)
  - `jsons/`: ejemplos de payloads
  - `validador_json/`: CLI para validar mensajes vs esquema
  - `schemas.db`: base de datos con esquemas JSON oficiales

---

## 📦 Repositorio de Esquemas JSON (schemas.db)

RPi aloja los esquemas JSON necesarios para validar mensajes entrantes y salientes. Se gestionan vía SQLite y scripts CLI. Algunos esquemas:

- `sensor_data`
- `status_response`
- `comando_control`
- `ack_comando`
- `heartbeat`

Estos esquemas son usados por `validador_json` para asegurar la integridad antes de enviar al bus MQTT.

---

## 🧭 Estructura de Tópicos MQTT

### Convención General

| Segmento     | Significado                                  |
|--------------|----------------------------------------------|
| `{tipo}`     | `sensor`, `status`, `comando`, `ack`         |
| `{modulo}`   | ID lógico del módulo (`dc`, `sump`, `io`)    |
| `{elemento}` | Sensor o actuador específico (`bomba`, etc.) |

Todos los tópicos usan la raíz:

```
acuario/{tipo}/{modulo}/{elemento}
```

### Publicaciones ESP32 → RPi

| Tópico                              | Contenido                           |
|-------------------------------------|-------------------------------------|
| `acuario/sensor/dc/temperatura`     | `{ "valor": 25.8, "unidad": "°C" }` |
| `acuario/status/dc`                 | `{ "estado": "OK" }`                |
| `acuario/ack/dc/bomba_retorno`      | `{ "resultado": "OK" }`             |

### Comandos RPi → ESP32

| Tópico                               | Contenido                           |
|--------------------------------------|-------------------------------------|
| `acuario/comando/dc/bomba_retorno`   | `{ "accion": "ENCENDER" }`          |
| `acuario/comando/io/valvula_sump`    | `{ "accion": "ABRIR" }`             |

---

## 🛠️ CLI en RPi

### Ejemplos

```bash
# Validar mensaje con esquema
validarjson --esquema heartbeat
validarjson --esquema ack_comando --archivo jsons/custom_ack.json

# Mostrar esquema
validarjson --esquema sensor_data --mostrar

# Configuración inicial
cd ~/aquarium-control
chmod +x setup.sh
./setup.sh
source ~/.zshrc
```

---

## ⚡ Boot y Configuración BIOS

- En cada boot, el módulo ESP32 revisa si se presionó una tecla por UART.
- Si se detecta input, se lanza el `bios_config_menu_aq()`.
- El `bios_config_menu_aq()` permite editar IP, netmask, gateway, NCM/WiFi/MQTT, y guarda en NVS.

> En versiones futuras, este menú se migrará a la RPi, usando `menu_master` con comunicación MQTT o CDC.

---

## 🧩 Diseño de Particiones (Ejemplo)

`partitions.csv` para 2MB flash:

```
# Name,    Type, SubType, Offset,  Size
nvs,       data, nvs,     0x9000,  0x6000
phy_init,  data, phy,     0xf000,  0x1000
factory,   app,  factory, 0x10000, 0x100000
```

Se sugiere ajustar tamaños si se usan OTA o logs extendidos.

---

## 📅 Roadmap

| Fase  | Objetivo                                     | Estado     |
|-------|----------------------------------------------|------------|
| 0     | Validación CDC Serial + JSON                 | ✅          |
| 1     | NCM Ethernet + MQTT funcional en ESP         | 🔄 En curso |
| 2     | Configuración remota desde `menu_master`     | 🔜          |
| 3     | Validación contra `schemas.db` en tiempo real| 🔜          |
| 4     | Motor de reglas (JSON y DSL tipo Python)     | 🔜          |

---

## 🧠 Lecciones Aprendidas

- La modularidad real requiere separación clara entre lógica, hardware y comunicación.
- `main.c` debe mantenerse limpio, con todo inicializado por `system_aq`.
- El `bios_config` embebido permite boot seguro incluso sin comunicación con RPi.
- JSON sobre CDC es una herramienta poderosa en etapa de boot/debug.
- TinyUSB NCM requiere cuidado con las dependencias de FreeRTOS y colisiones con tareas.

---

## 🧾 Créditos

Proyecto original: AquaControl_USB  
Autores: rod.oliva & ChatGPT PMO + Codex + Gemini  
Plataforma: ESP32-S3 + RPi 4  
Licencia: MIT  
Repositorio: https://github.com/rolivape/AquaControl-USB
