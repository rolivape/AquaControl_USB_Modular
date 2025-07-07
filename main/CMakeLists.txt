idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES system_aq nvs_init_aq bios_config_aq
)
