#pragma once

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char ip[16];        // Ej: "192.168.7.2"
    char netmask[16];   // Ej: "255.255.255.0"
    char gateway[16];   // Ej: "192.168.7.1"
    bool enable_ncm;
    bool enable_wifi;
    bool enable_mqtt;
} bios_config_t;

extern bios_config_t bios_config;

esp_err_t bios_config_init_aq(void);
esp_err_t bios_config_save_aq(const bios_config_t *config);
esp_err_t bios_config_load_aq(bios_config_t *config);
void bios_config_print_aq(void);
esp_err_t bios_config_menu_aq(void);
esp_err_t bios_config_check_or_menu_aq(void);

#ifdef __cplusplus
}
#endif
