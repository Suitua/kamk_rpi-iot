RPI IoT project

RPI IoT project is Internet of Things example project made using Arduino and Raspberry Pi.
The project consist of three sub projects are the Arduino node, RPI Gateway and the RPI UI.

Arduino Node project is for the Arduino sensor node that will send the measurement data via RF24 transmitter to Raspberry Pi. 
The Arduino node has only been tested Arduino Nano but it should work on other common models such as Arduino Uno.

The RPI Gateway listens for the RF24 and relays the data to ThingSpeak cloud service. The RF24 node has only been tested on
raspberry Pi Model B+ and Raspberry Pi 2 Model B, both running Raspbian (Debian Wheezy).

Raspberry Pi UI (Qt_Quick_Housemonitor) is the user interface that displays the data measured by Arduino node(s). 
The UI is meant to run on Raspberry Pi equipped with a small LCD display. The UI is made using the Qt creator IDE.

/***************************************************************************************************************************/

WHAT IS REQUIRED TO RUN THE PROJECT:

Raspberry Pi (pref. Model B+ or newer) with a working Internet connection.
Small touch screen for Raspberry Pi or an ordinary screen and a mouse.
Arduino prototyping platform
Two RF24 (nRF24L01) radios
Arduino IDE.
PC with a Qt Creator Ide and a crosscompiler that can produce binaries for Raspberry
or 
you can compile the Raspberry projects on RPI but it takes some time.

Links:
https://www.arduino.cc/en/Main/Software
http://www.qt.io/ide/
https://wiki.qt.io/RaspberryPi_Beginners_Guide
