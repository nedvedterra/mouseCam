#include "mouseCam_2.h"

uint8_t frame[FRAME_SIZE];
uint8_t deltaXY[2];
uint8_t partOfFrame = 0;

int main(void)
{
	SetupHardware();
	GlobalInterruptEnable();
	for (;;)
	{
		if(partOfFrame == 0)
		{
			readFrame(frame);
			readOffset();
		}
		HID_Task();
		USB_USBTask();
	}
}

void SetupHardware(void)
{
	MCUSR &= ~(1 << WDRF);
	wdt_disable();
	clock_prescale_set(clock_div_1);
	driverInit();
	USB_Init();
}

void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= Endpoint_ConfigureEndpoint(GENERIC_IN_EPADDR, EP_TYPE_INTERRUPT, GENERIC_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(GENERIC_OUT_EPADDR, EP_TYPE_INTERRUPT, GENERIC_EPSIZE, 1);
}

void EVENT_USB_Device_ControlRequest(void)
{
	switch (USB_ControlRequest.bRequest)
	{
		case HID_REQ_GetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				uint8_t GenericData[OUT_REPORT_SIZE];
				CreateGenericHIDReport(GenericData);

				Endpoint_ClearSETUP();

				Endpoint_Write_Control_Stream_LE(&GenericData, sizeof(GenericData));
				Endpoint_ClearOUT();
			}

			break;
		case HID_REQ_SetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				uint8_t GenericData[IN_REPORT_SIZE];

				Endpoint_ClearSETUP();

				Endpoint_Read_Control_Stream_LE(&GenericData, sizeof(GenericData));
				Endpoint_ClearIN();

				ProcessGenericHIDReport(GenericData);
			}

			break;
	}
}

void ProcessGenericHIDReport(uint8_t* DataArray)
{

}
void CreateGenericHIDReport(uint8_t* DataArray)
{
	if(partOfFrame < NUMBER_OF_FRAMEPARTS - 1)
	{
		DataArray[OUT_REPORT_SIZE - 1] = partOfFrame;
		for(int i = 0; i < OUT_REPORT_SIZE - 1; i++)
			DataArray[i] = frame[(OUT_REPORT_SIZE - 1) * partOfFrame + i];
		partOfFrame++;
	}
	else
	{
		DataArray[OUT_REPORT_SIZE - 1] = partOfFrame;
		DataArray[0] = deltaXY[0];
		DataArray[1] = deltaXY[1];
		partOfFrame = 0;
	}
}

void HID_Task(void)
{
	if (USB_DeviceState != DEVICE_STATE_Configured)
	  return;

	Endpoint_SelectEndpoint(GENERIC_OUT_EPADDR);

	if (Endpoint_IsOUTReceived())
	{
		if (Endpoint_IsReadWriteAllowed())
		{
			uint8_t GenericData[IN_REPORT_SIZE];
			Endpoint_Read_Stream_LE(&GenericData, sizeof(GenericData), NULL);
			ProcessGenericHIDReport(GenericData);
		}
		Endpoint_ClearOUT();
	}

	Endpoint_SelectEndpoint(GENERIC_IN_EPADDR);

	if (Endpoint_IsINReady())
	{
		uint8_t GenericData[OUT_REPORT_SIZE];
		CreateGenericHIDReport(GenericData);
		Endpoint_Write_Stream_LE(&GenericData, sizeof(GenericData), NULL);
		Endpoint_ClearIN();
	}
}
uint8_t readFromSensor(uint8_t address)
{
	DDRD |= (1 << SDIO);

	for(uint8_t i = 1 << 7; i > 0; i >>= 1)
	{
		PORTD &= ~(1 << SCLK);
		if((address & i))
			PORTD |= (1 << SDIO);
		else
			PORTD &= ~(1 << SDIO);
		_delay_us(1);
		PORTD |= 1 << SCLK;
		_delay_us(1);
	}
	_delay_us(110);

	DDRD &= ~(1 << SDIO);
	PORTD &= ~(1 << SDIO);

	uint8_t res = 0;
  	for (uint8_t i = 1 << 7; i > 0; i >>= 1) 
	{
		PORTD &= ~(1 << SCLK);
		_delay_us(1);
    	PORTD |= 1 << SCLK;
		_delay_us(1);
    	if((PIND & (1 << SDIO)))
      		res |= i;
	}
  	return res;
}

void writeToSensor(uint8_t data, uint8_t address)
{
	address |= 1 << 7;
  	DDRD |= (1 << SDIO);

  	for (uint8_t i = 1 << 7; i > 0 ; i >>= 1)
	{
		PORTD &= ~(1 << SCLK);
		if((address & i))
			PORTD |= (1 << SDIO);
		else
			PORTD &= ~(1 << SDIO);
		_delay_us(1);
		PORTD |= 1 << SCLK;
		_delay_us(1);
  	}
  	for (uint8_t i = 1 << 7; i > 0 ; i >>= 1)
	{
    	PORTD &= ~(1 << SCLK);
		if((data & i))
			PORTD |= (1 << SDIO);
		else
			PORTD &= ~(1 << SDIO);
		_delay_us(1);
		PORTD |= 1 << SCLK;
		_delay_us(1);
  	}
}
void driverInit(void) 
{
	DDRD = (1 << SCLK) | (1 << SDIO);
  	writeToSensor(0x01, CONFIG);
}

void readFrame(uint8_t* frame)
{
	writeToSensor(0x01, PIXEL_DATA);
	_delay_us(100);
	for(int i = 0; i < FRAME_SIZE; i++)
	{
		frame[i] = readFromSensor(PIXEL_DATA);
		if(!(frame[i] & (1 << 6)))
		{
			i--;
			continue;
		}
		frame[i] <<= 2;
		_delay_us(1);
	}
}

void readOffset(void)
{
	_delay_us(1);
	deltaXY[0] = readFromSensor(DELTA_X);
	_delay_us(1);
	deltaXY[1] = readFromSensor(DELTA_Y);
}
