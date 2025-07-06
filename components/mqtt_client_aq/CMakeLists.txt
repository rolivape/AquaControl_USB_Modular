idf_component_register(
    SRCS "mqtt_client_aq.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES mqtt log
)
