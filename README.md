RPI IoT project

RPI IoT project is Internet of Things example project made using Arduino and Raspberry Pi.
The project consist of three sub projects are the Arduino node, RPI Gateway and the RPI UI.

Arduino Node project is for the Arduino sensor node that will send the measurement data via RF24 transmitter to Raspberry Pi. 
The Arduino node has only been tested Arduino Nano but it should work on other common models such as Arduino Uno.

The RPI Gateway listens for the RF24 and relays the data to ThingSpeak cloud service. The RF24 node has only been tested on
raspberry Pi Model B+ and Raspberry Pi 2 Model B, both running Raspbian (Debian Wheezy).

Raspberry Pi UI (Qt_Quick_Housemonitor) is the user interface that displays the data measured by Arduino node(s). 
The UI is meant to run on Raspberry Pi equipped with a small LCD display.

/********************************************************************************************************************************/

THINGS REQUIRED TO RUN THE PROJECT:

Raspberry Pi (pref. Model B+ or newer) with a working Internet connection
Arduino prototyping platform
