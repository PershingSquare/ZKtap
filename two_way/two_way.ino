#include <Enrf24.h>  
#include <nRF24L01.h>  
#include <string.h>  
#include <SPI.h>  
  
Enrf24 radio(P3_4, P3_5, P3_6);  // P2.0=CE, P2.1=CSN, P2.2=IRQ  
const uint8_t rxaddr[] = { 0xBA, 0xAA, 0xAD, 0xD4, 0x45 };
const uint8_t txaddr[] = { 0x13, 0x37, 0xBE, 0xEF, 0x00 };
const char *str_illini = "ILLINI";

// Ping Out

void setup() {  
    Serial.begin(9600);  
  
    SPI.begin();  
    SPI.setDataMode(SPI_MODE0);  
    SPI.setBitOrder(MSBFIRST);  
  
    radio.begin(1000000, 76);  // Defaults 1Mbps, channel 0, max TX power
    radio.setCRC(1, 1);  
    radio.autoAck(true);
    radio.setTXaddress((void*)txaddr);
    radio.setRXaddress((void*)rxaddr);
}  
  
void loop() {    
    radio.print(str_illini);  
    radio.flush();  // Force transmit (don't wait for any more data)  
    Serial.println("Sent");
    delay(50);   
}  
