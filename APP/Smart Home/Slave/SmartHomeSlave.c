#include "StdTypes.h"
#include "MemMap.h"
/***MCAL***/
#include "DIO_Interface.h"
#include "Timer_Interface.h"
#include "SPI_Interface.h"
#include "Ex_Interrupt_Interface.h"
#include "ADC_Interface.h"
/***HAL***/
#include "LCD_Interface.h"
#include "Keypad_Interface.h"
#include "Sensors_Interface.h"
#include "RGB_Interface.h"
/***APP***/
#include "SmartHomeSlave_Interface.h"
#include "SmartHomeSlave_Cfg.h"
#include "SmartHomeSlave_Private.h"


#define F_CPU  8000000
#include <util/delay.h>
/************************************Smart Home Variables **********************************************/
static volatile data[2]={0},c=0;

/************************************Smart Home Slave **********************************************/
void SMART_HOMESlaveInit(void)
{
	DIO_Init();
	ADC_Init(VREF_AVCC,ADC_SCALER_64);
	SPI_Init();
	TIMER0_Init(TIMER0_FASTPWM_MODE,TIMER0_SCALER_8);
	TIMER0_OC0Mode(OC0_NON_INVERTING);
	
	Timer1_Init(TIMER1_FASTPWM_ICR_TOP_MODE,TIMER1_SCALER_8);
	Timer1_OCRA1Mode(OCRA_NON_INVERTING);
	Timer1_OCRB1Mode(OCRB_NON_INVERTING);
	ICR1=255;
	LCD_Init();
	SPI_SetCallBack(SPI_ReceiveInterrupt);
	SPI_InterruptEnable();
	GLOBAL_ENABLE();
}
volatile u8 TempValue,Temp=100;
void SMART_HOMESlaveRunnable(void)
{
	u16 AirFlag=0;
	LedStartValue();
	while(1)
	{
		TempValue=TEMP_Read();
		cli();
		LCD_SetCurser(1,6);
		LCD_WriteNumber(Temp);
		sei();
		if(Temp<(TempValue/10) && AirFlag==1)
		{
			DIO_WritePin(AIR_CONDITIONING,ROOM_STATUS_ON);
		}
		else if(Temp>(TempValue/10) || AirFlag==0)
		{
			DIO_WritePin(AIR_CONDITIONING,ROOM_STATUS_OFF);
		}
		
		
		if(data[0]==ROOM1)
		{
			if(data[1]==ROOM_ON)
			{
				DIO_WritePin(ROOM1_PIN,ROOM_STATUS_ON);
				InitValues();
			}
			else if(data[1]==ROOM_OFF)
			{
				DIO_WritePin(ROOM1_PIN,ROOM_STATUS_OFF);
				InitValues();
			}
		}
		else if(data[0]==ROOM2)
		{
			if(data[1]==ROOM_ON)
			{
				DIO_WritePin(ROOM2_PIN,ROOM_STATUS_ON);
				InitValues();
			}
			else if(data[1]==ROOM_OFF)
			{
				DIO_WritePin(ROOM2_PIN,ROOM_STATUS_OFF);
				InitValues();
			}
		}
		else if(data[0]==ROOM3)
		{
			if(data[1]==ROOM_ON)
			{
				DIO_WritePin(ROOM3_PIN,ROOM_STATUS_ON);
				InitValues();
			}
			else if(data[1]==ROOM_OFF)
			{
				DIO_WritePin(ROOM3_PIN,ROOM_STATUS_OFF);
				InitValues();
			}
		}
		else if(data[0]==MORE)
		{
			if(data[1]==ROOM4)
			{
				if(data[2]==ROOM_ON)
				{
					DIO_WritePin(ROOM4_PIN,ROOM_STATUS_ON);
					InitValues();
					data[2]=DEFAULT;
				}
				else if(data[2]==ROOM_OFF)
				{
					DIO_WritePin(ROOM4_PIN,ROOM_STATUS_OFF);
					InitValues();
					data[2]=DEFAULT;
				}
			}
			else if(data[1]==TV)
			{
				if(data[2]==ROOM_ON)
				{
					DIO_WritePin(TV_PIN,ROOM_STATUS_ON);
					data[0]=DEFAULT;
					data[1]=DEFAULT;
					data[2]=DEFAULT;
				}
				else if(data[2]==ROOM_OFF)
				{
					DIO_WritePin(TV_PIN,ROOM_STATUS_OFF);
					InitValues();
					data[2]=DEFAULT;
				}
			}
			else if(data[1]==AIR_COND)
			{
				if(data[2]==SET_TEMP)
				{
					if( AirFlag==1 && data[3]!=0)
					{
						InitValues();
						data[2]=DEFAULT;
						data[3]=DEFAULT;
					}
				}
				else if(data[2]==CONTROL)
				{
					if(data[3]==1)
					{
						AirFlag=1;
						InitValues();
						data[2]=DEFAULT;
						data[3]=DEFAULT;
					}
					else if(data[3]==2)
					{
						AirFlag=0;
						InitValues();
						data[2]=DEFAULT;
						data[3]=DEFAULT;
					}
				}
			}
			else if(data[1]==RGB)
			{
				if(data[2]!=DEFAULT)
				{
					RGB_SetColour(data[2]);
					cli();
					LCD_SetCurser(1,0);
					LCD_WriteNumber(data[2]);
					sei();
					InitValues();
					data[2]=DEFAULT;
				}
			}
			
		}
		
	}
}
static void LedStartValue(void)
{
	DIO_WritePin(ROOM1_PIN,ROOM_STATUS_OFF);
	DIO_WritePin(ROOM2_PIN,ROOM_STATUS_OFF);
	DIO_WritePin(ROOM3_PIN,ROOM_STATUS_OFF);
	DIO_WritePin(ROOM4_PIN,ROOM_STATUS_OFF);
}
static void InitValues(void)
{
	data[0]=DEFAULT;
	data[1]=DEFAULT;
}
static void SPI_ReceiveInterrupt(void)
{
	static u8 i=0,flag=0;
	data[i]=SPI_ReceiveNoBlock();
	SPI_SendNoBlock(data[1]);
	i++;
	if(data[0]==4 && flag==0 && data[1]!=0)
	{
		data[1]+=4;
		flag=1;
	}
	if(i==2 && data[0]!=4)
	{
		i=0;
	}
	if(i==3 && data[0]==4 && (data[1]==5 || data[1]==6 || data[1]==8))
	{
		i=0;
		flag=0;
	}
	if(i==4 && data[0]==4 && data[1]==7 && data[2]==1)
	{
		i=0;
		flag=0;
		Temp=data[3];
	}
	if(i==4 && data[0]==4 && data[1]==7 && data[2]==2 )
	{
		i=0;
		flag=0;
	}
}