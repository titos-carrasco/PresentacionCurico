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

#include <ADS1115_WE.h>
ADS1115_WE adc = ADS1115_WE(I2C_ADDRESS_ADS1115);

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

  // inicializa el ADS1115/ADS1015
  if (!adc.init()) {
    Serial.println("\nADS1115/ADS1015 no encontrado.");
    Serial.flush();
  } else {
    Serial.println("\nADS1115/ADS1015 encontrado.");
    Serial.flush();
    adc.setVoltageRange_mV(ADS1115_RANGE_2048);  // +/- 2048 mV
  }

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
    long results;
    results = readChannel(ADS1115_COMP_0_GND);
    jsonDoc["A0"] = results;
    results = readChannel(ADS1115_COMP_1_GND);
    jsonDoc["A1"] = results;
    results = readChannel(ADS1115_COMP_2_GND);
    jsonDoc["A2"] = results;
    results = readChannel(ADS1115_COMP_3_GND);
    jsonDoc["A3"] = results;

    // para procesamiento interno
    yield();

    // formateamos y vemos la data a transmitir
    serializeJson(jsonDoc, buffer);
    Serial.print("Data: ");
    Serial.print(buffer);
    Serial.println();
    Serial.flush();

    // tambien en la pantalla
    oled.write("Sensores analogos\n\n");
    sprintf(dbuff, "A0: %6ld\n", jsonDoc["A0"].as<long>());
    oled.write(dbuff);
    sprintf(dbuff, "A1: %6ld\n", jsonDoc["A1"].as<long>());
    oled.write(dbuff);
    sprintf(dbuff, "A2: %6ld\n", jsonDoc["A2"].as<long>());
    oled.write(dbuff);
    sprintf(dbuff, "A3: %6ld\n", jsonDoc["A3"].as<long>());
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

long readChannel(ADS1115_MUX channel) {
  long result = 0;  // 0V
  adc.setCompareChannels(channel);
  adc.startSingleMeasurement();
  while (adc.isBusy()) { delay(0); }
  result = adc.getRawResult();  // 0= 0V -- +32767=ADS1115_RANGE_xxxx
  return result;
}
