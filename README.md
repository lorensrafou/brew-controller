# brew-controller
Raspberry Pi Based Homebrew Controller

This module will use a DS18B20 temporature sensor to sense and control mashing temperature

HW:
* Raspbery Pi 3 B+ - https://www.adafruit.com/product/3775
* High Temp DS18B20 - https://www.adafruit.com/product/642
* Power Relay Module - https://www.adafruit.com/product/2935

## Setting up the PI
Edit the boot settings to enable the 1 wire interface

```
sudo nano /boot/config.txt
```

add the line:
"dtoverlay=w1-gpio"

## Installing the server
Clone the repo  

Setup a virtual environment and install the packages.

```
python3 -m venv .\venv
source venv/bin/activate
```

To exit the venv 

```
deactivate
```

## Starting Development

Create a virtual environment and install pylint:

```
python -m venv .\venv
.\venv\activate.ps1
pip install pylint
pip install -r requirements.txt
```