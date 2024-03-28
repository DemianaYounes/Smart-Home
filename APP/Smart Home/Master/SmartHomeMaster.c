#include "StdTypes.h"
#include "MemMap.h"
/***MCAL***/
#include "DIO_Interface.h"
#include "Timer_Interface.h"
#include "EEPROM_Interface.h"
#include "SPI_Interface.h"
#include "Ex_Interrupt_Interface.h"
#include "UART_Interface.h"
/***HAL***/
#include "LCD_Interface.h"
#include "Keypad_Interface.h"
#include "Servo_Interface.h"
/***APP***/
#include "SmartHomeMaster_Interface.h"
#include "SmartHomeMaster_Private.h"
#include "SmartHomeMaster_Cfg.h"

#define F_CPU  8000000
#include <util/delay.h>
/************************************Smart Home Variables **********************************************/

static volatile u8 AdminArr[4];
static volatile u8 GuestArr[4];
static volatile u8 BlockFlag=0,AdminBlock=0;
static volatile u8 Colors[16][10]={"0-Black","1-White","2-Red","3-Lime","4-Blue","5-Yellow","6-Cyan","7-Magenta",
                                  "8-Silver","9-Gray","10-Maroon","11-Olive","12-Green","13-Purple","14-Teal","15-Navy"};

/************************************Smart Home **********************************************/
void SMART_HOMEMasterInit(void)
{
	DIO_Init();
	EEPROM_Init();
	SPI_Init();
	UART_Init(9600);
	TIMER0_Init(TIMER0_NORMAL_MODE,TIMER0_SCALER_8);
	Timer1_Init(TIMER1_FASTPWM_OCRA_TOP_MODE,TIMER1_SCALER_8);
	Timer1_OCRB1Mode(OCRB_NON_INVERTING);
	OCR1A=19999;
	LCD_Init();
	Keypad_Init();
	TIMER0_OV_SetCallBack(Master_TimeOut);
	GLOBAL_ENABLE();
	TIMER0_OV_InterruptEnable();
}
void SMART_HOMERunnable(void)
{
	u8 key=NO_KEY,i=0,AdminFlag,GuestFlag,c=0,option=1,angle,sum=0;
	EEPROM_StorePassword();
	SmartHomeStartDisplay();
	
	while(1)
	{
		if(SMART_HOMEPasswordCheck(DEFAULT,ADMIN_CHECK)==Password_NotSet)
		{
			FirstTimeSettingDisplayAdmin();
			PasswordSetting(ADMIN_MODE);
			FirstTimeSettingDisplayGuest();
			PasswordSetting(GUEST_MODE);
			SavePasswordDisplay();
		}
		else
		{
			BlockFlag=0;
			AdminBlock=0;
			DIO_WritePin(BLOCK_LED_PIN,LED_Status_OFF);
			SelectModeDisplay();
			key=KEYPAD_GetKey();
			if(key!=NO_KEY)
			{
				if(key>='0' && key<='9')
				{
					/*Admin Mode*/
					if(key=='0')
					{
						AdminFlag=EnterPasswordForAdmin(key,ADMIN_MODE);
						LCD_Clear();
						while(AdminFlag==0 && AdminBlock==0)
						{
							option=1;
							angle=0;
							sum=0;
							DIO_WritePin(ADMIN_MODE_LED,LED_Status_ON);
							while(option==1)
							{
								UART_SendString("Enter 1 for the servo motor and 2 for the other options");
								UART_SendNumber(0x0d);
								UART_SendString("the other options");
								UART_SendNumber(0x0d);
								UART_SendString("--> ");
								option=UART_ReceiveBusyWait()-'0';
								UART_SendNumber(0x0d);
								if(option==1)
								{
									UART_SendString("Set angle: ");
									while(sum!=0x0d)
									{
										sum=UART_ReceiveBusyWait();
										if(sum!=0x0d)
										{
											angle=angle*10+(sum-'0');
										}
									}
									SERVO_SetAngleInProteus(angle);
									option=2;
								}
							}
							AdminOptions();
							DIO_WritePin(ADMIN_MODE_LED,LED_Status_OFF);
						}
						while(BlockFlag==0 && AdminFlag==1)
						{
							LCD_SetCurser(0,0);
							LCD_WriteString("Login Blocked");
							LCD_SetCurser(1,0);
							LCD_WriteString("wait 20 seconds");
							DIO_WritePin(BLOCK_LED_PIN,LED_Status_ON);
						}
					}
					else if(key=='1')
					{
						GuestFlag=EnterPasswordForAdmin(key,GUEST_MODE);
						LCD_Clear();
						while(GuestFlag==0 && BlockFlag==0)
						{
							DIO_WritePin(GUEST_MODE_LED,LED_Status_ON);
							GuestOptions();
							DIO_WritePin(GUEST_MODE_LED,LED_Status_OFF);
						}
						while(BlockFlag==0 && GuestFlag==1)
						{
							LCD_SetCurser(0,0);
							LCD_WriteString("Login Blocked");
							LCD_SetCurser(1,0);
							LCD_WriteString("wait 20 seconds");
							DIO_WritePin(BLOCK_LED_PIN,LED_Status_ON);
						}
					}
					
				}
			}
			
		}
	}
}


