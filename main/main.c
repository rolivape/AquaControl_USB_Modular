#include "system_aq.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    // Pequeña demora para asegurar que el USB-CDC se estabilice
    vTaskDelay(pdMS_TO_TICKS(200));

    // Inicializa todo el sistema
    system_init_aq();

    // app_main puede terminar aquí, ya que system_init_aq contiene el bucle principal.
}
