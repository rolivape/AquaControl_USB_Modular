
# 🧩 Módulo de Red USB-NCM para AquaControl-USB

Este módulo implementa una interfaz de red virtual sobre USB (NCM - Network Control Model) para habilitar comunicación punto a punto entre un ESP32-S3 y un host (por ejemplo, una Raspberry Pi), usando MQTT o TCP/IP.

---

## 🎯 Objetivo del diseño

- Mantener **un canal serial UART independiente** para flashing/debug desde una Mac.
- Exponer **una interfaz NCM (Ethernet-over-USB)** a una Raspberry Pi para conectividad de red.
- Integrar cliente MQTT para control remoto desde el host.

---

## 🧱 Arquitectura USB Propuesta

| Puerto USB         | Función                          | Conectado a |
|--------------------|----------------------------------|-------------|
| **USB derecho**    | UART (flashing, monitor serial)  | **Mac**     |
| **USB izquierdo**  | NCM (red virtual + MQTT)         | **RPi**     |

---

## ⚙️ Componentes involucrados

- `network_ncm_aq.c` → Inicializa pila USB/NCM y lanza cliente MQTT.
- `network_ncm.h` → Header del módulo.
- `esp_tinyusb`, `tinyusb_net.h` → Stack USB embebido + glue para red.
- `esp_netif`, `mqtt_client` → Interfaz TCP/IP + cliente MQTT nativo.

---

## ✅ Funcionalidades del módulo

- Inicializa TinyUSB como dispositivo NCM
- Crea interfaz de red virtual sobre USB
- Asigna IP estática (`192.168.7.2`) al ESP32
- Conecta a broker MQTT (`192.168.7.1`)
- Se suscribe a topic `acuario/cmd`
- Está modularizado y sigue el sufijo `_aq`

---

## 🧠 Lecciones del Infierno de TinyUSB

| Tema                        | Solución aplicada o Recordatorio |
|-----------------------------|----------------------------------|
| CDC + NCM simultáneos       | Evitado (CDC omitido)            |
| Mezcla tinyusb + esp_tinyusb | **Nunca mezclar**               |
| `TickType_t` no resuelto    | Agregar `freertos` a PRIV_REQUIRES |
| `esp_tinyusb.h` no encontrado | Confirmar instalación con `idf.py add-dependency esp_tinyusb` |
| `menuconfig` sin efecto     | Agregar `esp_tinyusb` en `PRIV_REQUIRES` |
| Firma errónea en callback   | Confirmar typedefs actualizados |
| Campos eliminados de struct | Validar `tinyusb_net_config_t` actualizado |

---

## 🧪 Cómo probar conectividad NCM

1. Conecta ESP32 vía **puerto izquierdo USB** a la **Raspberry Pi**
2. Configura IP estática en RPi:
```bash
sudo ip addr add 192.168.7.1/24 dev usb0
sudo ip link set usb0 up
```
3. Verifica conectividad:
```bash
ping 192.168.7.2
```
4. Prueba MQTT:
```bash
mosquitto_pub -h 192.168.7.2 -t acuario/cmd -m "GET_TEMP"
```

---

## 📁 Archivos relevantes

- `components/ncm_aq/ncm_aq.c`
- `components/ncm_aq/ncm_aq.h`
- `main/main.c` (entrypoint: `ncm_init_aq()`)

---

## 🧠 Analogía: ESP32 como cámara IP

| Elemento CCTV | Equivalente en AquaControl USB |
|---------------|---------------------------------|
| NVR           | Raspberry Pi (orquestador)      |
| Cámara IP     | ESP32-S3 con NCM/MQTT           |
| Video stream  | Comandos/respuestas JSON        |

--- 