static void GuestOptions(void)
{
	u8 key=NO_KEY,flag=0,data;
	LCD_Clear();
	while(BlockFlag==0)
	{
		flag=0;
		LCD_SetCurser(0,4);
		LCD_WriteString("                  ");
		LCD_SetCurser(1,4);
		LCD_WriteString("                  ");
		GuestOptionsDisplay();
		while(flag<2 && BlockFlag==0)
		{
			key=KEYPAD_GetKey();
			if(key!=NO_KEY)
			{
				if(key>='1' && key<='4')
				{
					data=SPI_sendReceiveByte(key-'0');
					ROOMStatus(key-'0',data);
					flag++;
				}
			}
		}
	}
}
static void GuestOptionsDisplay(void)
{
	LCD_SetCurser(0,0);
	LCD_WriteString("1:Room1");
	LCD_SetCurser(0,8);
	LCD_WriteString("2:Room2");
	LCD_SetCurser(1,0);
	LCD_WriteString("3:Room3");
	LCD_SetCurser(1,8);
	LCD_WriteString("4:ROOM4");
}
static void AdminOptions(void)
{
	u8 key=NO_KEY,data,flag=0,OptionFlag=0,Temp[2],Color,sum=0;
	LCD_Clear();
	while(AdminBlock==0)
	{
		flag=0;
		LCD_SetCurser(0,4);
		LCD_WriteString("                  ");
		LCD_SetCurser(1,4);
		LCD_WriteString("                  ");
		AdminModeDispalyPart1();
		OptionFlag=0;
		while(flag<2 && AdminBlock==0)
		{
			key=KEYPAD_GetKey();
			if(key!=NO_KEY)
			{
				if(key>='1' && key<='3' && OptionFlag==0)
				{
					data=SPI_sendReceiveByte(key-'0');
					ROOMStatus(key-'0',data);
					flag++;
				}
				if(key=='4')
				{
					OptionFlag=1;
					SPI_sendReceiveByte(key-'0');
					AdminModeDispalyPart2();
					while(1)
					{
						flag=2;
						key=KEYPAD_GetKey();
						if(key!=NO_KEY)
						{
							if(key=='1')
							{
								data=SPI_sendReceiveByte(key-'0');
								ROOMStatus(4,data);
								InputKey();
								key=NO_KEY;
								break;
							}
							if(key=='2')
							{
								data=SPI_sendReceiveByte(key-'0');
								TVStatus(data);
								InputKey();
								key=NO_KEY;
								break;
							}
							if(key=='3')
							{
								data=SPI_sendReceiveByte(key-'0');
								AirConditioningDisplay();
								data=InputKey();
								if(data=='2')
								{
									Control_AirConditioningDisplay();
									InputKey();
								}
								else if(data='1')
								{
									SetTemperture(0);
									Temp[0]=InputTemp();
									SetTemperture(Temp[0]);
									Temp[1]=InputTemp();
									SetTemperture(Temp[0]*10+Temp[1]);
									SPI_sendReceiveByte(Temp[0]*10+Temp[1]);
									_delay_ms(300);
								}
								key=NO_KEY;
								break;
							}
							if(key=='4')
							{
								sum=0;
								Color=0;
								data=SPI_sendReceiveByte(key-'0');
								LCD_SetCurser(0,0);
								LCD_WriteString("Choose a color");
								LCD_SetCurser(1,0);
								LCD_WriteString("from the terminal");
								RGBColors();
								while(sum!=0x0d)
								{
									sum=UART_ReceiveBusyWait();
									if(sum!=0x0d)
									{
										Color=Color*10+(sum-'0');
									}
								}
								//Color=UART_ReceiveNumber_u16();
								data=SPI_sendReceiveByte(Color);
								break;
							}
						}
					}
					
					
				}
			}
		}
	}
}
static void RGBColors(void)
{
	u8 i;
	UART_SendNumber(0x0d);
	UART_SendString("<--Select the Colour-->");
	UART_SendNumber(0x0d);
	for(i=0;i<16;i++)
	{
		UART_SendString(Colors[i]);
		UART_SendNumber(0x0d);
	}
	UART_SendString("-->");
}
static u8 SetTemperture(u8 TempValue)
{
	LCD_Clear();
	LCD_SetCurser(0,0);
	LCD_WriteString("Set Temp:");
	LCD_SetCurser(0,9);
	LCD_WriteNumber(TempValue);
}
static void AirConditioningDisplay(void)
{
	LCD_Clear();
	LCD_SetCurser(0,0);
	LCD_WriteString("1:Set Temperature");
	LCD_SetCurser(1,0);
	LCD_WriteString("2:Control");
	LCD_SetCurser(1,10);
	LCD_WriteString("3:RET");
}


