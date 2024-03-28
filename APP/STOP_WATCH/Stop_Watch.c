
#include "StdTypes.h"
/*Include From HAL*/
#include "_7_Segment_Interface.h"
#include "Button_Interface.h"
/*Include From stop watch*/
#include "Stop_Watch_Interface.h"
#include "Stop_Watch_Cfg.h"

#define F_CPU  8000000
#include <util/delay.h>
/******************Definitions Varibles***********************************/
static u8 num=COUNT_NUM;
static u8 Copu_u8FlagReset=0 ,Copy_u8flagPause=0,Copy_u8FlagMode=0,Copy_u8FlagStart=0,i;

/**
 *This stop watch has two modes
 *the first mode(1 pressesd)-->there are three buttons
 *1-for pause
 *2-for start
 *3-for reset
 *the second mode(2 pressesd)-->this option to configure setting of the watch by
 *1-button for increment
 *2-button for decrement
 **/

void Stop_WatchInit(void)
{
	
	Seven_SegmentDisplay_BCD(num);
}
void Stop_WatchRunning(void)
{
	if(Button_Mode()==PRESSED)
	{
		Copy_u8FlagMode++;
		while(Button_Mode()==PRESSED);
	}
	if(Copy_u8FlagMode%2==1)
	{
		if(ButtonStart_ButtonDecrement()==PRESSED && ButtonPause_ButtonIncrement()==NOT_PRESSED && Button_Reset()==NOT_PRESSED)
		{
			Seven_SegmentDisplay_BCD(num);
			num--;
			if(num==255)
			{
				num=COUNT_NUM;
			}
			/*Delay for debuncing*/
			_delay_ms(300);
			Copy_u8FlagStart=1;
		}
		else
		{
			Copy_u8FlagStart=0;
		}
		if(ButtonPause_ButtonIncrement()==PRESSED && Copy_u8flagPause==0)
		{
			/*Delay for debuncing*/
			_delay_ms(300);
			Copy_u8flagPause=1;
		}
		else
		{
			Copy_u8flagPause=0;
		}
		if(Button_Reset()==PRESSED && Copu_u8FlagReset==0)
		{
			num=COUNT_NUM;
			Seven_SegmentDisplay_BCD(num);
			/*Delay for debuncing*/
			_delay_ms(300);
			Copu_u8FlagReset=1;
		}
		else
		{
			Copu_u8FlagReset=0;
		}
	}
	else if(Copy_u8FlagMode%2==0)
	{
		if(ButtonStart_ButtonDecrement()==PRESSED && Copy_u8FlagStart==0)
		{
			num--;
			if(num==255)
			{
				num=99;
			}
			Seven_SegmentDisplay_BCD(num);
			Copy_u8FlagStart=1;
			/*Delay for debuncing*/
			_delay_ms(300);
		}
		else
		{
			Copy_u8FlagStart=0;
		}
		if(ButtonPause_ButtonIncrement()==PRESSED && Copy_u8flagPause==0)
		{
			
			num++;
			if(num==100)
			{
				num=0;
			}
			Seven_SegmentDisplay_BCD(num);
			/*Delay for debuncing*/
			_delay_ms(300);
			Copy_u8flagPause=1;
		}
		else
		{
			Copy_u8flagPause=0;
		}
	}
}
Button_value ButtonPause_ButtonIncrement(void)
{
	return Button_Pins(BUTTON_1);
}
Button_value ButtonStart_ButtonDecrement(void)
{
	return Button_Pins(BUTTON_2);
}
Button_value Button_Reset(void)
{
	return Button_Pins(BUTTON_3);
}
Button_value Button_Mode(void)
{
	return Button_Pins(BUTTON_4);
}


void Stop_WatchRunning_MUX(void)
{
	Seven_SegmentDisplay(num);
	if(Button_Mode()==PRESSED)
	{
		Copy_u8FlagMode++;
		while(Button_Mode()==PRESSED);
	}
	if(Copy_u8FlagMode%2==1)
	{
		if(ButtonStart_ButtonDecrement()==PRESSED && ButtonPause_ButtonIncrement()==NOT_PRESSED && Button_Reset()==NOT_PRESSED)
		{
			for(i=0;i<30;i++)
			{
				Seven_SegmentDisplay(num);
				_delay_ms(10);
			}
			num--;
			if(num==255)
			{
				num=COUNT_NUM;
			}
			/*Delay for debuncing*/
			Copy_u8FlagStart=1;
		}
		else
		{
			Copy_u8FlagStart=0;
		}
		if(ButtonPause_ButtonIncrement()==PRESSED && Copy_u8flagPause==0)
		{
			/*Delay for debuncing*/
			for(i=0;i<30;i++)
			{
				Seven_SegmentDisplay(num);
				_delay_ms(10);
			}
			Copy_u8flagPause=1;
		}
		else
		{
			Copy_u8flagPause=0;
		}
		if(Button_Reset()==PRESSED && Copu_u8FlagReset==0)
		{
			num=COUNT_NUM;
			for(i=0;i<30;i++)
			{
				Seven_SegmentDisplay(num);
				_delay_ms(10);
			}
			/*Delay for debuncing*/
			Copu_u8FlagReset=1;
		}
		else
		{
			Copu_u8FlagReset=0;
		}
	}
	else if(Copy_u8FlagMode%2==0)
	{
		if(ButtonStart_ButtonDecrement()==PRESSED && Copy_u8FlagStart==0)
		{
			num--;
			if(num==255)
			{
				num=99;
			}
			for(i=0;i<30;i++)
			{
				Seven_SegmentDisplay(num);
				_delay_ms(10);
			}
			Copy_u8FlagStart=1;
			/*Delay for debuncing*/
		}
		else
		{
			Copy_u8FlagStart=0;
		}
		if(ButtonPause_ButtonIncrement()==PRESSED && Copy_u8flagPause==0)
		{
			
			num++;
			if(num==100)
			{
				num=0;
			}
			for(i=0;i<30;i++)
			{
				Seven_SegmentDisplay(num);
				_delay_ms(10);
			}
			/*Delay for debuncing*/
			Copy_u8flagPause=1;
		}
		else
		{
			Copy_u8flagPause=0;
		}
	}
}