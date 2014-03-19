#include <stdio.h>
#include <usb.h>
     
#define VENDOR_ID	0x16c0
#define DEVICE_ID	0x03e8

#define BUFFER_SIZE 250

#if BUFFER_SIZE > 254
	#error "BUFFER_SIZE could not be more than 254"
#endif

void main() {
		usb_dev_handle  *handle = NULL;
 		unsigned char buffer[BUFFER_SIZE]={0}, i;
 		struct usb_bus      *bus;
		struct usb_device   *dev;
		int                 errorCode = -1, n=0;

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
    
    do {
    	errorCode = usb_control_msg(handle, ((1 & 1) << 7) | ((1 & 3) << 5), 0x33, 0, 0, buffer, BUFFER_SIZE, 0);
 			for(i=0;i<BUFFER_SIZE;i++) printf("%u\n", buffer[i]);
 		} while(errorCode > 0);
 		
 		usb_close(handle);
 		fprintf(stderr, "connection closed!\n");
}


