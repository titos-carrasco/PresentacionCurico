#include "MiRed.h"

// usaremos JSON como formato para los datos
#include <ArduinoJson.h>
JsonDocument jsonDoc;
char buffer[512];

// definiciones para los sensores
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
char dbuff[128];

#include <SoftwareSerial.h>
SoftwareSerial comm;

#include "SdsDustSensor.h"
SdsDustSensor sds(comm);

// inicializacion del programa
void setup() {
  // inicia monitor serial
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.flush();

  // inicializa la pantalla
  Wire.begin(OLED_SDA, OLED_SCL);
  oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  oled.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  oled.clearDisplay();

  // inicializa el SDS011
  comm.begin(9600, SWSERIAL_8N1, SSERIAL_RX, SSERIAL_TX, false);
  sds.begin();
  Serial.println(sds.queryFirmwareVersion().toString());   // prints firmware version
  Serial.println(sds.setQueryReportingMode().toString());  // ensures sensor is in 'query' reporting mode
  Serial.flush();

  // inicializa la red
  net_init();
}

// ciclo de ejecucion
void loop() {
  // conecta al servicio en la red
  oled.setCursor(0, 0);
  oled.write("Conexion ...   ");
  oled.display();

  if (net_connect()) {
    oled.setCursor(6 * 13, 0);
    oled.write("OK\n\n");
    oled.display();

    jsonDoc.clear();

    // id de este nodo
    jsonDoc["nodo_id"] = NODO_ID;

    // datos de los sensores conectados al ADS
    PmResult pm = sds.queryPm();
    float pm25 = pm.pm25;
    jsonDoc["pm25"] = pm25;
    float pm10 = pm.pm10;
    jsonDoc["pm10"] = pm10;

    // para procesamiento interno
    yield();

    // formateamos y vemos la data a transmitir
    serializeJson(jsonDoc, buffer);
    Serial.print("Data: ");
    Serial.print(buffer);
    Serial.println();
    Serial.flush();

    // tambien en la pantalla
    oled.write("nova PMsensor\n\n");
    sprintf(dbuff, "PM25: %5.1f\n\n", pm25);
    oled.write(dbuff);
    sprintf(dbuff, "PM10: %5.1f\n", pm10);
    oled.write(dbuff);
    oled.display();

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
