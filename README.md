# AquaControl USB — Firmware Modular para ESP32

Este firmware forma parte del sistema AquaControl USB. Define una arquitectura modular para ESP32 esclavos conectados por USB (CDC/NCM) o WiFi, coordinados por un RPi maestro. Está diseñado para ser escalable, robusto y fácil de mantener.

---

## 🎯 Objetivo

- Controlar sensores y actuadores del sistema de acuario de forma autónoma.
- Recibir configuración y reglas dinámicas desde el RPi vía MQTT.
- Operar por USB (NCM) o WiFi sin modificar la lógica de negocio.

---

## 🧱 Arquitectura de Componentes

El firmware está compuesto por módulos individuales, cada uno con responsabilidad única. Todos los módulos terminan en `_aq` para claridad.

### Componentes actuales:

- **`bios_config_aq`**  
  Maneja lectura y escritura de configuración persistente en NVS. Soporta modo interactivo tipo "BIOS" al arranque.

- **`ncm_aq`**  
  Inicializa TinyUSB en modo CDC-ECM (NCM) y configura IP estática si está definida.

- **`wifi_client_aq`**  
  Se activa solo si `enable_wifi` está en true (vía BIOS). Conecta como cliente a la red.

- **`mqtt_client_aq`**  
  Cliente MQTT que publica estado del módulo y recibe comandos. Siempre se activa, ya sea por NCM o WiFi.

- **`json_parser_aq`**  
  Procesa y valida mensajes JSON entrantes (configuración, comandos).

---

## 🔁 Flujo de Arranque (`main.c`)

