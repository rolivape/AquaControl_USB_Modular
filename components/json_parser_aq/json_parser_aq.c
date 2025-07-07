#include "json_parser_aq.h"
#include "esp_log.h"
#include "cJSON.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *TAG = "JSON_PARSER";

void json_parser_init_aq(void)
{
    ESP_LOGI(TAG, "Inicializando parser JSON");
}

bool parse_ping_command(const char *json_str)
{
    cJSON *root = cJSON_Parse(json_str);
    if (root == NULL)
    {
        ESP_LOGW(TAG, "Error al parsear JSON: %s", cJSON_GetErrorPtr());
        return false;
    }

    cJSON *modulo = cJSON_GetObjectItem(root, "modulo_destino");
    cJSON *comando = cJSON_GetObjectItem(root, "comando");

    bool is_ping = false;
    if (cJSON_IsString(modulo) && (strcmp(modulo->valuestring, "panel_ac") == 0) &&
        cJSON_IsString(comando) && (strcmp(comando->valuestring, "PING") == 0))
    {
        is_ping = true;
        ESP_LOGI(TAG, "Comando PING recibido para panel_ac");
    } else {
        ESP_LOGW(TAG, "JSON recibido no es un comando PING válido para panel_ac");
    }

    cJSON_Delete(root);
    return is_ping;
}

char *create_pong_response(void)
{
    cJSON *root = cJSON_CreateObject();
    if (root == NULL)
    {
        return NULL;
    }

    cJSON_AddStringToObject(root, "modulo_origen", "panel_ac");
    cJSON_AddStringToObject(root, "resultado", "OK");
    cJSON_AddStringToObject(root, "mensaje", "PONG");

    // Generar timestamp ISO8601
    char timestamp[30];
    time_t now;
    struct tm timeinfo;
    time(&now);
    gmtime_r(&now, &timeinfo);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
    cJSON_AddStringToObject(root, "timestamp", timestamp);

    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    return json_str;
}