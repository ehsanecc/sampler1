#include <stdio.h>
#include <pthread.h>
#include <usb.h>
#include "share.h"

#define BUFFER_SIZE	4096

unsigned int br = 0, bt = 0; // byte rate, bytes transferred

void trCalculator(void *ptr) { // transfer rate calculator
	unsigned int pp;
	while(1) {
		pp = bt;
		sleep(1); // 1 second sleep
		br = bt - pp;
		fprintf(stderr, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b%dKsps", br/1024);
	}
}

void main() {
	usb_dev_handle  *handle = NULL;
	pthread_t thread;
	pthread_attr_t thread_attr;
	unsigned char buffer[BUFFER_SIZE]={0}, i;
	struct usb_bus      *bus;
	struct usb_device   *dev;
	int errorCode = -1, n=0;

 	usb_init();
	usb_find_busses();
	usb_find_devices();
    
    for(bus = usb_get_busses(); bus; bus = bus->next) {
    	for(dev = bus->devices; dev; dev = dev->next) {  /* iterate over all devices on all busses */
    		if(dev->descriptor.idVendor == VENDOR_ID && dev->descriptor.idProduct == DEVICE_ID) {
    			fprintf(stderr, "device found, opening device...\n");
    			handle = usb_open(dev); /* we need to open the device in order to query strings */
    			break;
    		}
    	}
    }
    
    //usb_claim_interface(handle, 0);
    //usb_detach_kernel_driver_np(handle, 0);
    pthread_create(&thread, &thread_attr, (void*)trCalculator, NULL);
    do {
    	errorCode = usb_control_msg(handle, ((1 & 1) << 7) | ((1 & 3) << 5), 0x33, 0, 0, buffer, BUFFER_SIZE, 0);
    	if(errorCode > 0)
			bt += errorCode;
			//printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b%dsps", br);
 		/*for(i=0;i<BUFFER_SIZE;i++)
			printf("%u\n", buffer[i]);*/
 	} while(errorCode > 0);
 	printf("\nerrorCode %d %s\n", errorCode, usb_strerror());
 		
 	usb_close(handle);
 	fprintf(stderr, "connection closed!\n");
}


