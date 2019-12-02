#include <msp430.h>
#include "msprf24/msprf24.h"


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	uint8_t rxaddr[] = { 0xBA, 0xAA, 0xAD, 0xD4, 0x45 };
	uint8_t txaddr[] = { 0x13, 0x37, 0xBE, 0xEF, 0x00 };
	uint8_t *buf = (uint8_t *) malloc(32 * sizeof(uint8_t));

	rf_crc = RF24_EN_CRC | RF24_CRCO; // CRC enabled, 16 bit
	rf_addr_width = 5;
	rf_speed_power = RF24_SPEED_1MBPS | RF24_POWER_MIN;
	rf_channel = 76;
	msprf24_init();
	msprf24_set_pipe_packetsize(0, 32);
	msprf24_open_pipe(0, 1);

	msprf24_standby();
	w_tx_addr(txaddr);
	w_rx_addr(0, txaddr); // Watch out!

	while(1)
	{
	    __delay_cycles(800000);
	    buf[0] = 'G';
	    buf[1] = 'o';

	    w_tx_payload(32, buf);
	    msprf24_activate_tx();
	    LPM4; // what the hell does this do

	    if (rf_irq & RF24_IRQ_FLAGGED)
	    {
	        rf_irq &= ~RF24_IRQ_FLAGGED;
	        msprf24_irq_clear(rf_irq);
	    }
	}

	return 0;
}
