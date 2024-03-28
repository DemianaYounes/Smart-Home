#include "StdTypes.h"

#include "EEPROM_Interface.h"
#include "EEPROM_Services.h"

void EEPROM_WriteArray(u16 *Copy_u16Address, u8 *Copy_u8Data,u8 Copy_u8Size)
{
	static volatile u8 i=0;
	u8 x;
	if(i<Copy_u8Size)
	{
// 		LCD_SetCurser(1,0);
// 		LCD_WriteNumber(i);
		x=EEPROM_WriteDataInterrupt(Copy_u16Address[i],Copy_u8Data[i]);
		if(x==1)
		{
			i++; 
		}
	}
	else
	{
		EEPROM_DisableInterrupt();
	}
}