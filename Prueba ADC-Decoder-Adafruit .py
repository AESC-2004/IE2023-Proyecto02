# Import standard python modules.
import sys
import time
import serial

# --- FUNCIONES AUXILIARES ---
def escalar_a_255(valor_str):
    """Convierte el valor del slider (0-180) a un byte entre 0-255"""
    valor = int(valor_str)
    valor_escalado = (valor * 255 + 90) // 180
    return min(valor_escalado, 255)


# This example uses the MQTTClient instead of the REST client
from Adafruit_IO import MQTTClient
from Adafruit_IO import Client, Feed


# holds the count for the feed
run_count = 0

# Set to your Adafruit IO username and key.
# Remember, your key is a secret,
# so make sure not to publish it when you publish this code!
ADAFRUIT_IO_USERNAME = "esc22235"
ADAFRUIT_IO_KEY = "aio_TKfW36GL3IoMBXNcgO72Bs22Tb13"

# Set to the ID of the feed to subscribe to for updates.
FEEDS_RECEIVE = ['Motor1_TX', 'Motor2_TX', 'Motor3_TX', 'Motor4_TX', 'Motor5_TX', 'Motor6_TX', 'Motor7_TX', 'Motor8_TX']
FEED_ID_Send = 'Counter_RX'

# Define "callback" functions which will be called when certain events 
# happen (connected, disconnected, message arrived).
def connected(client):
    """Connected function will be called when the client is connected to
    Adafruit IO.This is a good place to subscribe to feed changes. The client
    parameter passed to this function is the Adafruit IO MQTT client so you
    can make calls against it easily.
    """
    # Subscribe to changes on a all feeds.
    print('Connected. Suscribing to Feeds')
    for feed in FEEDS_RECEIVE:
        client.subscribe(feed)
        print(f"  - {feed}")


def disconnected(client):
    """Disconnected function will be called when the client disconnects."""
    sys.exit(1)

def message(client, feed_id, payload):
    """Message function will be called when a subscribed feed has a new value.
    The feed_id parameter identifies the feed, and the payload parameter has
    the new value.
    """
    print(f'Feed {feed_id} received new value: {payload}')
    # Publish or "send" message to corresponding feed
    print(f'Sendind data back: {payload}')
    client.publish(FEED_ID_Send, payload)
    # Verifying which Feed sent data, and which info. should be sent to the MCU
    if      feed_id == 'Motor1_TX':
        arduino.write(bytes('#A:'+str(escalar_a_255(str(payload)))+';\r\n', 'utf-8'))
    elif    feed_id == 'Motor2_TX':
        arduino.write(bytes('#B:'+str(escalar_a_255(str(payload)))+';\r\n', 'utf-8'))
    elif    feed_id == 'Motor3_TX':
        arduino.write(bytes('#C:'+str(escalar_a_255(str(payload)))+';\r\n', 'utf-8'))
    elif    feed_id == 'Motor4_TX':
        arduino.write(bytes('#D:'+str(escalar_a_255(str(payload)))+';\r\n', 'utf-8'))
    elif    feed_id == 'Motor5_TX':
        arduino.write(bytes('#E:'+str(escalar_a_255(str(payload)))+';\r\n', 'utf-8'))
    elif    feed_id == 'Motor6_TX':
        arduino.write(bytes('#F:'+str(escalar_a_255(str(payload)))+';\r\n', 'utf-8'))
    elif    feed_id == 'Motor7_TX':
        arduino.write(bytes('#G:'+str(escalar_a_255(str(payload)))+';\r\n', 'utf-8'))
    elif    feed_id == 'Motor8_TX':
        arduino.write(bytes('#H:'+str(escalar_a_255(str(payload)))+';\r\n', 'utf-8'))
    time.sleep(0.5)
    


arduino = serial.Serial(port='COM3', baudrate=9600, timeout=0.1, )
time.sleep(2)
# Create an MQTT client instance.
client = MQTTClient(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

# Setup the callback functions defined above.
client.on_connect = connected
client.on_disconnect = disconnected
client.on_message = message

# Connect to the Adafruit IO server.
client.connect()

# The first option is to run a thread in the background so you can continue
# doing things in your program.
client.loop_background()

while True:
    """ 
    # Uncomment the next 3 lines if you want to constantly send data
    # Adafruit IO is rate-limited for publishing
    # so we'll need a delay for calls to aio.send_data()
    run_count += 1
    print('sending count: ', run_count)
    client.publish(FEED_ID_Send, run_count)
    """
    print('Running "main loop" ')
    time.sleep(3)
