import paho.mqtt.client as mqtt
client = mqtt.Client()
on_connect = None
on_message = None
on_disconnect = None
deviceName = None
def setName ( name ) :
    global client,deviceName
    client = mqtt.Client(client_id=name)
    deviceName = name
def connect (ip, port, username, password, keepalive=60) :
    client.username_pw_set(username, password)
    if on_connect is not None:
        client.on_connect = on_connect
    if on_message is not None:
        client.on_message = on_message
    if on_disconnect is not None:
        client.on_disconnect = on_disconnect
    client.connect(ip, port, keepalive)
    client.subscribe("gearLocal/" + deviceName)
def reconnect() :
    mqtt.reconnect()
def loop_start () :

    client.loop_start()

def loop_stop () :
    client.loop_stop()

def subscribe (topic,scope='LOCAL') :
    if scope == 'LOCAL':
        client.subscribe(topic)
    if scope == 'GLOBAL':
        client.subscribe('netpie/'+topic)
def publish (topic,payload,scope='LOCAL') :
    if scope == 'LOCAL':
        client.publish(topic, payload=payload, qos=0, retain=False)
    if scope == 'GLOBAL':
        client.publish('global/'+topic, payload=payload, qos=0, retain=False)
def chat (deviceName,payload) :
    client.publish('gearLocal/'+deviceName, payload=payload, qos=0, retain=False)
