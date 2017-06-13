/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.07.01
 * \version 0.2
 * Release_flags s
 */

#include <helium/os/usbHid.h>


#ifdef _WIN32
#include <windows.h>
extern "C"{
#ifdef _CROSS_COMPILING_WIN32
  //we are cross compiling
#include <hidsdi.h>
#else
#include <ddk\hidsdi.h>
#endif
}
#include <setupapi.h>
#include <helium/os/winError.h>

#else

#ifdef USE_LIBUSB
#include <usb.h>

#elif !defined(__APPLE__)
#include <libudev.h>
#endif
//file io
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>
#include <unistd.h>
#endif


#include <helium/util/printHex.h>
#include <helium/util/mout.h>

namespace helium{


  




#ifndef _WIN32


#ifdef USE_LIBUSB


#define usbDevice   usb_dev_handle  /* use libusb's device structure */

#define USBRQ_HID_GET_REPORT    0x01
#define USBRQ_HID_SET_REPORT    0x09

#define USB_HID_REPORT_TYPE_FEATURE 3


static const int  usesReportIDs=0;
static const int  reportNumber=0;

static int usbhidGetStringAscii(usb_dev_handle *dev, int index, char *buf, int buflen)
{
char    buffer[256];
int     rval, i;

    if((rval = usb_get_string_simple(dev, index, buf, buflen)) >= 0) /* use libusb version if it works */
        return rval;
    if((rval = usb_control_msg(dev, USB_ENDPOINT_IN, USB_REQ_GET_DESCRIPTOR, (USB_DT_STRING << 8) + index, 0x0409, buffer, sizeof(buffer), 5000)) < 0)
        return rval;
    if(buffer[1] != USB_DT_STRING){
        *buf = 0;
        return 0;
    }
    if((unsigned char)buffer[0] < rval)
        rval = (unsigned char)buffer[0];
    rval /= 2;
    /* lossy conversion to ISO Latin1: */
    for(i=1;i<rval;i++){
        if(i > buflen)              /* destination buffer overflow */
            break;
        buf[i-1] = buffer[2 * i];
        if(buffer[2 * i + 1] != 0)  /* outside of ISO Latin1 range */
            buf[i-1] = '?';
    }
    buf[i-1] = 0;
    return i-1;
}


#define USBOPEN_SUCCESS         0   /* no error */
#define USBOPEN_ERR_ACCESS      1   /* not enough permissions to open device */
#define USBOPEN_ERR_NOTFOUND    3   /* device not found */
#define USBOPEN_ERR_IO_PROD          4   /* I/O error */
#define USBOPEN_ERR_IO_MAN          5   /* I/O error */


UsbHid::UsbHid(int vendorId,int productId,const char *vendorName,const char* productName){ 
  struct usb_bus      *bus;
  struct usb_device   *dev;
  usb_dev_handle      *handle = NULL;
  static int          didUsbInit = 0;
  int errorCode = USBOPEN_ERR_NOTFOUND;

  if(!didUsbInit){
    usb_init();
    didUsbInit = 1;
  }
  usb_find_busses();
  usb_find_devices();
  for(bus=usb_get_busses(); bus; bus=bus->next){
    for(dev=bus->devices; dev; dev=dev->next){
      if(dev->descriptor.idVendor == vendorId && dev->descriptor.idProduct == productId){
	char    string[256];
	int     len;
	handle = usb_open(dev); /* we need to open the device in order to query strings */
	if(!handle){
	  errorCode = USBOPEN_ERR_ACCESS;
	  continue;
	}
	if(vendorName == NULL && productName == NULL){  /* name does not matter */
	  break;
	}
	/* now check whether the names match: */
	len = usbhidGetStringAscii(handle, dev->descriptor.iManufacturer, string, sizeof(string));
	if(len < 0){
	  errorCode = USBOPEN_ERR_IO_MAN;

	}else{
	  errorCode = USBOPEN_ERR_NOTFOUND;
	  /* fprintf(stderr, "seen device from vendor ->%s<-\n", string); */
	  if(strcmp(string, vendorName) == 0){
	    len = usbhidGetStringAscii(handle, dev->descriptor.iProduct, string, sizeof(string));
	    if(len < 0){
	      errorCode = USBOPEN_ERR_IO_PROD;	      
	    }else{
	      errorCode = USBOPEN_ERR_NOTFOUND;
	      /* fprintf(stderr, "seen product ->%s<-\n", string); */
	      if(strcmp(string, productName) == 0)
		break;
	    }
	  }
	}
	usb_close(handle);
	handle = NULL;
      }
    }
        if(handle)
	  break;
  }
    if(handle != NULL){
      char dname[32] = {0};
      errorCode = 0;
      hid.device = handle;
      std::cout<<"opened device"<<std::endl;
      int retval = usb_get_driver_np(handle, 0, dname, 31);
      if (!retval){
	usb_detach_kernel_driver_np(handle, 0);
      }else{
	//throw exc::UsbException("cannot detach driver");
      }

    }else{
      std::stringstream ss;
      ss<<"opening "<<std::hex<<vendorId<<":"<<std::hex<<productId;
      if (vendorName!=NULL||productName!=NULL){
	ss<<" aka ";
	if (vendorName!=NULL){
	  ss<<vendorName<<" ";
	}
	if (productName!=NULL){
	  ss<<productName;
	}
      }
      switch(errorCode){
      case USBOPEN_ERR_ACCESS:
	throw exc::UsbException(ss.str(),"not enough permission");
      case USBOPEN_ERR_IO_MAN:
	throw exc::UsbException(ss.str(),"cannot retrieve manifacturer string");
      case USBOPEN_ERR_IO_PROD:
	throw exc::UsbException(ss.str(),"cannot retrieve product string");
      case USBOPEN_ERR_NOTFOUND:
	throw exc::UsbException(ss.str(),"not found");
      }

    }  
}


  void   UsbHid::close(){
    if(hid.device != NULL){
      usb_close((usb_dev_handle *)hid.device);
      hid.device=NULL;
    }
  }
  
 

  UsbHid::~UsbHid(){    
    close();
  } 


