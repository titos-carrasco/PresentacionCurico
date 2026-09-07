#include <Arduino.h>

#ifndef _MI_RED_H
#define _MI_RED_H

#define OLED_SDA 5
#define OLED_SCL 4
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_ADDR 0x3C
#define I2C_ADDRESS_ADS1115 0x48

#define NODO_ID "Node_RCR_ANALOG"

#define WIFI_SSID "SSID"
#define WIFI_PASS "PASS"

#define MQTT_SERVER "192.168.197.146"
#define MQTT_PORT 1883
#define MQTT_TOPIC "rcr/demos/analog"

#define MODO_MQTT 1         // 0: modo local, 1: modo MQTT
#define DEEP_SLEEP 0        // 0: no usar DeepSleep, 1: usar DeepSleep
#define MS_SLEEP 1000       // milisegundos de espera entre ciclos (usar unos 1000 o menos para modo local)
#define MS_SLEEP_MQTT 3000  // milisegundos de espera entre ciclos (usar unos 1000 o menos para modo local) al usar MQTT

// ****************************************************************************
#if MODO_MQTT == 0
#define US_SLEEP MS_SLEEP * 1000UL  // microsegundos de espera entre ciclos para deep sleep

#define net_init()          // inicializa la red
#define net_connect() true  // conexion a la wifi y mqtt
#define net_publish(p)      // publica mensaje
#define net_ping()          // para indicar que estamos vivos
#define net_close()         // cierra conexion mqtt y de red

#else
#define MS_SLEEP MS_SLEEP_MQTT
#define US_SLEEP MS_SLEEP * 1000UL  // microsegundos de espera entre ciclos para deep sleep

extern void net_init();
extern bool net_connect();
extern void net_publish(char *payload);
extern void net_ping();
extern void net_close();

#endif

#endif  // _MI_RED_H