
#include <SPI.h>
#include "RF24.h"
#include "thingspeakrequest.h"
#include <math.h>         //loads the more advanced math functions
#include <avr/io.h>
#include <avr/interrupt.h>

#define SERIAL_DEBUG  1   //1 = On, 0 = Off

//#define INTERVAL_MSEC 900000 //15 Minutes
#define INTERVAL_MSEC 16000 //16 sec

#define TS_W_API_KEY  "0000000000000000" //Put your thingspeak write API key here (16 characters)

#define TS_W_CH_NUM  "00000"    //Put your thingspeak channel number here

//pinMode(2, INPUT);
volatile int sensorValue = 0;

int m_iBlockSize = 32;
char message[512];
float temp;
float Temp;
int val;
long int door1= 0;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7,8);
                     
byte addresses[][6] = {"1Node","4Node"};
void sendMessage(const char *msg);
float Thermister(int RawADC);

volatile int door1ChangeCount;

ThingSpeakUploadRequest uploadRequest;

void setup()
{
    door1ChangeCount = 0;
    sei(); 
    pinMode(2, INPUT);  //INT0 keskeytys
    attachInterrupt(0, INT0_handler, CHANGE);
    radio.begin();  
    
    radio.setAutoAck(1);                    // Ensure autoACK is enabled
    radio.enableAckPayload();               // Allow optional ack payloads
    radio.setRetries(0,15);                 // Smallest time between retries, max no. of retries
    radio.setPayloadSize(32);
    
    radio.openWritingPipe(addresses[1]);        // Both radios listen on the same pipes by default, but opposite addresses
    radio.openReadingPipe(1,addresses[0]);      // Open a reading pipe on address 0, pipe 1
    
    if(SERIAL_DEBUG) Serial.begin(9600);         
    
    if(SERIAL_DEBUG)Serial.print("\n"); 
}
void loop()
{ 
  
  //int val;                //Create an integer variable
  // double temp;            //Variable to hold a temperature value
  val=analogRead(0);      //Read the analog port 0 and store the value in val
  temp=Thermister(val);   //Runs the fancy math on the raw analog value
  Serial.println(temp);   //Print the value to the serial port


  uploadRequest.SetApiKey(TS_W_API_KEY);
  uploadRequest.SetFieldData(1,temp);
  //uploadRequest.SetFieldData(2,door1);  //Room 2 temp
  //uploadRequest.SetFieldData(3,random(RND_MIN,RAND_MAX));  //Room 3 temp
  uploadRequest.SetFieldData(4,door1);
  //uploadRequest.SetFieldData(5,random(RND_MIN,RAND_MAX));  //Room 2 door
  //uploadRequest.SetFieldData(6,random(RND_MIN,RAND_MAX));  //Room 3 door
  //uploadRequest.SetFieldData(7,random(RND_MIN,RAND_MAX));
  //uploadRequest.SetFieldData(8,random(RND_MIN,RAND_MAX));
  uploadRequest.ToString(message); 
  sendMessage(message);
  door1ChangeCount = 0;
 
  delay(INTERVAL_MSEC);   
}

void sendMessage(const char *msg)
{   
     unsigned int msgLen = strlen(msg);
     unsigned int fullBlocks = msgLen / m_iBlockSize;
     char lastBlock[m_iBlockSize];
     memset(lastBlock,0,sizeof(lastBlock));

     unsigned int block;

     for(block = 0; block < fullBlocks; block++)
     {
         radio.write(&msg[block*m_iBlockSize],m_iBlockSize);
     }

     strcpy(lastBlock,&msg[block*m_iBlockSize]);
     radio.write(&msg[block*m_iBlockSize],m_iBlockSize,0);
}


 
float Thermister(int RawADC) {  //Function to perform the fancy math of the Steinhart-Hart equation
 //double Temp;
 Temp = log(((10240000/RawADC) - 10000));
 Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
 Temp = Temp - 273.15 - 131.5;              // Convert Kelvin to Celsius
 //Temp = (Temp * 9.0)/ 5.0 + 32.0; // Celsius to Fahrenheit - comment out this line if you need Celsius
 return Temp;
}
 
void INT0_handler()
{
  cli();
  
  door1ChangeCount++;

  sensorValue = digitalRead(2);
  if (sensorValue)
  {
     door1 = 1;
     Serial.println("Door open"); 
  }

  else 
  {
    door1 = 0;
    Serial.println("Door closed");
  }

  if(door1ChangeCount > 1)
  {
         door1 = 1;
  }
  
  sei();
}



























 
