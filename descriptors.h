#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

#define IN_REPORT_SIZE  	2
#define SCLK 				1
#define SDIO 				0
#define	CONFIG				0x00
#define DELTA_X				0x02
#define DELTA_Y				0x03
#define PIXEL_DATA			0x08

		#include <LUFA/Drivers/USB/USB.h>
		#include <avr/pgmspace.h>
		#include <mouseCamConfig.h>

		typedef struct
		{
			USB_Descriptor_Configuration_Header_t Config;
			USB_Descriptor_Interface_t            HID_Interface;
			USB_HID_Descriptor_HID_t              HID_GenericHID;
	        USB_Descriptor_Endpoint_t             HID_ReportINEndpoint;
	        USB_Descriptor_Endpoint_t             HID_ReportOUTEndpoint;
		} USB_Descriptor_Configuration_t;

		enum StringDescriptors_t
		{
		    STRING_ID_Language      = 0, 
		    STRING_ID_Manufacturer  = 1, 
		    STRING_ID_Product       = 2, 
		};

		#define GENERIC_IN_EPADDR         (ENDPOINT_DIR_IN  | 1)
		#define GENERIC_OUT_EPADDR        (ENDPOINT_DIR_OUT | 2)
		#define GENERIC_EPSIZE            8

		uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
		                                    const uint8_t wIndex,
		                                    const void** const DescriptorAddress)
		                                    ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);
#endif