  int UsbHid::send(const unsigned char* buffer,int len){
    int bytesSent, reportId = buffer[0];
    
    if(!usesReportIDs){
      buffer++;   /* skip dummy report ID */
      len--;
    }

    /*
       bytesSent = usb_interrupt_write(
				       (usb_dev_handle*)hid.device,             // handle obtained with usb_open()
				  USB_ENDPOINT_OUT | 1,// identifies endpoint 1
				       (char*)buffer,             // data buffer
				  len,     // maximum amount to read
-1
				  );
    */

        bytesSent = usb_control_msg((usb_dev_handle*)hid.device, USB_TYPE_CLASS | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, USBRQ_HID_SET_REPORT, USB_HID_REPORT_TYPE_FEATURE << 8 | (reportId & 0xff), 0, (char*)buffer, len, 5000);
    if(bytesSent != len){
      if(bytesSent < 0){
	throw exc::UsbException("writing",usb_strerror());
      }
    }
    //std::cout<<"sent"<<std::endl;
    return bytesSent+(usesReportIDs?0:1);
  }


  int UsbHid::recv(unsigned char* buffer,int maxLen){
    int bytesReceived;
    if(!usesReportIDs){
        buffer++;   /* make room for dummy report ID */
        maxLen--;
    }
    //std::cout<<"maxlen is "<<maxLen<<std::endl;

    bytesReceived = usb_interrupt_read(
				       (usb_dev_handle*)hid.device,             // handle obtained with usb_open()
				  USB_ENDPOINT_IN | 1,// identifies endpoint 1
				       (char*)buffer,             // data buffer
				  maxLen,     // maximum amount to read
				       50000//timeout
				       );


    /*    bytesReceived = usb_control_msg((usb_dev_handle*)hid.device, USB_TYPE_CLASS | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USBRQ_HID_GET_REPORT, USB_HID_REPORT_TYPE_FEATURE << 8 | reportNumber, 0, (char*)buffer, maxLen, -1);*/
    if(bytesReceived < 0){
      	throw exc::UsbException("reading",usb_strerror());
    }

    if(!usesReportIDs){
        buffer[-1] = reportNumber;  /* add dummy report ID */
    }
    //std::cout<<"received "<<bytesReceived<<":";
    //printHex(buffer,bytesReceived);
    //std::cout<<std::endl;
    return bytesReceived+!usesReportIDs;

  }


#elif defined __APPLE__


  /*code from 
*******************************************************
 HIDAPI - Multi-Platform library for
 communication with HID devices.
 
 Alan Ott
 Signal 11 Software

 2010-07-03

 Copyright 2010, All Rights Reserved.
 
 At the discretion of the user of this library,
 this software may be licensed under the terms of the
 GNU Public License v3, a BSD-Style license, or the
 original HIDAPI license as outlined in the LICENSE.txt,
 LICENSE-gpl3.txt, LICENSE-bsd.txt, and LICENSE-orig.txt
 files located at the root of the source distribution.
 These files may also be found in the public source
 code repository located at:
        http://github.com/signal11/hidapi .
  */

}//ns helium
#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <CoreFoundation/CoreFoundation.h>
#include <wchar.h>
#include <locale.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>


		struct hid_device_;
		typedef struct hid_device_ hid_device; /**< opaque hidapi structure */
  struct hid_device_info {
    /** Platform-specific device path */
    char *path;
    /** Device Vendor ID */
    unsigned short vendor_id;
    /** Device Product ID */
    unsigned short product_id;
    /** Serial Number */
    wchar_t *serial_number;
    /** Device Release Number in binary-coded decimal,
	also known as Device Version Number */
    unsigned short release_number;
    /** Manufacturer String */
    wchar_t *manufacturer_string;
			/** Product string */
    wchar_t *product_string;
    /** Usage Page for this Device/Interface
	(Windows/Mac only). */
    unsigned short usage_page;
    /** Usage for this Device/Interface
	(Windows/Mac only).*/
    unsigned short usage;
    /** The USB interface which this logical device
	represents. Valid on both Linux implementations
	in all cases, and valid on the Windows implementation
	only if the device contains more than one interface. */
    int interface_number;
    
    /** Pointer to the next device */
    struct hid_device_info *next;
  };




typedef int pthread_barrierattr_t;
typedef struct pthread_barrier {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int trip_count;
} pthread_barrier_t;

static int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count)
{
	if(count == 0) {
		errno = EINVAL;
		return -1;
	}
	
	if(pthread_mutex_init(&barrier->mutex, 0) < 0) {
		return -1;
	}
	if(pthread_cond_init(&barrier->cond, 0) < 0) {
		pthread_mutex_destroy(&barrier->mutex);
		return -1;
	}
	barrier->trip_count = count;
	barrier->count = 0;

	return 0;
}

static int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
	pthread_cond_destroy(&barrier->cond);
	pthread_mutex_destroy(&barrier->mutex);
	return 0;
}

static int pthread_barrier_wait(pthread_barrier_t *barrier)
{
	pthread_mutex_lock(&barrier->mutex);
	++(barrier->count);
	if(barrier->count >= barrier->trip_count)
	{
		barrier->count = 0;
		pthread_cond_broadcast(&barrier->cond);
		pthread_mutex_unlock(&barrier->mutex);
		return 1;
	}
	else
	{
		pthread_cond_wait(&barrier->cond, &(barrier->mutex));
		pthread_mutex_unlock(&barrier->mutex);
		return 0;
	}
}


/* Implementation of wcsdup() for Mac. */
static wchar_t *dup_wcs(const wchar_t *s)
{
  size_t len = wcslen(s);
  wchar_t *ret = (wchar_t *)malloc((len+1)*sizeof(wchar_t));
  wcscpy(ret, s);
  
  return ret;
}
  

/* Linked List of input reports received from the device. */
struct input_report {
	uint8_t *data;
	size_t len;
	struct input_report *next;
};


struct hid_device_ {
	IOHIDDeviceRef device_handle;
	int blocking;
	int uses_numbered_reports;
	int disconnected;
	CFStringRef run_loop_mode;
	CFRunLoopRef run_loop;
	CFRunLoopSourceRef source;
	uint8_t *input_report_buf;
	CFIndex max_input_report_len;
	struct input_report *input_reports;

	pthread_t thread;
	pthread_mutex_t mutex; /* Protects input_reports */
	pthread_cond_t condition;
	pthread_barrier_t barrier; /* Ensures correct startup sequence */
	pthread_barrier_t shutdown_barrier; /* Ensures correct shutdown sequence */
	int shutdown_thread;
	
	hid_device *next;
};

/* Static list of all the devices open. This way when a device gets
   disconnected, its hid_device structure can be marked as disconnected
   from hid_device_removal_callback(). */


static 	IOHIDManagerRef hid_mgr = 0x0;
static hid_device *device_list = NULL;
static pthread_mutex_t device_list_mutex = PTHREAD_MUTEX_INITIALIZER;


  int hid_exit(void)
  {
    if (hid_mgr) {
      /* Close the HID manager. */
      IOHIDManagerClose(hid_mgr, kIOHIDOptionsTypeNone);
      CFRelease(hid_mgr);
      hid_mgr = NULL;
    }
    
    return 0;
}

