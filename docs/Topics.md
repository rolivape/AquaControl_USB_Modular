from pathlib import Path

topics_md = """
# 🧩 topics.md — Estructura de Tópicos MQTT para AquaControl_USB

Todos los mensajes utilizan como raíz el namespace `acuario/`.

---

## 📌 Convención General

| Segmento     | Significado                                                |
| ------------ | ---------------------------------------------------------- |
| `{tipo}`     | Tipo de mensaje: `sensor`, `status`, `comando`, `ack`      |
| `{modulo}`   | Nombre lógico del módulo: `dc`, `ac`, `io`, `sump`, etc.   |
| `{elemento}` | Nombre del sensor o actuador: `temperatura`, `bomba`, etc. |

---

## 📤 Publicaciones de ESP32 hacia RPi

| Tópico MQTT                     | Payload JSON                                                        |
| ------------------------------- | ------------------------------------------------------------------- |
| `acuario/sensor/dc/temperatura` | `{ "valor": 25.8, "unidad": "°C", "timestamp": "..." }`             |
| `acuario/status/dc`             | `{ "estado": "OK", "timestamp": "..." }`                            |
| `acuario/ack/dc/bomba_retorno`  | `{ "resultado": "OK", "mensaje": "ejecutado", "timestamp": "..." }` |

---

## 📥 Comandos de RPi hacia ESP32

| Tópico MQTT                        | Payload JSON                               |
| ---------------------------------- | ------------------------------------------ |
| `acuario/comando/dc/bomba_retorno` | `{ "accion": "ENCENDER", "duracion": 10 }` |
| `acuario/comando/io/valvula_sump`  | `{ "accion": "ABRIR" }`                    |

---

## 🔁 Ejemplo de Flujo Completo

1. **ESP32 publica sensor:**
   - Tópico: `acuario/sensor/dc/temperatura`
   - Payload:
     ```json
     {
       "valor": 25.8,
       "unidad": "°C",
       "timestamp": "2025-05-19T14:25:00Z"
     }
     ```

2. **RPi publica comando:**
   - Tópico: `acuario/comando/dc/bomba_retorno`
   - Payload:
     ```json
     {
       "accion": "ENCENDER",
       "duracion": 10
     }
     ```

3. **ESP32 responde ACK:**
   - Tópico: `acuario/ack/dc/bomba_retorno`
   - Payload:
     ```json
     {
       "resultado": "OK",
       "mensaje": "Comando ejecutado",
       "timestamp": "2025-05-19T14:26:00Z"
     }
     ```

---

## 🔐 Notas Finales

- El broker MQTT corre localmente en la Raspberry Pi (Mosquitto).
- Todos los mensajes están en formato JSON.
- Los esquemas son validados por RPi con SQLite3 usando `schemas.db`.
- No se permite hardcoding de nombres o rutas: los esquemas y tópicos son dinámicos.
"""

topics_path = Path("/mnt/data/topics.md")
topics_path.write_text(topics_md.strip())
topics_path
