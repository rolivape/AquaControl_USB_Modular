// bios_config_aq.c

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "esp_log.h"
#include "bios_config_aq.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/uart.h"

#define TAG "BIOS_CONFIG_AQ"
#define MENU_TIMEOUT_MS 5000 // 5 segundos

bios_config_t bios_config; // Config global

// Función para cargar config desde NVS
esp_err_t bios_config_load_aq(bios_config_t *config);
// Función para guardar config en NVS
esp_err_t bios_config_save_aq(const bios_config_t *config);

// Lógica de menú interactivo
esp_err_t bios_config_menu_aq(void)
{
    bios_config_t config;

    printf("\n====== MODO CONFIGURACIÓN BIOS ======\n");

    printf("¿Habilitar NCM? (1=Sí, 0=No) [1]: ");
    scanf("%d", (int *)&config.enable_ncm);

    printf("¿Habilitar WiFi? (1=Sí, 0=No) [0]: ");
    scanf("%d", (int *)&config.enable_wifi);

    printf("¿Habilitar MQTT? (1=Sí, 0=No) [1]: ");
    scanf("%d", (int *)&config.enable_mqtt);

    printf("IP (ej. 192.168.7.2) [192.168.7.2]: ");
    scanf("%15s", config.ip);

    printf("Netmask (ej. 255.255.255.0): ");
    scanf("%15s", config.netmask);

    printf("Gateway (ej. 192.168.7.1): ");
    scanf("%15s", config.gateway);

    esp_err_t err = bios_config_save_aq(&config);
    if (err == ESP_OK)
    {
        printf("✅ Configuración guardada correctamente.\n\n");
    }
    else
    {
        printf("❌ Error guardando configuración.\n\n");
    }

    return err;
}

// Lógica de detección de input al boot
esp_err_t bios_config_check_or_menu_aq(void)
{
    char ch = 0;
    printf("\nPresiona una tecla para entrar a configuración BIOS... (%d ms)\n", MENU_TIMEOUT_MS);

    // Configuramos UART para polling sin eco
    uart_set_rx_timeout(UART_NUM_0, 1);
    int elapsed = 0;
    const int step = 100;

    while (elapsed < MENU_TIMEOUT_MS)
    {
        int len = uart_read_bytes(UART_NUM_0, (uint8_t *)&ch, 1, pdMS_TO_TICKS(step));
        if (len > 0)
        {
            printf("\n[!] Entrada detectada. Ingresando a modo configuración...\n");
            return bios_config_menu_aq(); // Modo configuración
        }
        elapsed += step;
    }

    ESP_LOGI(TAG, "No se detectó entrada. Continuando arranque normal.");
    return bios_config_init_aq(); // Solo carga config desde NVS
}
