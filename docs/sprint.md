# 🧠 SPRINT.md — Prompt de Trabajo para Codex y Gemini CLI

## 🎯 Objetivo del Sprint

Optimizar el proyecto **AquaControl_USB_Modular** hasta lograr una compilación completa y estable de la Fase 0:
- Comunicación USB-CDC funcional.
- Configuración BIOS interactiva (por stdin).
- Estructura modular clara y compilable.
- Documentación validada (`README.md`, `topics.md`, `GEMINI.md`).

---

## 🧱 Estructura del Proyecto
AquaControl_USB_Modular/
│
├── main/
├── components/
│   ├── bios_config_aq/
│   ├── system_aq/
│   ├── ncm_aq/               ⛔ aún con errores (no compila)
│   └── …
├── docs/
│   ├── README.md
│   ├── GEMINI.md
│   ├── topics.md
│   └── Sprint1.md ← este archivo

---

## 📌 Alcance actual del Sprint

### ✅ Ya implementado y funcionando:
- `bios_config_check_or_menu_aq()` usando `select()` sobre `stdin` para entrada por USB-CDC.
- Uso de `esp_tinyusb` para consola virtual USB.
- Lectura y escritura de configuración BIOS a NVS.
- Validación de esquemas JSON en RPi.

### ❌ Pendiente por resolver:
- El archivo `ncm_aq.c` no compila: símbolos como `tinyusb_net_config_t` y `tinyusb_net_init()` no existen o están mal definidos.
- No debe eliminarse, solo **aislar el archivo problemático** mientras se resuelve.
- Modularización de CDC + NCM (pendiente refactor limpio).

---

## 🛡 Instrucciones de seguridad
🚫 NO BORRAR archivos existentes, especialmente módulos en desarrollo.
✅ En su lugar, renómbralos con .bak o comenta su inclusión en CMakeLists.txt.

Por ejemplo:
mv ncm_aq.c ncm_aq.c.bak

---

## 📦 Build & Flash

```bash
idf.py build
idf.py -p /dev/ttyACM0 flash monitor

Verifica siempre el puerto correcto con ls /dev/tty.* o ls /dev/cu.* según sistema operativo.

Reglas para Codex / Gemini
	1.	Respeta la arquitectura modular: No fusionar componentes en main.
	2.	Documentar cambios en README.md o GEMINI.md.
	3.	Usar esp_tinyusb para USB-CDC, no tinyusb_net, ni esp-usb-net.
	4.	Los comandos interactivos deben funcionar desde stdin (VFS USB).
	5.	Cualquier archivo nuevo debe tener sufijo _aq si es parte del sistema.

⸻

🧩 Historia y contexto

Este proyecto ha evolucionado con múltiples aprendizajes:
	•	El uso de uart_read_bytes() no es compatible con USB-CDC → reemplazado con select() sobre stdin.
	•	El stack tinyusb_net introdujo errores, se abandonó en favor de una implementación propia sobre esp_tinyusb.
	•	La configuración BIOS se ha modularizado, persistiendo en NVS, y expuesta mediante consola al arranque.
	•	La validación de mensajes JSON se hace con esquemas guardados en SQLite en RPi (schemas.db).
    
    
    🔄 Workflow recomendado
# 1. Limpieza previa (opcional)
rm -rf build/

# 2. Renombrar módulos rotos (ej. NCM)
mv components/ncm_aq/ncm_aq.c components/ncm_aq/ncm_aq.c.bak

# 3. Compilar
idf.py build

# 4. Flashear
idf.py -p /dev/ttyACM0 flash monitor

# 5. Probar entrada BIOS por USB

📌 Notas finales
	•	No se permite hardcoding de tópicos MQTT ni JSON.
	•	Toda definición debe validarse desde la DB schemas.db.
	•	Este archivo (SPRINT.md) define el contexto del sprint activo y guía el trabajo de IA y humanos.