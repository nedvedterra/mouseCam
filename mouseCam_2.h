#ifndef _MOUSECAM_H_
#define _MOUSECAM_H_

		#include <avr/io.h>
		#include <util/delay.h>
		#include <avr/wdt.h>
		#include <avr/power.h>
		#include <avr/interrupt.h>
		#include <stdbool.h>
		#include <string.h>

		#include "Descriptors.h"

		#include <LUFA/Drivers/USB/USB.h>

		void SetupHardware(void);
		void HID_Task(void);

		void EVENT_USB_Device_ConfigurationChanged(void);
		void EVENT_USB_Device_ControlRequest(void);

		void ProcessGenericHIDReport(uint8_t* DataArray);
		void CreateGenericHIDReport(uint8_t* DataArray);
		uint8_t readFromSensor(uint8_t address);
		void writeToSensor(uint8_t data, uint8_t address);
		void driverInit(void);
		void readFrame(uint8_t* frame);
		void readOffset(void);

#endif
