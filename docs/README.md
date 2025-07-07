# AquaControl USB – Sistema Modular de Control para Acuarios

Proyecto de automatización robusto para acuarios marinos, basado en módulos ESP32-S3 conectados por USB a un controlador central (Raspberry Pi). Enfatiza un enfoque *local-first*, modular, expandible y resistente a fallos.

---

## 📦 Estructura del Repositorio

```
/AquaControl_USB_Modular/
├── main/                  # Entrada principal (app_main)
├── components/            # Módulos reutilizables (bios_config, mqtt, json, etc)
│   ├── bios_config_aq/    # Configuración NVS y menú interactivo
│   ├── json_parser_aq/    # Validación JSON
│   └── ...
├── partitions.csv         # Mapa de particiones
├── README.md              # Documentación principal
├── topics.md              # Estructura de tópicos MQTT
└── scripts/               # Scripts útiles para RPi
```

---

## 🚀 Fase 0 – Comunicación USB CDC + Configuración BIOS

En esta fase se habilita comunicación USB Serial (CDC-ACM) entre RPi y ESP32. Esto permite:

- Validar conectividad serial básica
- Enviar comandos JSON desde consola o Python
- Leer / escribir configuración en NVS vía menú interactivo
- Simular entrada manual al BIOS (UART) o comando automático (JSON)

**Funcionalidad crítica disponible:**

✅ Comunicación serial JSON (CDC)  
✅ Persistencia en NVS (`bios_config`)  
✅ Menú interactivo BIOS (`bios_config_menu_aq`)  
✅ Configuración con timeout por UART o comando remoto  
✅ Validación local del estado cargado

---

## 🧠 Arquitectura Modular (Diseño objetivo)

- **ESP32-S3** como nodos esclavos por USB (CDC o NCM)
- **RPi MASTER** como controlador central con:
  - Broker MQTT local (`mosquitto`)
  - Base de datos `schemas.db` con validación por JSON Schema
  - UI opcional (CLI, Web)
- Comunicación:
  - Local: CDC/NCM
  - Alternativo: Wi-Fi/MQTT
- Cada módulo implementa lógica SAFE MODE local

---

## 🧬 Validación de Esquemas JSON

**Repositorio:** `schemas.db` (SQLite en RPi)

Tabla `esquemas`:

| id  | nombre          | version | esquema (JSON) | descripcion              | timestamp           |
| --- | --------------- | ------- | -------------- | ------------------------ | ------------------- |
| 1   | sensor_data     | 1.0     | {...}          | Datos de sensores        | 2025-05-19T00:02:54 |
| 2   | comando_control | 1.0     | {...}          | Comando a módulo ESP32   | 2025-05-19T00:03:30 |
| 3   | status_response | 1.0     | {...}          | Estado general módulo    | 2025-05-19T00:04:06 |
| 4   | heartbeat       | 1.0     | {...}          | Señal de vida            | 2025-05-19T00:04:35 |
| 5   | ack_comando     | 1.0     | {...}          | ACK por comando recibido | 2025-05-19T00:49:47 |

---

## 🛰️ Estructura de Tópicos MQTT

**Archivo dedicado:** [`topics.md`](topics.md)

- Publicaciones desde ESP32:
  - `acuario/sensor/dc/temperatura`
  - `acuario/status/sump`
  - `acuario/ack/dc/bomba`

- Comandos desde RPi:
  - `acuario/comando/dc/valvula_sump`

---