static void Control_AirConditioningDisplay(void)
{
	LCD_Clear();
	LCD_SetCurser(0,0);
	LCD_WriteString("Air Cond:");
	LCD_SetCurser(0,10);
	if(TVStatus==1)
	{
		LCD_WriteString("S:ON");
	}
	else
	{
		LCD_WriteString("S:OFF");
	}
	LCD_SetCurser(1,0);
	LCD_WriteString("1-ON 2-OFF 3-RET");
}

static u8 InputTemp(void)
{
	u8 key=NO_KEY;
	while(1)
	{
		key=KEYPAD_GetKey();
		if(key!=NO_KEY)
		{
			if(key>='0' && key<='9')
			{
				//SPI_sendReceiveByte(key-'0');
				break;
			}
		}
	}
	return key-'0';
}

static u8 InputKey(void)
{
	u8 key=NO_KEY;
	while(1)
	{
		key=KEYPAD_GetKey();
		if(key!=NO_KEY)
		{
			if(key>='1' && key<='9')
			{
				SPI_sendReceiveByte(key-'0');
				break;
			}
		}
	}
	return key;
}
static void TVStatus(u8 TVStatus)
{
	LCD_Clear();
	LCD_SetCurser(0,0);
	LCD_WriteString("TV:");
	LCD_SetCurser(0,6);
	if(TVStatus==1)
	{
		LCD_WriteString("S:ON");
	}
	else
	{
		LCD_WriteString("S:OFF");
	}
	LCD_SetCurser(1,0);
	LCD_WriteString("1-ON 2-OFF 3-RET");
}
static void ROOMStatus(u8 RoomNum,u8 ROOMStatus)
{
	LCD_Clear();
	LCD_SetCurser(0,0);
	LCD_WriteString("ROOM");
	LCD_SetCurser(0,4);
	LCD_WriteNumber(RoomNum);
	LCD_SetCurser(0,6);
	if(ROOMStatus==1)
	{
		LCD_WriteString("S:ON");
	}
	else
	{
		LCD_WriteString("S:OFF");
	}
	LCD_SetCurser(1,0);
	LCD_WriteString("1-ON 2-OFF 3-RET");
	
}
static void AdminModeDispalyPart2(void)
{
	LCD_SetCurser(0,0);
	LCD_WriteString("1:Room4");
	LCD_SetCurser(0,8);
	LCD_WriteString("2:TV   ");
	LCD_SetCurser(1,0);
	LCD_WriteString("3:Air Cond ");
	LCD_SetCurser(1,11);
	LCD_WriteString("4:RGB");
}
static void AdminModeDispalyPart1(void)
{
	LCD_SetCurser(0,0);
	LCD_WriteString("1:Room1");
	LCD_SetCurser(0,8);
	LCD_WriteString("2:Room2");
	LCD_SetCurser(1,0);
	LCD_WriteString("3:Room3");
	LCD_SetCurser(1,8);
	LCD_WriteString("4:More");
}
static void PasswordSetting(u8 Mode)
{
	u8 i=0,key=NO_KEY;
	while(i<4)
	{
		key=KEYPAD_GetKey();
		if(key!=NO_KEY)
		{
			if(key>='0' && key<='9')
			{
				EnterPasswordMode(key,i);
				SMART_HOMESetPassword(key,Mode);
				i++;
			}
		}
	}
}
static u8 EnterPasswordForAdmin(u8 key,u8 Mode)
{
	u8 i,c,flag=0,ModeKey;
	LCD_Clear();
	c=0;
	ModeKey=key;
	while(c<3)
	{
		if(ModeKey=='0')
		{
			SelectAdminModeDisplay();
		}
		else if(ModeKey=='1')
		{
			SelectGuestModeDisplay();
		}
		i=0;
		flag=0;
		while(i<4)
		{
			key=KEYPAD_GetKey();
			if(key>='0' && key<='9')
			{
				EnterPasswordMode(key,i);
				if(SMART_HOMEPasswordCheck(key-'0',Mode)!=Password_True)
				{
					flag=1;
				}
				i++;
			}
		}
		LCD_Clear();
		if(flag==0)
		{
			LCD_SetCurser(0,0);
			LCD_WriteString("True Password");
			_delay_ms(300);
			break;
		}
		else
		{
			c++;
			PasswordTries(c);
		}
	}
	return flag;
}
static void SavePasswordDisplay(void)
{
	LCD_Clear();
	LCD_SetCurser(0,0);
	LCD_WriteString("Password Saved");
	_delay_ms(500);
	LCD_Clear();
}
static void SelectAdminModeDisplay(void)
{
	LCD_SetCurser(0,0);
	LCD_WriteString("Admin Mode");
	LCD_SetCurser(1,0);
	LCD_WriteString("Enter Pass:");
}
static void SelectGuestModeDisplay(void)
{
	LCD_SetCurser(0,0);
	LCD_WriteString("Guest Mode");
	LCD_SetCurser(1,0);
	LCD_WriteString("Enter Pass:");
}
static void EEPROM_StorePassword(void)
{
	u8 i;
	for(i=0;i<4;i++)
	{
		AdminArr[i]=EEPROM_read(ADMIN_ADDRESS+i);
	}
	for(i=0;i<4;i++)
	{
		GuestArr[i]=EEPROM_read(GUEST_ADDRESS+i);
	}
}
static void SmartHomeStartDisplay(void)
{
	u8 HomeArr[8]={ 0x04,0x0E,0x0E,0x1F,0x1F,0x1F,0x1F,0x00};
	LCD_SetCurser(0,0);
	LCD_WriteString("Welcome to Smart");
	LCD_SetCurser(1,0);
	LCD_WriteString("Home System");
	LCD_CustomChar(1,HomeArr);
	LCD_SetCurser(1,12);
	LCD_WriteChar(1);
	_delay_ms(600);
	LCD_Clear();
}
static void SelectModeDisplay(void)
{
	LCD_SetCurser(0,0);
	LCD_WriteString("Select Mode:      ");
	LCD_SetCurser(1,0);
	LCD_WriteString("0:Admin 1:Guest   ");
}
static void FirstTimeSettingDisplayAdmin(void)
{
	LCD_SetCurser(0,0);
	LCD_WriteString("Login For The ");
	LCD_SetCurser(1,0);
	LCD_WriteString("First Time");
	_delay_ms(600);
	LCD_Clear();
	LCD_SetCurser(0,0);
	LCD_WriteString("Set Admin Pass");
	LCD_SetCurser(1,0);
	LCD_WriteString("Admin Pass:");
}
static void FirstTimeSettingDisplayGuest(void)
{
	LCD_Clear();
	LCD_SetCurser(0,0);
	LCD_WriteString("Password Saved");
	_delay_ms(500);
	LCD_Clear();
	LCD_SetCurser(0,0);
	LCD_WriteString("Set Guest Pass");
	LCD_SetCurser(1,0);
	LCD_WriteString("Guest Pass:");
}
static void EnterPasswordMode(u8 key,u8 PassNum)
{
	LCD_SetCurser(1,PassNum+11);
	LCD_WriteNumber(key-'0');
	_delay_ms(50);
	LCD_SetCurser(1,PassNum+11);
	LCD_WriteChar('*');
}
static void PasswordTries(u8 TryNumbers)
{
	LCD_WriteString("Wrong Password");
	LCD_SetCurser(1,0);
	LCD_WriteString("Tries Left:");
	LCD_SetCurser(1,11);
	LCD_WriteNumber(3-TryNumbers);
	_delay_ms(1000);
	LCD_Clear();
}
static void  SMART_HOMESetPassword(u8 key,u8 mode)
{
	static u8 i=0;
	if(mode==ADMIN_MODE)
	{
		if(key>='0' && key<='9')
		{
			AdminArr[i]=key-'0';
			EEPROM_write(ADMIN_ADDRESS+i,AdminArr[i]);
			i++;
			if(i==4)
			{
				i=0;
			}
		}
	}
	else if(mode==GUEST_MODE)
	{
		if(key>='0' && key<='9')
		{
			GuestArr[i]=key-'0';
			EEPROM_write(GUEST_ADDRESS+i,GuestArr[i]);
			i++;
			if(i==4)
			{
				i=0;
			}
		}
	}
}