static int return_data(hid_device *dev, unsigned char *data, size_t length)
{
	/* Copy the data out of the linked list item (rpt) into the
	   return buffer (data), and delete the liked list item. */
	struct input_report *rpt = dev->input_reports;
	size_t len = (length < rpt->len)? length: rpt->len;
	memcpy(data, rpt->data, len);
	dev->input_reports = rpt->next;
	free(rpt->data);
	free(rpt);
	return len;
}


static void free_hid_device(hid_device *dev)
{
	if (!dev)
		return;
	
	/* Delete any input reports still left over. */
	struct input_report *rpt = dev->input_reports;
	while (rpt) {
		struct input_report *next = rpt->next;
		free(rpt->data);
		free(rpt);
		rpt = next;
	}

	/* Free the string and the report buffer. The check for NULL
	   is necessary here as CFRelease() doesn't handle NULL like
	   free() and others do. */
	if (dev->run_loop_mode)
		CFRelease(dev->run_loop_mode);
	if (dev->source)
		CFRelease(dev->source);
	free(dev->input_report_buf);

	/* Clean up the thread objects */
	pthread_barrier_destroy(&dev->shutdown_barrier);
	pthread_barrier_destroy(&dev->barrier);
	pthread_cond_destroy(&dev->condition);
	pthread_mutex_destroy(&dev->mutex);

	/* Remove it from the device list. */
	pthread_mutex_lock(&device_list_mutex);
	hid_device *d = device_list;
	if (d == dev) {
		device_list = d->next;
	}
	else {
		while (d) {
			if (d->next == dev) {
				d->next = d->next->next;
				break;
			}
			
			d = d->next;
		}
	}
	pthread_mutex_unlock(&device_list_mutex);

	/* Free the structure itself. */
	free(dev);
}




