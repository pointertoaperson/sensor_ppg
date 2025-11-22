import paho.mqtt.client as mqtt
import time

# Callback when the client connects to the broker
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("TEST/DEV02/heart")

# Callback when a message is received
def on_message(client, userdata, msg):
    print(f"Received: {msg.topic} {msg.payload.decode()}")

# Create MQTT client instance
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Connect to broker
client.connect("broker.hivemq.com", 1883, 60)

# Start loop in background so main thread can publish
client.loop_start()

try:
    while True:
        value = 222
        client.publish("TEST/DEV02/heart", value)
        print(f"Published: {value}")
        time.sleep(5)
except KeyboardInterrupt:
    print("Exiting...")
    client.loop_stop()
    client.disconnect()