static Password_Status_t SMART_HOMEPasswordCheck(u8 password,u8 PassCheck)
{
	Password_Status_t PassStatus=Password_NotSet;
	u8 j,flag;
	static u8 i;
	flag=EEPROM_read(password);
	if(flag==0xff)
	{
		for(j=0;j<4;j++)
		{
			flag=EEPROM_read(ADMIN_ADDRESS+j);
			if(flag!=0xff)
			{
				PassStatus=Password_False;
				break;
			}
		}
		if(PassStatus==Password_True)
		{
			EEPROM_write(password,1);
		}
	}
	else
	{
		PassStatus=Password_True;
		if(PassCheck==ADMIN_CHECK)
		{
			if(password!=AdminArr[i])
			{
				PassStatus=Password_False;
			}
			else if(password==AdminArr[i])
			{
				i++;
			}
			if(i==4)
			{
				i=0;
			}
		}
		else if(PassCheck==GUEST_CHECK)
		{
			if(password!=GuestArr[i])
			{
				PassStatus=Password_False;
			}
			else if(password==GuestArr[i])
			{
				i++;
			}
			if(i==4)
			{
				i=0;
			}
		}
	}
	
	return PassStatus;
}
static void Master_TimeOut(void)
{
	static u32 c=0;
	c++;
	if(c==78125)  //20sec
	{
		BlockFlag=1;
		DIO_WritePin(BLOCK_LED_PIN,LED_Status_OFF);
	}
	if(c==156250)   //40sec
	{
		c=0;
		BlockFlag=1;
		DIO_WritePin(BLOCK_LED_PIN,LED_Status_OFF);
		AdminBlock=1;
	}
}