static int init_hid_manager(void)
{
	IOReturn res;
	
	/* Initialize all the HID Manager Objects */
	hid_mgr = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
	IOHIDManagerSetDeviceMatching(hid_mgr, NULL);
	IOHIDManagerScheduleWithRunLoop(hid_mgr, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
	res = IOHIDManagerOpen(hid_mgr, kIOHIDOptionsTypeNone);
	return (res == kIOReturnSuccess)? 0: -1;
}



int  hid_init(void)
{
  if (!hid_mgr) {
    if (init_hid_manager() < 0) {
      hid_exit();
      return -1;
    }
  }
  return 0;
}

void hid_close(hid_device *dev)
{
	if (!dev)
		return;

	/* Disconnect the report callback before close. */
	if (!dev->disconnected) {
		IOHIDDeviceRegisterInputReportCallback(
			dev->device_handle, dev->input_report_buf, dev->max_input_report_len,
			NULL, dev);
		IOHIDManagerRegisterDeviceRemovalCallback(hid_mgr, NULL, dev);
		IOHIDDeviceUnscheduleFromRunLoop(dev->device_handle, dev->run_loop, dev->run_loop_mode);
		IOHIDDeviceScheduleWithRunLoop(dev->device_handle, CFRunLoopGetMain(), kCFRunLoopDefaultMode);
	}
	
	/* Cause read_thread() to stop. */
	dev->shutdown_thread = 1;
	
	/* Wake up the run thread's event loop so that the thread can exit. */
	CFRunLoopSourceSignal(dev->source);
	CFRunLoopWakeUp(dev->run_loop);
	
	/* Notify the read thread that it can shut down now. */
	pthread_barrier_wait(&dev->shutdown_barrier);

	/* Wait for read_thread() to end. */
	pthread_join(dev->thread, NULL);

	/* Close the OS handle to the device, but only if it's not
	   been unplugged. If it's been unplugged, then calling
	   IOHIDDeviceClose() will crash. */
	if (!dev->disconnected) {
		IOHIDDeviceClose(dev->device_handle, kIOHIDOptionsTypeNone);
	}
	
	/* Clear out the queue of received reports. */
	pthread_mutex_lock(&dev->mutex);
	while (dev->input_reports) {
		return_data(dev, NULL, 0);
	}
	pthread_mutex_unlock(&dev->mutex);

	free_hid_device(dev);
}




static int32_t get_int_property(IOHIDDeviceRef device, CFStringRef key)
{
	CFTypeRef ref;
	int32_t value;
	
	ref = IOHIDDeviceGetProperty(device, key);
	if (ref) {
		if (CFGetTypeID(ref) == CFNumberGetTypeID()) {
			CFNumberGetValue((CFNumberRef) ref, kCFNumberSInt32Type, &value);
			return value;
		}
	}
	return 0;
}



static int get_string_property(IOHIDDeviceRef device, CFStringRef prop, wchar_t *buf, size_t len)
{
  CFStringRef str = (CFStringRef)IOHIDDeviceGetProperty(device, prop);

	buf[0] = 0x0000;

	if (str) {
		CFRange range;
		range.location = 0;
		range.length = len;
		CFIndex used_buf_len;
		CFStringGetBytes(str,
			range,
			kCFStringEncodingUTF32LE,
			(char)'?',
			FALSE,
			(UInt8*)buf,
			len,
			&used_buf_len);
		buf[len-1] = 0x00000000;
		return used_buf_len;
	}
	else
	  return 0;
		
}


static int get_string_property_utf8(IOHIDDeviceRef device, CFStringRef prop, char *buf, size_t len)
{
  CFStringRef str = (CFStringRef)IOHIDDeviceGetProperty(device, prop);

	buf[0] = 0x0000;

	if (str) {
		CFRange range;
		range.location = 0;
		range.length = len;
		CFIndex used_buf_len;
		CFStringGetBytes(str,
			range,
			kCFStringEncodingUTF8,
			(char)'?',
			FALSE,
			(UInt8*)buf,
			len,
			&used_buf_len);
		buf[len-1] = 0x00000000;
		return used_buf_len;
	}
	else
		return 0;
		
}

static unsigned short get_vendor_id(IOHIDDeviceRef device)
{
	return get_int_property(device, CFSTR(kIOHIDVendorIDKey));
}

static unsigned short get_product_id(IOHIDDeviceRef device)
{
	return get_int_property(device, CFSTR(kIOHIDProductIDKey));
}


static int32_t get_max_report_length(IOHIDDeviceRef device)
{
	return get_int_property(device, CFSTR(kIOHIDMaxInputReportSizeKey));
}


static int get_serial_number(IOHIDDeviceRef device, wchar_t *buf, size_t len)
{
	return get_string_property(device, CFSTR(kIOHIDSerialNumberKey), buf, len);
}

static int get_manufacturer_string(IOHIDDeviceRef device, wchar_t *buf, size_t len)
{
	return get_string_property(device, CFSTR(kIOHIDManufacturerKey), buf, len);
}

static int get_product_string(IOHIDDeviceRef device, wchar_t *buf, size_t len)
{
	return get_string_property(device, CFSTR(kIOHIDProductKey), buf, len);
}


static int make_path(IOHIDDeviceRef device, char *buf, size_t len)
{
	int res;
	unsigned short vid, pid;
	char transport[32];

	buf[0] = '\0';

	res = get_string_property_utf8(
		device, CFSTR(kIOHIDTransportKey),
		transport, sizeof(transport));
	
	if (!res)
		return -1;

	vid = get_vendor_id(device);
	pid = get_product_id(device);

	res = snprintf(buf, len, "%s_%04hx_%04hx_%p",
	                   transport, vid, pid, device);
	
	
	buf[len-1] = '\0';
	return res+1;
}





static hid_device *new_hid_device(void){
  hid_device *dev = (hid_device*)calloc(1, sizeof(hid_device));
	dev->device_handle = NULL;
	dev->blocking = 1;
	dev->uses_numbered_reports = 0;
	dev->disconnected = 0;
	dev->run_loop_mode = NULL;
	dev->run_loop = NULL;
	dev->source = NULL;
	dev->input_report_buf = NULL;
	dev->input_reports = NULL;
	dev->shutdown_thread = 0;
	dev->next = NULL;

	/* Thread objects */
	pthread_mutex_init(&dev->mutex, NULL);
	pthread_cond_init(&dev->condition, NULL);
	pthread_barrier_init(&dev->barrier, NULL, 2);
	pthread_barrier_init(&dev->shutdown_barrier, NULL, 2);
	
	/* Add the new record to the device_list. */
	pthread_mutex_lock(&device_list_mutex);
	if (!device_list)
		device_list = dev;
	else {
		hid_device *d = device_list;
		while (d) {
			if (!d->next) {
				d->next = dev;
				break;
			}
			d = d->next;
		}
	}
	pthread_mutex_unlock(&device_list_mutex);
	
	return dev;
}






struct hid_device_info  *hid_enumerate(unsigned short vendor_id, unsigned short product_id)
{
	struct hid_device_info *root = NULL; // return object
	struct hid_device_info *cur_dev = NULL;
	CFIndex num_devices;
	int i;
	
	setlocale(LC_ALL,"");

	/* Set up the HID Manager if it hasn't been done */
	hid_init();
	
	/* Get a list of the Devices */
	CFSetRef device_set = IOHIDManagerCopyDevices(hid_mgr);

	/* Convert the list into a C array so we can iterate easily. */	
	num_devices = CFSetGetCount(device_set);
	IOHIDDeviceRef *device_array = (IOHIDDeviceRef *)calloc(num_devices, sizeof(IOHIDDeviceRef));
	CFSetGetValues(device_set, (const void **) device_array);

	/* Iterate over each device, making an entry for it. */	
	for (i = 0; i < num_devices; i++) {
		unsigned short dev_vid;
		unsigned short dev_pid;
		#define BUF_LEN 256
		wchar_t buf[BUF_LEN];
		char cbuf[BUF_LEN];

		IOHIDDeviceRef dev = device_array[i];

        if (!dev) {
            continue;
        }
		dev_vid = get_vendor_id(dev);
		dev_pid = get_product_id(dev);

		/* Check the VID/PID against the arguments */
		if ((vendor_id == 0x0 && product_id == 0x0) ||
		    (vendor_id == dev_vid && product_id == dev_pid)) {
			struct hid_device_info *tmp;
			size_t len;

		    	/* VID/PID match. Create the record. */
			tmp = (hid_device_info*)malloc(sizeof(struct hid_device_info));
			if (cur_dev) {
				cur_dev->next = tmp;
			}
			else {
				root = tmp;
			}
			cur_dev = tmp;

			// Get the Usage Page and Usage for this device.
			cur_dev->usage_page = get_int_property(dev, CFSTR(kIOHIDPrimaryUsagePageKey));
			cur_dev->usage = get_int_property(dev, CFSTR(kIOHIDPrimaryUsageKey));

			/* Fill out the record */
			cur_dev->next = NULL;
			len = make_path(dev, cbuf, sizeof(cbuf));
			cur_dev->path = strdup(cbuf);

			/* Serial Number */
			get_serial_number(dev, buf, BUF_LEN);
			cur_dev->serial_number = dup_wcs(buf);

			/* Manufacturer and Product strings */
			get_manufacturer_string(dev, buf, BUF_LEN);
			cur_dev->manufacturer_string = dup_wcs(buf);
			get_product_string(dev, buf, BUF_LEN);
			cur_dev->product_string = dup_wcs(buf);
			
			/* VID/PID */
			cur_dev->vendor_id = dev_vid;
			cur_dev->product_id = dev_pid;

			/* Release Number */
			cur_dev->release_number = get_int_property(dev, CFSTR(kIOHIDVersionNumberKey));

			/* Interface Number (Unsupported on Mac)*/
			cur_dev->interface_number = -1;
		}
	}
	
	free(device_array);
	CFRelease(device_set);
	
	return root;
}

void  hid_free_enumeration(struct hid_device_info *devs)
{
	/* This function is identical to the Linux version. Platform independent. */
	struct hid_device_info *d = devs;
	while (d) {
		struct hid_device_info *next = d->next;
		free(d->path);
		free(d->serial_number);
		free(d->manufacturer_string);
		free(d->product_string);
		free(d);
		d = next;
	}
}


static void hid_report_callback(void *context, IOReturn result, void *sender,
                         IOHIDReportType report_type, uint32_t report_id,
                         uint8_t *report, CFIndex report_length)
{
	struct input_report *rpt;
	hid_device *dev = (hid_device*)context;

	/* Make a new Input Report object */
	rpt = (input_report*)calloc(1, sizeof(struct input_report));
	rpt->data = (uint8_t*)calloc(1, report_length);
	memcpy(rpt->data, report, report_length);
	rpt->len = report_length;
	rpt->next = NULL;

	/* Lock this section */
	pthread_mutex_lock(&dev->mutex);
	
	/* Attach the new report object to the end of the list. */
	if (dev->input_reports == NULL) {
		/* The list is empty. Put it at the root. */
		dev->input_reports = rpt;
	}
	else {
		/* Find the end of the list and attach. */
		struct input_report *cur = dev->input_reports;
		int num_queued = 0;
		while (cur->next != NULL) {
			cur = cur->next;
			num_queued++;
		}
		cur->next = rpt;

		/* Pop one off if we've reached 30 in the queue. This
		   way we don't grow forever if the user never reads
		   anything from the device. */
		if (num_queued > 30) {
			return_data(dev, NULL, 0);
		}
	}

	/* Signal a waiting thread that there is data. */
	pthread_cond_signal(&dev->condition);

	/* Unlock */
	pthread_mutex_unlock(&dev->mutex);

}


static void hid_device_removal_callback(void *context, IOReturn result,
                                        void *sender, IOHIDDeviceRef dev_ref)
{
	/* Stop the Run Loop for this device. */
	pthread_mutex_lock(&device_list_mutex);
	hid_device *d = device_list;
	while (d) {
		if (d->device_handle == dev_ref) {
			d->disconnected = 1;
			CFRunLoopStop(d->run_loop);
		}
		
		d = d->next;
	}
	pthread_mutex_unlock(&device_list_mutex);
}


static void perform_signal_callback(void *context)
{
  hid_device *dev = (hid_device*)context;
  CFRunLoopStop(dev->run_loop); //TODO: CFRunLoopGetCurrent()
}


static void *read_thread(void *param)
{
  hid_device *dev = (hid_device*)param;
	
	/* Move the device's run loop to this thread. */
	IOHIDDeviceScheduleWithRunLoop(dev->device_handle, CFRunLoopGetCurrent(), dev->run_loop_mode);

	/* Create the RunLoopSource which is used to signal the
	   event loop to stop when hid_close() is called. */
	CFRunLoopSourceContext ctx;
	memset(&ctx, 0, sizeof(ctx));
	ctx.version = 0;
	ctx.info = dev;
	ctx.perform = &perform_signal_callback;
	dev->source = CFRunLoopSourceCreate(kCFAllocatorDefault, 0/*order*/, &ctx);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), dev->source, dev->run_loop_mode);
	
	/* Store off the Run Loop so it can be stopped from hid_close()
	   and on device disconnection. */
	dev->run_loop = CFRunLoopGetCurrent();

	/* Notify the main thread that the read thread is up and running. */
	pthread_barrier_wait(&dev->barrier);

	/* Run the Event Loop. CFRunLoopRunInMode() will dispatch HID input
	   reports into the hid_report_callback(). */
	SInt32 code;
	while (!dev->shutdown_thread && !dev->disconnected) {
		code = CFRunLoopRunInMode(dev->run_loop_mode, 1000/*sec*/, FALSE);
		/* Return if the device has been disconnected */
		if (code == kCFRunLoopRunFinished) {
			dev->disconnected = 1;
			break;
		}


		/* Break if The Run Loop returns Finished or Stopped. */
		if (code != kCFRunLoopRunTimedOut &&
		    code != kCFRunLoopRunHandledSource) {
			/* There was some kind of error. Setting
			   shutdown seems to make sense, but
			   there may be something else more appropriate */
			dev->shutdown_thread = 1;
			break;
		}
	}

	/* Now that the read thread is stopping, Wake any threads which are
	   waiting on data (in hid_read_timeout()). Do this under a mutex to
	   make sure that a thread which is about to go to sleep waiting on
	   the condition acutally will go to sleep before the condition is
	   signaled. */
	pthread_mutex_lock(&dev->mutex);
	pthread_cond_broadcast(&dev->condition);
	pthread_mutex_unlock(&dev->mutex);

	/* Close the OS handle to the device, but only if it's not
	   been unplugged. If it's been unplugged, then calling
	   IOHIDDeviceClose() will crash. */
	if (!dev->disconnected) {
		IOHIDDeviceClose(dev->device_handle, kIOHIDOptionsTypeNone);
	}
	
	/* Wait here until hid_close() is called and makes it past
	   the call to CFRunLoopWakeUp(). This thread still needs to
	   be valid when that function is called on the other thread. */
	pthread_barrier_wait(&dev->shutdown_barrier);

	return NULL;
}


