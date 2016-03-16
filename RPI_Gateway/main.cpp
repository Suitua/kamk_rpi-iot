/*
*  Copyright (C) 2016, Timo Leinonen <timojt.leinonen@gmail.com>
*
*  Description: "The RPI_Gateway is meant for relaying data from Arduino sensor
*  nodes to ThingSpeak cloud service. The sensor nodes communicate with the gateway via RF24.
*  See the Arduino_Node project for the arduino sensor node code.
*  The gateway supports maximum of 5 different client nodes."
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*  Version 1.0
*  Author: Timo Leinonen
*/

#include <sstream>
#include <iostream>
#include "armPi/rf24receiver.h"
#include "thingspeakrequest.h"
#include "thingspeakclient.h"

using namespace std;

#define DEBUG   1		//Comment out to disable printing

int main(/*int argc, char** argv*/)
{

    ThingSpeakClient tsClient;          //ThingSpeak client for uploading the data
    ThingSpeakUploadRequest ulRequest;
    RF24Receiver rf24receiver(RPI_BPLUS_GPIO_J8_22, RPI_BPLUS_GPIO_J8_24, BCM2835_SPI_SPEED_8MHZ);

    rf24receiver.StartReceiving();

    char buf[1024];
    int iPipeNum;

    // receiver loop
    while (1)
    {

        iPipeNum = rf24receiver.GetFirstAvailablePipe();
        rf24receiver.ReadMessage(buf,1024,iPipeNum);
        ulRequest.ResetData();

        printf("Received upload request from %s: \n",rf24receiver.GetPipeAddress(iPipeNum));
        if(false == ulRequest.FromString(buf))
        {

            if(DEBUG)printf("\nFailed to parse upload request: \n");
            continue;
        }

        for(int i = 0; i < 8; i++)
        {
            tsClient.setField(i+1,ulRequest.GetFieldData(i+1));
        }

        if(false == tsClient.writeFields(ulRequest.GetApiKey()))
        {

            if(DEBUG)printf("\nFailed to  upload data: %s\n",tsClient.GetLastError());
        }

        else
            if(DEBUG)printf("\n-----Data uploaded succesfully-----\n");


    } //while 1
    return 0;

} //main




