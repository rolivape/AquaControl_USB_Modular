typedef struct
{
    char ip[16];
    char netmask[16];
    char gateway[16];
    bool enable_ncm;
    bool enable_wifi;
    bool enable_mqtt;
} bios_config_t;

extern bios_config_t bios_config;

esp_err_t bios_config_init_aq(void);
esp_err_t bios_config_save_aq(const bios_config_t *config);
esp_err_t bios_config_load_aq(bios_config_t *config);
void bios_config_print_aq(void);