static int cond_wait(const hid_device *dev, pthread_cond_t *cond, pthread_mutex_t *mutex)
{
	while (!dev->input_reports) {
		int res = pthread_cond_wait(cond, mutex);
		if (res != 0)
			return res;

		/* A res of 0 means we may have been signaled or it may
		   be a spurious wakeup. Check to see that there's acutally
		   data in the queue before returning, and if not, go back
		   to sleep. See the pthread_cond_timedwait() man page for
		   details. */
		
		if (dev->shutdown_thread || dev->disconnected)
			return -1;
	}
	
	return 0;
}

static int cond_timedwait(const hid_device *dev, pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime)
{
	while (!dev->input_reports) {
		int res = pthread_cond_timedwait(cond, mutex, abstime);
		if (res != 0)
			return res;

		/* A res of 0 means we may have been signaled or it may
		   be a spurious wakeup. Check to see that there's acutally
		   data in the queue before returning, and if not, go back
		   to sleep. See the pthread_cond_timedwait() man page for
		   details. */
		
		if (dev->shutdown_thread || dev->disconnected)
			return -1;
	}
	
	return 0;

}

int hid_read_timeout(hid_device *dev, unsigned char *data, size_t length, int milliseconds)
{
	int bytes_read = -1;

	/* Lock the access to the report list. */
	pthread_mutex_lock(&dev->mutex);
	
	/* There's an input report queued up. Return it. */
	if (dev->input_reports) {
		/* Return the first one */
		bytes_read = return_data(dev, data, length);
		goto ret;
	}

	/* Return if the device has been disconnected. */
	if (dev->disconnected) {
		bytes_read = -1;
		goto ret;
	}
	
	if (dev->shutdown_thread) {
		/* This means the device has been closed (or there
		   has been an error. An error code of -1 should
		   be returned. */
		bytes_read = -1;
		goto ret;
	}

	/* There is no data. Go to sleep and wait for data. */
	
	if (milliseconds == -1) {
		/* Blocking */
		int res;
		res = cond_wait(dev, &dev->condition, &dev->mutex);
		if (res == 0)
			bytes_read = return_data(dev, data, length);
		else {
			/* There was an error, or a device disconnection. */
			bytes_read = -1;
		}
	}
	else if (milliseconds > 0) {
		/* Non-blocking, but called with timeout. */
		int res;
		struct timespec ts;
		struct timeval tv;
		gettimeofday(&tv, NULL);
		TIMEVAL_TO_TIMESPEC(&tv, &ts);
		ts.tv_sec += milliseconds / 1000;
		ts.tv_nsec += (milliseconds % 1000) * 1000000;
		if (ts.tv_nsec >= 1000000000L) {
			ts.tv_sec++;
			ts.tv_nsec -= 1000000000L;
		}
		
		res = cond_timedwait(dev, &dev->condition, &dev->mutex, &ts);
		if (res == 0)
			bytes_read = return_data(dev, data, length);
		else if (res == ETIMEDOUT)
			bytes_read = 0;
		else
			bytes_read = -1;
	}
	else {
		/* Purely non-blocking */
		bytes_read = 0;
	}

ret:
	/* Unlock */
	pthread_mutex_unlock(&dev->mutex);
	return bytes_read;
}



