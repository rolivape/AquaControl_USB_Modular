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
