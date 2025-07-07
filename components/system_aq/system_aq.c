#include "system_aq.h"
#include "nvs_init_aq.h"
#include "bios_config_aq.h"
#include "bios_config_aq_menu.h"
#include "json_parser_aq.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <string.h>
#include <stdlib.h>

static const char *TAG = "SYSTEM_AQ";

void system_init_aq(void)
{
    // 1. Inicializaciones críticas que no dependen de la consola
    nvs_init_aq();
    bios_config_init_aq();

    // 2. Mensaje de log para indicar que el sistema básico está listo
    ESP_LOGI(TAG, "NVS y configuracion de BIOS inicializados.");
    ESP_LOGI(TAG, "Esperando 2 segundos para estabilizar USB-CDC...");
    vTaskDelay(pdMS_TO_TICKS(2000));

    // 3. Ahora que el USB está estable, comprobamos si el usuario quiere entrar al menú
    ESP_LOGI(TAG, "Comprobando entrada para el menu de BIOS...");
    bios_config_check_or_menu_aq();

    // 4. Bucle principal para la lógica de la aplicación (PING/PONG)
    ESP_LOGI(TAG, "Sistema inicializado. Esperando comandos PING por USB-CDC...");

    char buffer[256];
    fd_set set;
    struct timeval timeout;

    while (1)
    {
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int rv = select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout);
        if (rv > 0 && FD_ISSET(STDIN_FILENO, &set))
        {
            ssize_t bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
            if (bytes_read > 0)
            {
                buffer[bytes_read] = '\0';
                char *newline = strchr(buffer, '\n');
                if (newline) *newline = '\0';
                
                ESP_LOGI(TAG, "Recibido: %s", buffer);

                if (parse_ping_command(buffer))
                {
                    char *pong_response = create_pong_response();
                    if (pong_response)
                    {
                        printf("%s\n", pong_response);
                        ESP_LOGI(TAG, "Enviado PONG: %s", pong_response);
                        free(pong_response);
                    }
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