static int set_report(hid_device *dev, IOHIDReportType type, const unsigned char *data, size_t length)
{
	const unsigned char *data_to_send;
	size_t length_to_send;
	IOReturn res;

	/* Return if the device has been disconnected. */
   	if (dev->disconnected)
   		return -1;

	if (data[0] == 0x0) {
		/* Not using numbered Reports.
		   Don't send the report number. */
		data_to_send = data+1;
		length_to_send = length-1;
	}
	else {
		/* Using numbered Reports.
		   Send the Report Number */
		data_to_send = data;
		length_to_send = length;
	}
	
	if (!dev->disconnected) {
		res = IOHIDDeviceSetReport(dev->device_handle,
					   type,
					   data[0], /* Report ID*/
					   data_to_send, length_to_send);
	
		if (res == kIOReturnSuccess) {
			return length;
		}
		else
			return -1;
	}
	
	return -1;
}


hid_device *  hid_open_path(const char *path)
{
  	int i;
	hid_device *dev = NULL;
	CFIndex num_devices;
	
	dev = new_hid_device();

	/* Set up the HID Manager if it hasn't been done */
	hid_init();

	CFSetRef device_set = IOHIDManagerCopyDevices(hid_mgr);
	
	num_devices = CFSetGetCount(device_set);
	IOHIDDeviceRef *device_array = (IOHIDDeviceRef *)calloc(num_devices, sizeof(IOHIDDeviceRef));
	CFSetGetValues(device_set, (const void **) device_array);	
	for (i = 0; i < num_devices; i++) {
		char cbuf[BUF_LEN];
		size_t len;
		IOHIDDeviceRef os_dev = device_array[i];
		
		len = make_path(os_dev, cbuf, sizeof(cbuf));
		if (!strcmp(cbuf, path)) {
			// Matched Paths. Open this Device.
			IOReturn ret = IOHIDDeviceOpen(os_dev, kIOHIDOptionsTypeNone);
			if (ret == kIOReturnSuccess) {
				char str[32];

				free(device_array);
				CFRelease(device_set);
				dev->device_handle = os_dev;
				
				/* Create the buffers for receiving data */
				dev->max_input_report_len = (CFIndex) get_max_report_length(os_dev);
				dev->input_report_buf = (uint8_t*)calloc(dev->max_input_report_len, sizeof(uint8_t));
				
				/* Create the Run Loop Mode for this device.
				   printing the reference seems to work. */
				sprintf(str, "HIDAPI_%p", os_dev);
				dev->run_loop_mode = 
					CFStringCreateWithCString(NULL, str, kCFStringEncodingASCII);
				
				/* Attach the device to a Run Loop */
				IOHIDDeviceRegisterInputReportCallback(
					os_dev, dev->input_report_buf, dev->max_input_report_len,
					&hid_report_callback, dev);
				IOHIDManagerRegisterDeviceRemovalCallback(hid_mgr, hid_device_removal_callback, NULL);
				
				/* Start the read thread */
				pthread_create(&dev->thread, NULL, read_thread, dev);

				/* Wait here for the read thread to be initialized. */
				pthread_barrier_wait(&dev->barrier);
				
				return dev;
			}
			else {
				goto return_error;
			}
		}
	}

return_error:
	free(device_array);
	CFRelease(device_set);
	free_hid_device(dev);
	return NULL;
}


bool matches(const char* s, wchar_t* w){
  if (s==NULL) return true;
  wchar_t buffer[256];
  mbstowcs(buffer,s,257);
  return wcscmp(buffer,w)==0;
}


namespace helium{

  UsbHid::UsbHid(int vendorId,int productId,const char *manufacturer, const char* product){    


    struct hid_device_info *cur_dev,*devs;
    cur_dev = devs=hid_enumerate(vendorId, productId);
    const char* path_to_open=NULL;
   
    while (cur_dev) {
      //std::cout<<"found "<<std::hex<<cur_dev->vendor_id<<" "<<cur_dev->product_id<<std::endl;
      if (cur_dev->vendor_id==vendorId&&cur_dev->product_id==productId
	  &&matches(manufacturer,cur_dev->manufacturer_string)
	  &&matches(product,cur_dev->product_string)
	  ){
	path_to_open=cur_dev->path;
	break;
      }
      cur_dev=cur_dev->next;
    }
    if (path_to_open){      
      hid.device=hid_open_path(path_to_open);
      std::cout<<"path to open is "<<path_to_open<<" and device is "<<hid.device<<std::endl;
      hid_free_enumeration(devs);
    }else{
      std::stringstream ss;
      ss<<"Device "<<std::hex<<vendorId<<":"<<std::hex<<productId;
      if (manufacturer!=NULL||product!=NULL){
	ss<<" aka ";
	if (manufacturer!=NULL){
	  ss<<manufacturer<<" ";
	}
	if (product!=NULL){
	  ss<<product;
	}
      }
      ss<<" not found";
      hid_free_enumeration(devs);    
      throw exc::UsbException(ss.str());
    }
    
   
  } 


void UsbHid::close(){
    if (hid.device){
      hid_close((hid_device*)hid.device);
      hid.device=NULL;
    }
  }
  

  UsbHid::~UsbHid(){
    close();
  }


  int UsbHid::send(const unsigned char* buff,int size){
    int r=set_report((hid_device*)hid.device, kIOHIDReportTypeOutput, buff, size);
    //std::cout<<"sent"<<r<<std::endl;
    return r;
  }


  int UsbHid::recv(unsigned char* buff,int size){
    hid_device* dev=(hid_device*)hid.device;
    //std::cout<<"asking for "<<size<<std::endl;
    int r=hid_read_timeout(dev, buff, size, (dev->blocking)? -1: 0);
    //std::cout<<" got "<<r<<std::endl;
    //printHex(buff,size);
    //std::cout<<std::endl;
    return r;
  }







#else







