#!/usr/bin/env python3
# Requires PyAudio and PySpeech.
from pygame import mixer
import speech_recognition as sr
#import microgear.client as microgear
from time import ctime
import time
import os
from gtts import gTTS
#import playsound
import localMqtt as local
username = 'user'
password = '1234'
deviceName = 'voice'
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
# The callback for when a PUBLISH message is received from the server.

def on_message(client, userdata, msg):
    if msg.topic == 'namePeople':
        name = str(msg.payload.decode("utf-8"))
        speak('Welcome '+ name)

def on_disconnect(client, userdata, rc):
    if rc != 0:
        print("Unexpected disconnection.")
        local.reconnect()
def speak(audioString):
    #print(audioString)
    tts = gTTS(text=audioString, lang='en')
    local.publish('speech',audioString,'GLOBAL')
    fileName = str(time.time()) + ".mp3"
    tts.save(fileName)
    os.system("mpg321 " + fileName)
    mixer.init()
    mixer.music.load(fileName)
    mixer.music.play()
    while mixer.music.get_busy() == True:
        continue
    mixer.quit() 
    #playsound.playsound(fileName,True)

    #playsound(fileName,True)
    os.remove(fileName)
 
def recordAudio():
    # Record Audio
    r = sr.Recognizer()
    with sr.Microphone() as source:
        print("Say something!")
        #local.publish('speech',"Let talk with me.",'GLOBAL')
        audio = r.listen(source)
 
    # Speech recognition using Google Speech Recognition
    data = ""
    try:
        # Uses the default API key
        # To use another API key: `r.recognize_google(audio, key="GOOGLE_SPEECH_RECOGNITION_API_KEY")`
        data = r.recognize_google(audio)
        print("You said: " + data)
        
    except sr.UnknownValueError:
        #speak("I not understand you.")
        
        print("Google Speech Recognition could not understand audio")
    except sr.RequestError as e:
        print("Could not request results from Google Speech Recognition service; {0}".format(e))
 
    return data
 
def jarvis(data):
    if data is not '':
        local.publish('speech',"You : " + data,'GLOBAL')
        if "how are you" in data or data in ("how are you","how do you feel") :
            speak("I am fine")
    
        if "what time is it" in data:
            speak(ctime())
            
        if "turn on light" in data:
            speak("Ok turn on the light")
            local.chat("lightEdge",'1')
            
        if "turn off light" in data:
            speak("Ok turn off the light")
            local.chat("lightEdge",'0')
    
        if "turn off air" in data:
            speak("Ok turn off Air condition")
            local.chat("AIRCONSWITCH",'10')
            
        if "turn on air" in data:
            speak("Ok turn on Air condition")
            local.chat("AIRCONSWITCH",'11')
    
        if "where is" in data:
            data = data.split(" ")
            location = data[2]
            speak("Hold on, I will show you where " + location + " is.")
            os.system("chromium-browser https://www.google.nl/maps/place/" + location + "/&amp;")
 
# initialization

local.setName( deviceName )
local.on_connect = on_connect
local.on_message = on_message
local.on_disconnect = on_disconnect
local.connect("192.168.4.1", 1883, username, password)
local.subscribe('namePeople','LOCAL')
local.subscribe('Inroom','GLOBAL')
local.loop_start()
time.sleep(2)
#speak("สวัสดีฉันชื่อจูมง มีอะไรให้ฉันช่วย")
#speak("Hi everyone, what can I do for you?")

while 1:
    data = recordAudio()
    jarvis(data)

#client.loop_stop()
local.loop_stop()