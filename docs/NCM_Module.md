# 🧩 Módulo de Red USB-NCM para AquaControl-USB

Este módulo implementa una interfaz de red virtual sobre USB (NCM - Network Control Model) para habilitar comunicación punto a punto entre un ESP32-S3 y un host (por ejemplo, una Raspberry Pi), usando MQTT o TCP/IP.

---

## 🎯 Objetivo del diseño

- Mantener **un canal UART independiente** para flashing/debug desde una Mac.
- Exponer **una interfaz NCM (Ethernet-over-USB)** a una Raspberry Pi.
- Integrar cliente MQTT para control remoto desde el host.

---

## ⚙️ Arquitectura USB Propuesta

| Puerto USB        | Función                         | Conectado a |
| ----------------- | ------------------------------- | ----------- |
| **USB derecho**   | UART (flashing, monitor serial) | **Mac**     |
| **USB izquierdo** | NCM (red virtual + MQTT)        | **RPi**     |

---

## 🔌 Componentes involucrados

- `ncm_aq.c` → Inicializa pila USB/NCM y configura red virtual.
- `mqtt_client_aq.c` → Se conecta al broker MQTT del host.
- `bios_config_aq` → Asigna IP y otros parámetros en tiempo de boot.
- Stack: `tinyusb`, `tusb_net.h`, `esp_netif`, `mqtt_client`.

---

## ✅ Funcionalidades

- Inicializa TinyUSB como dispositivo NCM.
- Crea interfaz de red virtual sobre USB.
- Asigna IP estática al ESP32 (`192.168.7.2`).
- Conecta a broker MQTT (`192.168.7.1`).
- Se suscribe a topic `acuario/esp32/config/<tipo_modulo>` para recibir configuración dinámica.
- Modularidad total: cada módulo tiene sufijo `_aq`.

---

## 🧠 Lecciones integradas

| Problema                         | Solución Aplicada                            |
| -------------------------------- | -------------------------------------------- |
| Conflicto CDC + NCM simultáneos  | Separación de puertos físicos (UART/NCM).    |
| Logs perdidos por USB            | UART dedicado para debug.                    |
| Fallo por DHCP                   | IP fija desde `bios_config_aq` o script.     |
| Errores de compilación TinyUSB   | Uso limpio del stack oficial (sin wrappers). |
| Paths rotos por estructura mixta | Reorganización bajo `esp32_firmware/`.       |

---

## 🧪 Cómo probar conectividad NCM

1. Conecta ESP32 (puerto NCM) a RPi.
2. Asigna IP a `usb0` en RPi:
```bash
sudo ip addr add 192.168.7.1/24 dev usb0
sudo ip link set usb0 up


Realiza un ping:

bash
Copy
Edit
ping 192.168.7.2
Publica un mensaje de prueba MQTT:

bash
Copy
Edit
mosquitto_pub -h 192.168.7.2 -t acuario/cmd -m "GET_TEMP"
🧠 Analogía: RPi como NVR, ESP32 como Cámaras IP
RPi = NVR: orquesta, decide, consulta.

ESP32 = cámara IP: pasivo, responde, simple.

text
Copy
Edit
     +---------------------------+
     |        Raspberry Pi       |
     |         (NVR/Client)      |
     +-------------+-------------+
                   |
             USB/NCM (eth)
                   |
     +-------------v-------------+
     |          ESP32            |
     |  (Sensor/Relay Module)    |
     |       (Server role)       |
     +---------------------------+
🧩 IP estática desde ESP32
c
Copy
Edit
esp_netif_ip_info_t ip_info;
ip_info.ip.addr = ipaddr_addr("192.168.7.2");
ip_info.gw.addr = ipaddr_addr("192.168.7.1");
ip_info.netmask.addr = ipaddr_addr("255.255.255.0");

ESP_ERROR_CHECK(esp_netif_dhcpc_stop(netif_ncm));
ESP_ERROR_CHECK(esp_netif_set_ip_info(netif_ncm, &ip_info));
📍 Este código vive dentro de ncm_init_aq() o se genera dinámicamente desde bios_config_aq.

🔄 Comportamiento esperado
Si bios_config_aq detecta parámetros en NVS → usa esos valores.

Si no hay valores → carga defaults.

MQTT está siempre activo si red está activa (NCM o WiFi).

🧭 Estado actual del módulo
 UART y NCM separados por puerto físico.

 TinyUSB operativo como NCM.

 IP fija o dinámica desde bios_config_aq.

 MQTT funcional.

 Arquitectura modular implementada.

 JSON parser en validación.

 Integración con reglas DSL pendiente.

📁 Archivos relevantes
markdown
Copy
Edit
esp32_firmware/
└── components/
    ├── ncm_aq/
    │   ├── ncm_aq.c
    │   └── ncm_aq.h
    ├── mqtt_client_aq/
    ├── bios_config_aq/
    └── json_parser_aq/    ← en desarrollo
