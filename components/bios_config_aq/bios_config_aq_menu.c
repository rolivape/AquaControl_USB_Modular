#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bios_config_aq.h"

// Elimina salto de línea de fgets
static void strip_newline(char *str)
{
    char *pos = strchr(str, '\n');
    if (pos)
        *pos = '\0';
}

void bios_config_menu_aq(void)
{
    printf("==== MENÚ CONFIGURACIÓN BIOS ====\n");

    bios_config_t config;
    bios_config_load_aq(&config);

    printf("IP actual [%s]: ", config.ip);
    scanf("%15s", config.ip);

    printf("Netmask actual [%s]: ", config.netmask);
    scanf("%15s", config.netmask);

    printf("Gateway actual [%s]: ", config.gateway);
    scanf("%15s", config.gateway);

    printf("¿Habilitar NCM? (1=Sí, 0=No) [%d]: ", config.enable_ncm);
    scanf("%d", (int *)&config.enable_ncm);

    printf("¿Habilitar WiFi? (1=Sí, 0=No) [%d]: ", config.enable_wifi);
    scanf("%d", (int *)&config.enable_wifi);

    printf("¿Habilitar MQTT? (1=Sí, 0=No) [%d]: ", config.enable_mqtt);
    scanf("%d", (int *)&config.enable_mqtt);

    esp_err_t err = bios_config_save_aq(&config);
    if (err == ESP_OK)
    {
        printf("✅ Configuración guardada correctamente.\n");
    }
    else
    {
        printf("❌ Error al guardar configuración.\n");
    }

    bios_config_print_aq(); // Usa la global ya actualizada
}
