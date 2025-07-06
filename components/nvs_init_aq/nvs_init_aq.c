#include "nvs_flash.h"
#include "esp_log.h"

static const char *TAG = "NVS_INIT_AQ";

void nvs_init_aq(void)
{
    ESP_LOGI(TAG, "Entrando a nvs_init_aq()...");

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_LOGW(TAG, "Formateando NVS...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "NVS inicializado correctamente.");
}