  class UdevData{
  public:
    
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device *dev, *dev_int;
    



    UdevData():udev(0),enumerate(0),dev(0)
    {
    }

    ~UdevData(){
      if (dev){
	udev_device_unref(dev);
      }
      if (enumerate){
	udev_enumerate_unref(enumerate);
      }
      if (udev){
	udev_unref(udev);
      }
    }
  };

UsbHid::UsbHid(int vendorId,int productId,const char *manufacturer,const char* product){ 
  UdevData u;



  u.udev = udev_new();
  if (!u.udev){
    throw exc::UsbException("error in udev_new");
  }
  u.enumerate = udev_enumerate_new(u.udev);
  udev_enumerate_add_match_subsystem(u.enumerate, "hidraw");
  udev_enumerate_scan_devices(u.enumerate);
  u.devices = udev_enumerate_get_list_entry(u.enumerate);
  udev_list_entry_foreach(u.dev_list_entry, u.devices) {
    const char *sys;

    
    sys = udev_list_entry_get_name(u.dev_list_entry);
    u.dev = udev_device_new_from_syspath(u.udev, sys);

    //this must be taken here, because ater u.dev is overwritten
    const char* filename=udev_device_get_devnode(u.dev);
    

    u.dev_int = udev_device_get_parent_with_subsystem_devtype(
                    u.dev, "usb", "usb_interface");
    if (!u.dev_int){
      throw exc::UsbException("error in udev_device_get_parent_with_subsystem_devtype");
    }
    //intnum = udev_device_get_sysattr_value(u.dev_int,"bInterfaceNumber");
    //interface = udev_device_get_sysattr_value(u.dev_int,"interface");

    u.dev = udev_device_get_parent_with_subsystem_devtype(u.dev, "usb", "usb_device");
    if (!u.dev){
      throw exc::UsbException("error in udev_device_get_parent_with_subsystem_devtype");
    }
    
    //serial = udev_device_get_sysattr_value(dev,"serial");
    //std::cout<<"found "<<std::hex<<convertHex<int>(udev_device_get_sysattr_value(u.dev, "idVendor"))<<" "<<
    //convertHex<int>(udev_device_get_sysattr_value(u.dev, "idProduct"))<<" "<<
    //udev_device_get_sysattr_value(u.dev,"manufacturer")<<" "<<
    //udev_device_get_sysattr_value(u.dev,"product")<<std::endl;
      


    if (
	(convertHex<int>(udev_device_get_sysattr_value(u.dev, "idVendor"))==vendorId)&&
	(convertHex<int>(udev_device_get_sysattr_value(u.dev, "idProduct"))==productId)&&	
	(!manufacturer||!strcmp(manufacturer,udev_device_get_sysattr_value(u.dev,"manufacturer")))&&
	(!product||!strcmp(product,udev_device_get_sysattr_value(u.dev,"product")))
	){


      hid.fd = open(filename, O_RDWR|O_SYNC);
      //std::cout<<"OPENING "<<filename<<" as "<<hid.fd<<std::endl;
      if (hid.fd<0){
	throw exc::UsbException(stringBegin()<<"Error in opening "<<filename<<stringEnd());
      }
      return;
    }
  }//foreach
  throw exc::UsbPeripheralNotFound(vendorId,productId,
				   (manufacturer?std::string(manufacturer)+" ":std::string())+
				   (product?std::string(product):"")
				   );
				   				   
}


  void UsbHid::flushInput(){   
    int flags = fcntl(hid.fd, F_GETFL, 0);
    fcntl(hid.fd, F_SETFL, flags | O_NONBLOCK);
    char buffer[256];
    int r;
    do{
      r=::read(hid.fd,buffer,256);
      std::cout<<"Flushed "<<r<<" bytes"<<std::endl;
    }while(r>0);
    fcntl(hid.fd, F_SETFL, flags);
  }



  void UsbHid::close(){
    if (hid.fd!=-1){
      //  std::cout<<"closing "<<hid.fd<<std::endl;
      ::close(hid.fd);
      hid.fd=-1;
    }
  }


  UsbHid::~UsbHid(){
    close();
  }



  int UsbHid::send(const unsigned char* buff,int size){
    
    //std::cout<<"sending ";
    //printHex(buff,size);
    //std::cout<<std::endl;

    int r=::write(hid.fd,buff,size);    
    if (r>=0) return r;
    throw exc::UsbWriteException(size,strerror(errno),errno);
  }//send


  

  int UsbHid::recv(unsigned char* data,int size){
    int r=::read(hid.fd,data,size);
    //std::cout<<"received "<<r<<" out of "<<size<<" from "<<hid.fd<<std::endl;
    //std::cout<<"returning "<<r<<std::endl;
    if (r>=0) return r;
    throw exc::UsbReadException(size,strerror(errno),errno);
  }

  UsbHid::Handle UsbHid::getHandle(){
    return hid;
  }


 

  
#endif //!libhid

#else


  struct InitData{
    HDEVINFO hDeviceInfo;
    PSP_DEVICE_INTERFACE_DETAIL_DATA pInterfaceDetailData;

    UsbHid::Handle& hid;
    
    bool clearHid;

    InitData(UsbHid::Handle& phid):hDeviceInfo(INVALID_HANDLE_VALUE),pInterfaceDetailData(NULL),hid(phid),clearHid(true){
      hid=INVALID_HANDLE_VALUE;
    }

    void reset(){
     
      if (pInterfaceDetailData){
	LocalFree(pInterfaceDetailData);    
	pInterfaceDetailData=0;
      }
      if (clearHid&&hid!=INVALID_HANDLE_VALUE){
	CloseHandle(hid);
	hid=INVALID_HANDLE_VALUE;
      }
    }
      
    ~InitData(){      
      reset();
      if (hDeviceInfo!=INVALID_HANDLE_VALUE){
	SetupDiDestroyDeviceInfoList(hDeviceInfo);     
      }	    
    }
      
  };



 
 
