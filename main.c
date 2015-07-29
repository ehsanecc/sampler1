/****************************************************************************************
sampler1 is an USB Device based on AVR CPU with CPU clock 16MHz and it samples it's all 8
 channels in less than 5 microseconds.
It could use as an 20Ksps oscillscope( it's really less, but it's fairly good for an AVR )

i plan to make sampler2 with an FPGA and a fast 12bit ADC ( about 100MHz speed at least )
so sampler2 could use as an 10MHz oscillscope.

i'm gonna determine an interface for sampler and this interface will use in all
sampler versions ( as long as no special feature added to sampler )
****************************************************************************************/

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */

#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include "usbdrv.h"

#include "share.h"

#define _2BIT_RESOLUTION		((ADCH&0xC0)>>6)
#define _4BIT_RESOLUTION		((ADCH&0xF0)>>4)
#define _6BIT_RESOLUTION		((ADCH&0xFC)>>2)
#define _8BIT_RESOLUTION		(ADCH)
#define _10BIT_RESOLUTION		(unsigned int)((ADCH<<2)|(ADCL>>6))

sampler1_configure config; // global config

unsigned int samplesRemaining = 0;

// delay for 16MHz clock
void __delay_us(uint16_t us) {
	asm volatile(
			"mov r25, %B0"EOL
			"L1: mov r24, %A0"EOL
			"L2: dec r24"EOL
			"nop;nop;nop;nop;"EOL
			"nop;nop;nop;nop;"EOL
			"nop;nop;nop;nop;"EOL
			"nop;nop;nop;"EOL
			"cpi r24, 0"EOL
			"brne L2"EOL
			"dec r25"EOL
			"cpi r25, 0"EOL
			"brne L1"EOL
			:: "r"(us)
			: "r24", "r25");
}

// Read the 8 most significant bits
// of the AD conversion result
unsigned int read_adc(uint8_t adc_input)
{
	ADMUX=adc_input | 0x60;
	//_delay_us(1);
	asm("nop; nop; nop; nop;"); // slight delay
	ADCSRA|=0x40;
	while ((ADCSRA & 0x10)==0);
	ADCSRA|=0x10;
	
	return ADCH;
}
 
usbMsgLen_t usbFunctionSetup(uint8_t setupData[8])
{
    usbRequest_t *rq = (void *)setupData;   	// cast to structured data for parsing
    
    switch(rq->bRequest) {
    	case SAMPLER1_USEEXTNADC:
    		config.use_external_adc = true;
	    case SAMPLER1_SETSAMRESO:
	    	config.sample_resolution = rq->wValue.word;
	    	break;
	    case SAMPLER1_SETSAMRATE:
	    	config.sample_rate = rq->wValue.word;
	    	break;
    	case SAMPLER1_GETSAMPLES:
        samplesRemaining = rq->wLength.word;
        return USB_NO_MSG; // this function should return data ( samples )
    }
    
		return 0;                               	// ignore all unknown requests
}

/* i should write this function in assembley to make it more faster */
uint8_t usbFunctionRead(uint8_t *samples, uint8_t len)
{
	register uint8_t i; // make it CPU register to get faster code execution
	
	if(len > samplesRemaining)    // len is max chunk size
		len = samplesRemaining;     // send an incomplete chunk
		
	samplesRemaining -= len;
	for(i=0;i<len;i++)
		samples[i] = 0;
	
	return len;                   // return real chunk size
}

void adcInit()
{
	// 1MHz ( @16Mhz )
	// Ref : AVCC pin
	ADMUX=0x60; // 0110 0000
	ADCSRA=0x84;// 1000 0100  [ADCEN=1] [XTAL/16]
}


/* ------------------------------------------------------------------------- */

int __attribute__((noreturn)) main(void)
{
		uint8_t i=0;
		
		config.use_external_adc = false;
		config.sample_rate = 0xffff; // maximum
		config.sample_resolution = 8; // 8 bits

		adcInit();
    usbInit();
    usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
    while(--i) { wdt_reset(); _delay_ms(1); } /* fake USB disconnect for > 250 ms */
    
    usbDeviceConnect();
    sei(); // enable interrupts
    
    while(1) usbPoll();     /* main event loop */
}

/* ------------------------------------------------------------------------- */
