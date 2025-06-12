# This connects and listens to the dummy MQTT PUBLISH FROM ESP8266




import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("/esp8266/adc")

def on_message(client, userdata, msg):
    print(f"Received: {msg.topic} {msg.payload.decode()}")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("broker.hivemq.com", 1883, 60)
client.loop_forever()
