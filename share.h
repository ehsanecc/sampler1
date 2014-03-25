
#ifndef _SHARE_H_
#define _SHARE_H_

#define VENDOR_ID	0x16c0
#define DEVICE_ID	0x03e8

typedef enum {
	false=0,
	true
} bool;

typedef struct {
	unsigned int sample_rate;
	unsigned char sample_resolution;
	bool use_external_adc;
} sampler1_configure;

#define SAMPLER1_GETSAMPLES			0x11 // index is channel, length is number of samples
#define SAMPLER1_SETSAMRATE			0x22 // 
#define SAMPLER1_SETSAMRESO			0x33 // set samples resolution (2,4,6,8,10)
#define SAMPLER1_USEEXTNADC			0xff // use external adc ( if any )

#define EOL			"\n\t"

#endif
