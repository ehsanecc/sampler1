#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */

#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include "usbdrv.h"

/************************************************************
sampler1 is an USB Device based on AVR CPU with CPU clock
16MHz and it samples it's all 8 channels in less than 5 micro seconds.
It could use as an 20kHz oscillscope( it's really less, but it's fairly good for an AVR )

i plan to make sampler2 with an FPGA and a fast 12bit ADC ( about 100MHz speed at least )
so sampler2 could use as an 10MHz oscillscope.

i'm gonna determine an interface for sampler and this interface will use in all
sampler versions ( as long as no special feature added to sampler )
*************************************************************/

uchar samplesRemaining = 0;

// Read the 8 most significant bits
// of the AD conversion result
uchar read_adc(uchar adc_input)
{
	ADMUX=adc_input | 0x60;
	_delay_us(1);
	ADCSRA|=0x40;
	while ((ADCSRA & 0x10)==0);
	ADCSRA|=0x10;
	
	return ADCH;
}
 
usbMsgLen_t usbFunctionSetup(uchar setupData[8])
{
    usbRequest_t *rq = (void *)setupData;   // cast to structured data for parsing
    
    switch(rq->bRequest){
    	case 0x33:														// request code for samples
        samplesRemaining = rq->wLength.word;// store the amount of samples requested
        return USB_NO_MSG;                  // tell driver to use usbFunctionRead()
    	}
    
    return 0;                               // ignore all unknown requests
}

uchar usbFunctionRead(uchar *samples, uchar len)
{
	uchar i;
    
	if(len > samplesRemaining)      // len is max chunk size
		len = samplesRemaining;     // send an incomplete chunk
	samplesRemaining -= len;
	for(i = 0; i < len; i++)
		samples[i] = read_adc(0); 	// copy the data to the buffer

	return len;                     // return real chunk size
}

void adcInit()
{
	// 1000.0 kHz ( 16Mhz F_CPU )
	// Reference: AVCC pin
	ADMUX=0x60;
	ADCSRA=0x84;
}


/* ------------------------------------------------------------------------- */

int __attribute__((noreturn)) main(void)
{
		uchar   i;

		adcInit();
    usbInit();
    usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
    i = 0;
    while(--i){             /* fake USB disconnect for > 250 ms */
        wdt_reset();
        _delay_ms(1);
    } usbDeviceConnect();
    sei();
    
    for(;;) usbPoll();      /* main event loop */
}

/* ------------------------------------------------------------------------- */
