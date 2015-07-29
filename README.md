sampler1
========

sampler1 is an USB Device based on AVR CPU with CPU clock 16MHz and it samples it's all 8
 channels in less than 5 microseconds.
It could use as an 20Ksps oscillscope( it's really less, but it's fairly good for an AVR )

i plan to make sampler2 with an FPGA and a fast 12bit ADC ( about 100MHz speed at least )
so sampler2 could use as an 10MHz oscillscope.

i'm gonna determine an interface for sampler and this interface will use in all
sampler versions ( as long as no special feature added to sampler )