import time
import queue
import json
import requests
import paho.mqtt.client as mqtt

THINGSPEAK_URL = "https://api.thingspeak.com/update"


class Bridge:
    def __init__(self, mqtt_server, mqtt_port, mqtt_topics):
        self.messages = queue.Queue()

        self.mqtt_server = mqtt_server
        self.mqtt_port = mqtt_port
        self.mqtt_topics = mqtt_topics
        self.mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
        self.mqtt_client.on_connect = self.mqttOnConnect
        self.mqtt_client.on_message = self.mqttOnMessage

    def mqttOnConnect(self, client, _userdata, _flags, _reason_code, _properties):
        for topic in self.mqtt_topics:
            client.subscribe(topic)

    def mqttOnMessage(self, _client, _userdata, msg):
        self.messages.put(msg)

    def run(self):
        self.mqtt_client.connect(self.mqtt_server, self.mqtt_port)
        self.mqtt_client.loop_start()
        time.sleep(2)

        running = True
        while running:
            try:
                message = self.messages.get_nowait()
            except queue.Empty:
                time.sleep(0.1)
                continue
            payload = message.payload.decode("utf-8")
            print(payload, flush=True)

            data = json.loads(payload)
            if data["nodo_id"] == "Node_RCR_PMSENSOR":
                feed = {
                    "api_key": "XXX",
                    "field1": data["pm25"],
                    "field2": data["pm10"],
                }
                resp = requests.post(THINGSPEAK_URL, data=feed)
            elif data["nodo_id"] == "Node_RCR_DHT22":
                feed = {
                    "api_key": "XXX",
                    "field1": data["temperatura"],
                    "field2": data["humedad"],
                }
                resp = requests.post(THINGSPEAK_URL, data=feed)
            elif data["nodo_id"] == "Node_RCR_ANALOG":
                feed = {
                    "api_key": "XXX",
                    "field1": data["A0"],
                    "field2": data["A1"],
                    "field3": data["A2"],
                    "field4": data["A3"],
                }
                resp = requests.post(THINGSPEAK_URL, data=feed)


# ---
app = Bridge(
    "127.0.0.1", 1883, ["rcr/demos/PMsensor", "rcr/demos/dht22", "rcr/demos/analog"]
)
app.run()
