#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void json_parser_init_aq(void);

    /**
     * @brief Parsea un string JSON para verificar si es un comando PING válido.
     *
     * @param json_str El string JSON a parsear.
     * @return true si el JSON es un PING para "panel_ac", false en caso contrario.
     */
    bool parse_ping_command(const char *json_str);

    /**
     * @brief Crea una respuesta PONG en formato JSON.
     *
     * @return Un string JSON que debe ser liberado por el llamador con free().
     */
    char *create_pong_response(void);

#ifdef __cplusplus
}
#endif