#include "MiRed.h"

#if MODO_MQTT == 1

#include <ESP8266WiFi.h>
WiFiClient wifiClient;

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
Adafruit_MQTT_Client mqttClient(&wifiClient, MQTT_SERVER, MQTT_PORT, NODO_ID, "", "");

bool connectToWIFI();
bool connectToMQTT();

void net_init() {
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
}

bool net_connect() {
  return connectToWIFI() && connectToMQTT();
}

void net_publish(char *payload) {
  Serial.print("Publicando la data en MQTT: .");
  Serial.flush();
  if (mqttClient.publish(MQTT_TOPIC, payload, 0, false))
    Serial.println(" OK");
  else
    Serial.println(" Error");
  Serial.flush();
  mqttClient.ping();
}

void net_ping() {
  mqttClient.ping();
}

void net_close() {
  mqttClient.disconnect();
  wifiClient.stop();
  delay(1000);
}


bool connectToWIFI() {
  if (WiFi.status() == WL_CONNECTED)
    return true;

  Serial.print("\n\nConectando a la WiFi (");
  Serial.print(WIFI_SSID);
  Serial.print("): .");
  Serial.flush();
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  for (int i = 0; i < 20; i++) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print(" Ok\n");
      Serial.flush();
      return true;
    }
    Serial.print(".");
    Serial.flush();
    delay(1000);
  }
  Serial.print(" Error al intentar conectar\n");
  Serial.flush();
  return false;
}

bool connectToMQTT() {
  if (mqttClient.connected())
    return true;

  Serial.print("Conectando al servicio MQTT (");
  Serial.print(MQTT_SERVER);
  Serial.print(":");
  Serial.print(MQTT_PORT);
  Serial.print("): .");
  Serial.flush();
  for (int i = 0; i < 10; i++) {
    if (mqttClient.connect() == 0) {
      Serial.print(" Ok\n");
      Serial.flush();
      return true;
    }
    Serial.print(".");
    Serial.flush();
    delay(100);
  }
  Serial.print(" Error al conectar\n");
  Serial.flush();
  return false;
}

#endif
