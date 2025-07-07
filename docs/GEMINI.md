# 🤖 GEMINI.md – Guía de Interacción para Gemini CLI
**Proyecto: AquaControl_USB_Modular**

---

## 📌 Contexto del Proyecto

Este proyecto implementa un sistema modular de automatización para acuarios marinos, basado en módulos esclavos con ESP32-S3 conectados a una Raspberry Pi MASTER vía USB. Usa comunicación USB-CDC como vía principal en la Fase 0, con planeación futura para MQTT sobre USB-NCM o WiFi.

---

## 🎯 Rol de Gemini CLI

Gemini actúa como **ingeniero de firmware principal supervisado**. Sus funciones incluyen:

- Corregir errores de compilación y de ejecución en tiempo real.
- Leer y modificar archivos en `components/`, `main/` o `scripts/`.
- Refactorizar únicamente componentes solicitados.
- Usar contexto y documentación (`README.md`, `docs/`) como fuente principal.
- Respetar la arquitectura modular y el orden de inicialización.

---

## 🎯 Objetivo del Sprint Actual

Implementar y validar la **comunicación USB CDC (Serial)** entre RPi y ESP32-S3 usando `esp_tinyusb`, con una BIOS interactiva estable y una lógica de PING/PONG funcional.

📎 **Referencia:** `docs/sprint.md`

---

## ✅ Estado actual

- ✅ `bios_config_aq` opera correctamente, guarda en NVS y no usa APIs de UART.
- ✅ `esp_tinyusb` se inicializa correctamente como consola VFS para CDC.
- ✅ `select()` + `read()` permiten capturar entrada en BIOS por USB de forma segura.
- ✅ La lógica PING/PONG JSON funciona sobre USB-CDC.
- ✅ El firmware compila y se flashea sin errores.

---

## 📁 Componentes clave

| Componente       | Función                                             |
| ---------------- | --------------------------------------------------- |
| `bios_config_aq` | Configuración interactiva al boot (con menú y NVS). |
| `system_aq`      | Orquesta inicialización de cada módulo y contiene el bucle principal. |
| `json_parser_aq` | Valida y parsea mensajes JSON usando `cJSON`.       |
| `main/`          | Contiene `app_main()` limpio, que solo llama a `system_init_aq`. |

---

## 🚫 Restricciones

❌ No usar:
- `tinyusb_net`, `tusb_ncm`, `usb_ethernet`, ni `esp_netif_usb`.
- Funciones `uart_*` directamente. Todo por VFS de USB-CDC.
- `scanf()` o APIs de `termios.h`.

❌ No modificar:
- `main.c` sin autorización.
- El orden de inicialización en `system_aq.c` sin entender las dependencias.

✔️ Sí usar:
- `esp_tinyusb` como base única de USB CDC.
- `CONFIG_ESP_CONSOLE_USB_CDC=y` en `sdkconfig` para redirigir `stdin`/`stdout`.
- `select()` para entrada no bloqueante y `fgets` para lectura de línea.

---

## 🧠 Lecciones aprendidas obligatorias

1.  **El orden de inicialización es crítico:** `stdin` no está disponible hasta que la pila USB-CDC se ha inicializado y estabilizado. Cualquier intento de leer de la consola antes de tiempo (`bios_config_check_or_menu_aq`) causará un `LoadProhibited panic`. La solución es añadir un `vTaskDelay` en `system_aq.c` *antes* de intentar leer.
2.  **La consola debe ser USB-CDC:** El `sdkconfig` debe tener `CONFIG_ESP_CONSOLE_USB_CDC=y` y `CONFIG_ESP_CONSOLE_UART_DEFAULT=n`. De lo contrario, `printf` y otras funciones de stdio intentarán usar el UART físico, causando un pánico si no está configurado.
3.  **Evitar APIs de TTY:** Funciones como `tcflush` o la librería `termios.h` no son seguras para USB-CDC y deben evitarse. Para limpiar el buffer de entrada, es mejor leer y descartar los datos pendientes.
4.  **Usar `fgets` para entrada de línea:** Es más robusto y seguro que implementar una lógica de lectura de caracteres manual.

---

## 🧪 Flashing y Validación

1.  **Puerto de Flasheo:** El puerto serie para este dispositivo es `/dev/tty.wchusbserial59700477991`.

2.  **Comando de Flasheo:**
    ```bash
    idf.py -p /dev/tty.wchusbserial59700477991 flash
    ```

3.  **Comando de Monitoreo:**
    ```bash
    idf.py -p /dev/tty.wchusbserial59700477991 monitor
    ```

4.  **Prueba de BIOS:** Al iniciar, presiona `Enter` en el monitor serie para acceder al menú de configuración.
5.  **Prueba de PING/PONG:** Si no se accede a la BIOS, envía el siguiente JSON para probar la comunicación:
    ```json
    {"modulo_destino": "panel_ac", "comando": "PING"}
    ```
    El sistema debe responder con un JSON de PONG.