```c
void app_main(void) {
    bios_init_aq();       // Carga config desde NVS
    ncm_start_aq();       // Inicia NCM si está configurado
    wifi_start_aq();      // Inicia WiFi si enable_wifi == true
    mqtt_start_aq();      // MQTT siempre se conecta
}
El main.c no contiene ninguna lógica condicional. Toda la lógica de habilitación/lectura está delegada a los componentes.

🛠 Configuración BIOS desde el ESP32
Al arrancar, el sistema espera 5 segundos por un input vía puerto serial para entrar al modo "BIOS" y configurar parámetros:

Tipo de módulo (relay, sensor, etc.)

IP (NCM)

SSID y Password (WiFi)

Habilitar WiFi o no

Todo se guarda en NVS y se puede modificar sin volver a flashear el ESP32.

🌐 Comunicación MQTT
MQTT es el único canal oficial de comunicación, independientemente del medio (USB o WiFi).

Se utilizan los siguientes tópicos:

text
Copy
Edit
acuario/esp32/config/<tipo_modulo>   ← el RPi publica config específica
acuario/esp32/status/<tipo_modulo>   → el ESP32 publica su estado o info BIOS
📡 Interfaces de Red
El firmware puede funcionar con una de dos interfaces activas:

NCM (USB CDC-ECM)
Se configura con IP fija desde firmware.

Conexión punto a punto con la RPi vía USB.

Ejemplo:

ESP32: 192.168.7.2

RPi: 192.168.7.1

c
Copy
Edit
esp_netif_ip_info_t ip_info;
ip_info.ip.addr = ipaddr_addr("192.168.7.2");
ip_info.gw.addr = ipaddr_addr("192.168.7.1");
ip_info.netmask.addr = ipaddr_addr("255.255.255.0");
WiFi
Se habilita si enable_wifi = true.

Se conecta como cliente.

IP dinámica o estática, según config BIOS.

🧠 Analogía: RPi como NVR / ESP32 como Cámaras
Sistema de Video	AquaControl USB
NVR central	RPi
Cámaras IP	Módulos ESP32
Cable ethernet/WiFi	NCM por USB o WiFi
Stream de video	Datos JSON
Software de monitoreo	MQTT + Dashboard en RPi

🧪 Estado Actual
✅ Arquitectura definida

✅ Componentes separados y operativos

✅ Orquestación limpia vía main.c

⏳ Consolidación final de scripts

⏳ Prueba de conexión punto a punto (NCM)

⏳ Backend SQLite en RPi en fase de diseño

🗺 Próximos Pasos
Consolidar y compilar el firmware completo.

Validar comunicación punto a punto (NCM).

Enviar y recibir configuración vía MQTT.

Diseñar base de datos SQLite en RPi.

Ampliar reglas dinámicas y logging centralizado.

📁 Estructura del Proyecto
css
Copy
Edit
esp32_firmware/
├── components/
│   ├── bios_config_aq/
│   ├── ncm_aq/
│   ├── wifi_client_aq/
│   ├── mqtt_client_aq/
│   └── json_parser_aq/
├── main/
│   └── main.c
├── sdkconfig
├── CMakeLists.txt

## ⏱ Flujo de Ejecución: Desde Boot hasta Lógica Funcional

### 🔁 Flujo de ejecución completo – desde boot hasta operación

---

## 🧷 0. Bootloader y particiones
- ESP32-S3 inicia desde la partición `bootloader`.
- Revisa partición de `factory` o `ota_0` (si aplica).
- Carga el firmware principal desde `0x10000`.
- Inicializa SPI Flash, RNG, eFuse, etc.
- Muestra tabla de particiones y pasa el control a `app_main()`.

---

## 🟢 1. app_main() (en `main/main.c`)
- Solo invoca `system_init_aq()` respetando la política de main mínimo.

---

## 🧠 2. system_init_aq() (en `system_aq.c`)
Contiene toda la lógica de preparación del sistema:

```c
void system_init_aq(void)
{
    nvs_init_aq();                      // 🔹 Init robusto de NVS
    bios_config_init_aq();             // 🔹 Carga configuración desde NVS
    bios_config_check_or_menu_aq();    // 🔹 Ofrece menú interactivo si hay input
    bios_config_print_aq();            // 🔹 Muestra configuración cargada

    ncm_aq_init();                     // 🔹 Inicializa USB NCM (si está habilitado)
    mqtt_client_aq_init();            // 🔹 Arranca cliente MQTT (si aplica)
    wifi_client_aq_init();            // 🔹 (si el panel lo requiere)

    hw_control_init();                // 🔹 Inicializa capa HW según panel
    logic_manager_init();            // 🔹 FSM o lógica embebida
}
```

---

## 🧩 3. bios_config_aq/: Configuración persistente
- Carga o genera `bios_config_t` con:
  - IP, Netmask, Gateway
  - `enable_ncm`, `enable_wifi`, `enable_mqtt`
- Ofrece menú tipo BIOS si se detecta input UART al boot.

---

## 🔌 4. ncm_aq/: Comunicación USB (CDC/NCM)
- Si `enable_ncm == true`, se habilita NCM como interfaz virtual de red.
- Permite hacer `ping` desde RPi vía USB.

---

## 📡 5. mqtt_client_aq/ y wifi_client_aq/
- MQTT se inicializa y suscribe a tópicos.
- WiFi sólo si está habilitado (más común en Panel I/O).

---

## ⚙️ 6. hw_control/: Abstracción hardware
- Detecta panel activo (AC, DC, IO).
- Llama al `panel_ac_init()` o `panel_dc_init()`.
- Cada uno implementa GPIOs, relés, sensores y lógica SAFE local.

---

## 🧠 7. logic/: Control embebido
- Aquí vivirá el FSM del panel.
- Puede actuar sobre eventos, sensores, comandos, etc.

---

## 🧾 8. json/ y rol_manager/
- `json_parser_aq`: Parser de mensajes JSON entrantes.
- `rol_manager`: Gestiona identidad, rol y tipo de módulo.

---

✅ Resultado final:
El ESP32 queda activo con configuración cargada, comunicaciones inicializadas y panel operativo.


AquaControl_USB_Modular

Sistema modular de automatización para acuarios marinos basado en ESP32-S3 + Raspberry Pi. La arquitectura es 100% local, sin dependencia de la nube, y permite control, automatización y monitoreo a través de mensajes JSON sobre USB-CDC o MQTT.

🧠 Arquitectura General
	•	ESP32-S3: Módulos esclavos (AC, DC, IO…) conectados por USB-CDC o Wi-Fi.
	•	RPi (MASTER): Controlador central. Procesa comandos, guarda logs, muestra dashboards.
	•	Protocolo: JSON validado, MQTT como bus interno de eventos.

🔌 Comunicación USB-CDC (esp_tinyusb)

Implementación correcta:

Utilizamos esp_tinyusb exclusivamente como backend para USB-CDC, con configuración manual del stack USB. El dispositivo se comporta como consola estándar (stdin/stdout) sin interferencia de red.

Lecciones aprendidas:
	•	NO usar tinyusb_net / tusb_ncm: Provoca conflictos con consola y flashing, limita compatibilidad.
	•	NO mapear VFS al UART físico (UART_NUM_0) si usamos USB-CDC.
	•	NO usar uart_read_bytes con USB-CDC: no detecta input desde idf.py monitor.

Solución robusta:

Usamos select() sobre stdin (VFS) para detectar entrada de usuario de forma no bloqueante. Esto permite:
	•	Menú BIOS interactivo al boot si se presiona tecla.
	•	Portabilidad entre USB o UART físico (sin cambiar código).
	•	Integración limpia con idf.py monitor.

🔧 Flujo de Boot
	1.	Boot
	2.	Espera 5 segundos en BIOS:
	•	Si usuario presiona tecla → menú interactivo.
	•	Si no → arranque normal con config desde NVS.
	3.	Inicio de operación y comunicación.

🧬 Componentes principales
	•	main/: Entrada app_main.
	•	components/
	•	bios_config_aq/: Config interactiva y persistencia NVS.
	•	usb_cdc_aq/: Configuración USB CDC vía esp_tinyusb.
	•	system_aq/: Inicialización del sistema.
	•	json_parser_aq/: Validación de estructuras JSON.
	•	mqtt_client_aq/: Publicación y recepción MQTT.
	•	logic_aq/: SAFE/NORMAL mode.
	•	hw/: Abstracción de sensores y actuadores.

🧭 Flujo de trabajo modular
	1.	El ESP32 al arrancar consulta si hay entrada por stdin.
	2.	Si hay tecla presionada, lanza menú interactivo (BIOS).
	3.	Si no, carga parámetros de NVS y comienza la operación.
	4.	Se conecta por MQTT (USB o WiFi) y empieza a emitir:
	•	Heartbeats
	•	Status
	•	Sensor_data
	5.	Recibe comandos y responde con ACK.

🧪 Validación de JSONs

RPi mantiene una base de datos SQLite con los esquemas de validación:
	•	schemas.db con tabla esquemas
	•	Scripts CLI para validar:
	•	validarjson –esquema sensor_data
	•	validarjson –esquema comando_control –archivo custom.json

📡 Estructura de Tópicos MQTT

Raíz: acuario/

Publicaciones ESP32:
	•	acuario/sensor/dc/temperatura
	•	acuario/status/dc
	•	acuario/ack/dc/bomba_retorno

Comandos RPi:
	•	acuario/comando/dc/bomba_retorno
	•	acuario/comando/io/valvula_sump

Ejemplo:
	1.	ESP32 publica sensor
	2.	RPi envía comando
	3.	ESP32 responde con ACK

📁 Estructura complementaria en RPi
	•	jsons/: JSONs de prueba
	•	scripts/: CLI para validación
	•	validador_json/: Lógica y setup
	•	setup.sh: Script automatizado de entorno

✅ Próximos pasos
	•	Consolidar NCM en segunda fase
	•	Integrar setup de red dinámico vía config BIOS
	•	Estabilizar lógica SAFE MODE local
	•	Proteger flashing con modo recovery