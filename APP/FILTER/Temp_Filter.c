#include "StdTypes.h"

#include "Sensors_Interface.h"

static u16 TempValueUnFilterd;
static u16 TempValueFilterd;
void Filter_Init(void)
{
	TempValueUnFilterd=TEMP_Read();
	TempValueFilterd=TempValueUnFilterd;
}

/*not more than 258us */
void Filter_Runnable(void)
{
	static u16 FilterArr[10],i=0;
	u8 j;
	u32 sum=0;
	FilterArr[i]=TEMP_Read();
	TempValueFilterd=FilterArr[i];
	i++;
	if(i==10)
	{
		i=0;
	}
	for(j=0;j<10;j++)
	{
		sum+=FilterArr[j];
	}
	TempValueFilterd=sum/10;
}
/*The aim of the function getter is to return a global variable*/
u16 Filter_GetFilterdTemp(void)
{
	return TempValueFilterd;
}

u16 Filter_GetUnFilterdTemp(void)
{
	return TempValueUnFilterd;
}