
from flask import Flask, render_template
import os
import glob
import time

app = Flask(__name__)
debug = True


#Get the device going
os.system('modprobe w1-gpio')
os.system('modprobe w1-therm')

#Setup some variables for reading.
base_dir = '/sys/bus/w1/devices/'
device_folder = glob.glob(base_dir + '28*')[0]
device_file = device_folder + '/w1_slave'

#Reads the raw unformatted celcius temperature from the DS18B20
def read_temp_raw():
    f = open(device_file, 'r')
    lines = f.readlines()
    f.close()
    return lines

#Reads and formats the temperature in F
def read_temp():
    lines = read_temp_raw()
    while lines[0].strip()[-3:] != 'YES':
        time.sleep(0.2)
        lines = read_temp_raw()
    equals_pos = lines[1].find('t=')
    if equals_pos != -1:
        temp_string = lines[1][equals_pos+2:]
        temp_c = float(temp_string) / 1000.0
        temp_f = temp_c * 9.0 / 5.0 + 32.0
        return temp_f

#show the current temp
@app.route('/')
def index():
  return  "Welcome to the Brew Controller!"

@app.route('/temp')
def temp():
  return "%f" % read_temp()

#set the set temperature
@app.route('/setpoint/<int:setpoint>')
def setpoint(setpoint):
  return "%i" % setpoint

#set the set temperature
@app.route('/brew/<bool:start>')
def brew(start):
  return 'brewing: %b' % start

if __name__ == '__main__':
  app.debug = debug
  app.run(host='0.0.0.0')