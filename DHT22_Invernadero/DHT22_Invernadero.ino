#include "MiRed.h"

// usaremos JSON como formato para los datos
#include <ArduinoJson.h>
JsonDocument jsonDoc;
char buffer[512];

// definiciones para los sensores
#include "DHT.h"
DHT dht(PIN_DHT, DHT22);

// inicializacion del programa
void setup() {
  // inicia monitor serial
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.flush();

  // inicializa el DHT22
  dht.begin();

  // para leer el voltaje de la pila
  pinMode(PIN_VOLTAJE, INPUT);

  // inicializa la red
  net_init();
}

// ciclo de ejecucion
void loop() {
  // conecta al servicio en la red
  if (net_connect()) {
    jsonDoc.clear();
    
    // id de este nodo
    jsonDoc["nodo_id"] = NODO_ID;

    // datos del sensor DHT22
    float temperatura = dht.readTemperature();
    float humedad = dht.readHumidity();
    jsonDoc["temperatura"] = isnan(temperatura) ? -99999 : temperatura;
    jsonDoc["humedad"] = isnan(humedad) ? -99999 : humedad;

    // datos del voltaje
    jsonDoc["voltaje"] = analogRead(PIN_VOLTAJE);

    // para procesamiento interno
    yield();

    // formateamos y vemos la data a transmitir
    serializeJson(jsonDoc, buffer);
    Serial.print("Data: ");
    Serial.print(buffer);
    Serial.println();
    Serial.flush();

    // publicamos la data en servidor
    net_publish(buffer);

    // avisamos que seguimos vivos
    net_ping();
  }

// repetimos el proceso segun la configuracion
#if DEEP_SLEEP == 0
  delay(MS_SLEEP);
#else
  Serial.println();
  Serial.println("Reiniciando ...");
  Serial.flush();

  net_close();
  ESP.deepSleep(US_SLEEP);
#endif
}