  bool isEqual(const char* target,const wchar_t* s){
    for (;target!=0&&*target!=0;++target,++s){
      if (*s>255|| (*target!=(char)(*s) )){
		 return false;
      }
    }
    return true;
  }
  //based on  http://msdn.microsoft.com/en-us/library/windows/hardware/ff540174(v=vs.85).aspx
  //and http://www.signal11.us/oss/hidapi/
  UsbHid::UsbHid(int vendorId,int productId,const char *manufacturer,const char* product){ 
  GUID guidDeviceInterface;
  HidD_GetHidGuid(&guidDeviceInterface);
  /*if (guidDeviceInterface==NULL){
    throw exc::UsbException("cannot HidD_GetHidGuid");
  }*/
  InitData ini(hid);

  BOOL bResult = TRUE;
 
  SP_DEVINFO_DATA DeviceInfoData;
  
  SP_DEVICE_INTERFACE_DATA deviceInterfaceData;

  
  ULONG requiredLength=0;
  
  //  LPTSTR lpDevicePath = NULL;
  
  DWORD index = 0;
  
  // Get information about all the installed devices for the specified
  // device interface class.
  ini.hDeviceInfo = SetupDiGetClassDevs( 
        &guidDeviceInterface,
        NULL, 
        NULL,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (ini.hDeviceInfo == INVALID_HANDLE_VALUE) 
    { 
        // ERROR 
      throw exc::UsbException("SetupDiGetClassDevs",getLastErrorMsg());
    }

    //Enumerate all the device interfaces in the device information set.
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (index = 0; SetupDiEnumDeviceInfo(ini.hDeviceInfo, index, &DeviceInfoData); index++)
    {
      
        deviceInterfaceData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);

        //Get information about the device interface.
        bResult = SetupDiEnumDeviceInterfaces( 
					      ini.hDeviceInfo,
					      &DeviceInfoData,
					      &guidDeviceInterface,
					      0, 
					      &deviceInterfaceData);

        // Check if last item
        if (GetLastError () == ERROR_NO_MORE_ITEMS)
        {
            break;
        }

        //Check for some other error
        if (!bResult) 
        {
	  throw exc::UsbException("SetupDiEnumDeviceInterfaces",getLastErrorMsg());
        }

        //Interface data is returned in SP_DEVICE_INTERFACE_DETAIL_DATA
        //which we need to allocate, so we have to call this function twice.
        //First to get the size so that we know how much to allocate
        //Second, the actual call with the allocated buffer
        
        bResult = SetupDiGetDeviceInterfaceDetail(
						  ini.hDeviceInfo,
						  &deviceInterfaceData,
						  NULL, 0,
						  &requiredLength,
						  NULL);


        //Check for some other error
        if (!bResult) 
        {
            if ((ERROR_INSUFFICIENT_BUFFER==GetLastError()) && (requiredLength>0))
            {
                //we got the size, allocate buffer
	      ini.pInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, requiredLength);
                
                if (!ini.pInterfaceDetailData) 
                { 
		  throw exc::UsbException("Error allocating memory for the device detail buffer");
                }
            }
            else
            {
	      throw exc::UsbException("SetupDiEnumDeviceInterfaces",getLastErrorMsg());
            }
        }

        //get the interface detailed data
        ini.pInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        //Now call it with the correct size and allocated buffer
        bResult = SetupDiGetDeviceInterfaceDetail(
						  ini.hDeviceInfo,
                &deviceInterfaceData,
						  ini.pInterfaceDetailData,
                requiredLength,
                NULL,
                &DeviceInfoData);
        
        //Check for some other error
        if (!bResult) 
	  {
	    throw exc::UsbException("SetupDiGetDeviceInterfaceDetail",getLastErrorMsg());
        }

        //copy device path
                
        /*size_t nLength = wcslen (pInterfaceDetailData->DevicePath) + 1;  
        lpDevicePath = (TCHAR *) LocalAlloc (LPTR, nLength * sizeof(TCHAR));
        StringCchCopy(lpDevicePath, nLength, pInterfaceDetailData->DevicePath);
        lpDevicePath[nLength-1] = 0;
                        
        printf("Device path:  %s\n", lpDevicePath);
	*/

        std::cout<<"path "<<ini.pInterfaceDetailData->DevicePath<<std::endl;
        
	hid=CreateFileA(ini.pInterfaceDetailData->DevicePath,
                        GENERIC_READ | GENERIC_WRITE,
                        //ACCESS_TYPE_NONE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL,
                        OPEN_EXISTING,
                        0*FILE_FLAG_OVERLAPPED,
                        NULL);
	if (hid==INVALID_HANDLE_VALUE) continue;

        std::cout<<"valid handle"<<std::endl;
        
	HIDD_ATTRIBUTES attributes;
	bResult = HidD_GetAttributes(hid, &attributes);

	std::cout<<"**+**"<<std::hex<<attributes.VendorID<<" "<<attributes.ProductID<<" "<<std::dec<<std::endl;

	
	

	/*HidD_GetProductString(hid,string,2048);
	for (int i=0;i<10;i++){
	 std::cout<<(int)string[i]<<" ";
	}
	std::cout<<std::endl;
	for (int i=0;i<10;i++){
	 std::cout<<(string[i]<256?((char)string[i]):'_') ;
	}

	std::cout<<std::endl;
	*/
	if (bResult&&attributes.VendorID==vendorId&&attributes.ProductID==productId){
#define BUFFERSIZE 1024
	  wchar_t tman[BUFFERSIZE],tprod[BUFFERSIZE]; 
	  
	  HidD_GetManufacturerString(hid,tman,BUFFERSIZE);
	  HidD_GetProductString(hid,tprod,BUFFERSIZE);
	  
	  if (isEqual(manufacturer,tman)&&isEqual(product,tprod)){
	    std::cout<<"Found!!"<<std::endl;
	    ini.clearHid=false;
	    return; //SUCCESS
	  }
	  	
	}//vendor product match
	ini.reset();

    }//for

    throw exc::UsbPeripheralNotFound(vendorId,productId,
				     (manufacturer?std::string(manufacturer)+" ":std::string())+
				     (product?std::string(product):std::string())
				     );


}





  int UsbHid::send(const unsigned char* buff,int size){
    DWORD bWritten;
    if (!WriteFile(hid, buff, size, &bWritten, 0)){
      throw exc::UsbWriteException(size,toString(bWritten)+" written :"+getLastErrorMsg(),GetLastError());
    }
    return bWritten;
  }

  int UsbHid::recv(unsigned char* buff,int size){
    DWORD bRead;
    if (!ReadFile(hid, buff, size, &bRead, 0)){
      throw exc::UsbReadException(size,toString(bRead)+" read :"+getLastErrorMsg(),GetLastError());
    }
    return bRead;
  }


void  UsbHid::close(){
    if (hid!=INVALID_HANDLE_VALUE){
      CloseHandle(hid);
      hid=INVALID_HANDLE_VALUE;
    }
  }

  UsbHid::~UsbHid(){
    close();
  }





#endif





}

