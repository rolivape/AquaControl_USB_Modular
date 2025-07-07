// bios_config_aq.c

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <errno.h>
#include "esp_log.h"
#include "bios_config_aq.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#define TAG "BIOS_CONFIG_AQ"
#define MENU_TIMEOUT_S 5
#define NVS_NAMESPACE "bios_config"

bios_config_t bios_config; // Config global

// --- Funciones de utilidad para entrada ---

static void strip_newline(char *str)
{
    char *pos = strchr(str, '\n');
    if (pos) *pos = '\0';
    pos = strchr(str, '\r');
    if (pos) *pos = '\0';
}

// Función simplificada para obtener una línea de entrada del usuario
static void get_line_input(const char *prompt, char *buffer, size_t buffer_size, const char *default_val)
{
    printf("%s [%s]: ", prompt, default_val);
    fflush(stdout);

    char line_buffer[128] = {0};
    // Usamos fgets para leer una línea completa, es más seguro.
    if (fgets(line_buffer, sizeof(line_buffer), stdin) != NULL) {
        strip_newline(line_buffer);
        if (strlen(line_buffer) > 0) {
            strncpy(buffer, line_buffer, buffer_size - 1);
            buffer[buffer_size - 1] = '\0';
        } else {
            // Si el usuario solo presiona Enter, usa el valor por defecto
            strncpy(buffer, default_val, buffer_size - 1);
            buffer[buffer_size - 1] = '\0';
        }
    } else {
        // En caso de error o EOF, usa el valor por defecto
        strncpy(buffer, default_val, buffer_size - 1);
        buffer[buffer_size - 1] = '\0';
    }
}

// --- Lógica principal del componente ---

esp_err_t bios_config_load_aq(bios_config_t *config)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) abriendo NVS", esp_err_to_name(err));
        return err;
    }

    size_t required_size = sizeof(bios_config_t);
    err = nvs_get_blob(nvs_handle, "config", config, &required_size);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "No se encontró config en NVS. Usando valores por defecto.");
        *config = (bios_config_t){
            .ip = "192.168.7.2",
            .netmask = "255.255.255.0",
            .gateway = "192.168.7.1",
            .enable_ncm = true,
            .enable_wifi = false,
            .enable_mqtt = true};
        err = ESP_OK;
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) leyendo de NVS", esp_err_to_name(err));
    }

    nvs_close(nvs_handle);
    return err;
}

esp_err_t bios_config_save_aq(const bios_config_t *config)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) return err;

    err = nvs_set_blob(nvs_handle, "config", config, sizeof(bios_config_t));
    if (err == ESP_OK) {
        err = nvs_commit(nvs_handle);
    }
    
    nvs_close(nvs_handle);
    return err;
}

esp_err_t bios_config_menu_aq(void)
{
    printf("\n==== MENÚ CONFIGURACIÓN BIOS ====\n");

    bios_config_t config;
    bios_config_load_aq(&config);

    char buffer[64];

    get_line_input("IP", config.ip, sizeof(config.ip), config.ip);
    get_line_input("Netmask", config.netmask, sizeof(config.netmask), config.netmask);
    get_line_input("Gateway", config.gateway, sizeof(config.gateway), config.gateway);

    snprintf(buffer, sizeof(buffer), "%d", config.enable_ncm);
    get_line_input("Habilitar NCM? (1=Sí, 0=No)", buffer, sizeof(buffer), buffer);
    config.enable_ncm = atoi(buffer);

    snprintf(buffer, sizeof(buffer), "%d", config.enable_wifi);
    get_line_input("Habilitar WiFi? (1=Sí, 0=No)", buffer, sizeof(buffer), buffer);
    config.enable_wifi = atoi(buffer);

    snprintf(buffer, sizeof(buffer), "%d", config.enable_mqtt);
    get_line_input("Habilitar MQTT? (1=Sí, 0=No)", buffer, sizeof(buffer), buffer);
    config.enable_mqtt = atoi(buffer);

    esp_err_t err = bios_config_save_aq(&config);
    if (err == ESP_OK) {
        printf("\u2705 Configuración guardada correctamente.\n");
        memcpy(&bios_config, &config, sizeof(bios_config_t));
    } else {
        printf("\u274c Error al guardar configuración: %s\n", esp_err_to_name(err));
    }

    bios_config_print_aq();
    return err;
}

esp_err_t bios_config_check_or_menu_aq(void)
{
    printf("\nPresiona Enter para entrar a configuración BIOS... (%d s)\n", MENU_TIMEOUT_S);
    fflush(stdout);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    char ch;
    bool received_input = false;
    for (int i = 0; i < MENU_TIMEOUT_S * 10; ++i) {
        esp_task_wdt_reset();
        if (read(STDIN_FILENO, &ch, 1) > 0) {
            received_input = true;
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // Restaurar stdin a modo bloqueante para fgets
    fcntl(STDIN_FILENO, F_SETFL, flags);

    if (received_input) {
        printf("\n[!] Entrada detectada. Ingresando a modo configuración...\n");
        // Limpia cualquier entrada residual leyéndola hasta que no quede nada.
        // Esto reemplaza a tcflush.
        while (read(STDIN_FILENO, &ch, 1) > 0);

        return bios_config_menu_aq();
    } else {
        ESP_LOGI(TAG, "No se detectó entrada. Continuando arranque normal.");
        return bios_config_init_aq();
    }
}

esp_err_t bios_config_init_aq(void)
{
    return bios_config_load_aq(&bios_config);
}

void bios_config_print_aq(void)
{
    printf("\n====== CONFIGURACIÓN ACTUAL ======\n");
    printf("  NCM:     %s\n", bios_config.enable_ncm ? "Habilitado" : "Deshabilitado");
    printf("  WiFi:    %s\n", bios_config.enable_wifi ? "Habilitado" : "Deshabilitado");
    printf("  MQTT:    %s\n", bios_config.enable_mqtt ? "Habilitado" : "Deshabilitado");
    printf("  IP:      %s\n", bios_config.ip);
    printf("  Netmask: %s\n", bios_config.netmask);
    printf("  Gateway: %s\n", bios_config.gateway);
    printf("==================================\n\n");
}
