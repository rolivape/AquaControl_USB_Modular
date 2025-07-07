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
