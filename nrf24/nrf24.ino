#include <Enrf24.h>  
#include <nRF24L01.h>  
#include <string.h>  
#include <SPI.h>  
  
Enrf24 radio(P3_4, P3_5, P3_6);  // P2.0=CE, P2.1=CSN, P2.2=IRQ  
const uint8_t txaddr[] = { 0xE7, 0xD3, 0xF0, 0x35, 0xFF };  
  
const int sensorPin = A4;  
int sensorValue = 0;  
int dataPacket = 0;  
  
void setup() {  
    Serial.begin(9600);  
  
    SPI.begin();  
    SPI.setDataMode(SPI_MODE0);  
    SPI.setBitOrder(MSBFIRST);  
  
    radio.begin( 1000000, 76 );  // Defaults 1Mbps, channel 0, max TX power
    radio.setCRC(1, 1);  
      
    radio.setTXaddress((void*)txaddr);  
    
    // set push button  
    pinMode( P1_3, INPUT );  
    // set leds  
    pinMode( P1_0, OUTPUT );  
    // do some startup animation  
    for( int i = 0; i < 5; i++ ) {  
        digitalWrite( P1_0, HIGH );  
        delay( 250 );  
        digitalWrite( P1_0, LOW );  
        delay( 250 );  
    }  
    digitalWrite(P1_0, LOW);  
}  
  
void loop() {  
    Serial.println(dataPacket); 
      
    radio.print( 'c' );  
    radio.flush();  // Force transmit (don't wait for any more data)  
  
    delay(50);  
      
}  
