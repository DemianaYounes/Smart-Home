#include "StdTypes.h"

#include "DIO_Interface.h"

#include "LCD_Interface.h"
#include "Ultrasonic_Interface.h"
#include "_4_ULTRASONIC_Interface.h"
#include "_4_ULTRASONIC_Private.h"

volatile DIO_Pin_type TRIG_PIN=PINC0;
static volatile u8 pdistace[4];

void ULTRASONIC_Runnable(void)
{
	_4_ULTRASONIC_Distance();
	LCD_SetCurser(0,0);
	LCD_WriteNumber(pdistace[0]);
	LCD_SetCurser(0,5);
	LCD_WriteNumber(pdistace[1]);
	LCD_SetCurser(1,0);
	LCD_WriteNumber(pdistace[2]);
	LCD_SetCurser(1,5);
	LCD_WriteNumber(pdistace[3]);
}

static void _4_ULTRASONIC_Distance()
{
	u8 i=0;
	ultrasonic_type UltrasonicCheck=ULTRASONIC_NOK;
	TRIG_PIN=PINC0;
	while(i<4)
	{
		UltrasonicCheck=ULTRASONIC_Distance_PeriodicCheck(&pdistace[i]);
		if(UltrasonicCheck==ULTRASONIC_OK)
		{
			i++;
			TRIG_PIN=PINC0+i;
		}
	}
}