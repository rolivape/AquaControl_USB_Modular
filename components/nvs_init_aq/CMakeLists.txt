idf_component_register(
    SRCS "nvs_init_aq.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES nvs_flash log
)
