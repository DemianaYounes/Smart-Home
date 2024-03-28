#include "StdTypes.h"
#include "Utils.h"

#include "UART_Interface.h"
#include "DIO_Interface.h"
#include "UART_Services.h"

#define F_CPU 8000000
#include <util/delay.h>

void UART_SendString(u8*str)
{
	u8 i;
	for(i=0;str[i];i++)
	{
		UART_SendBusyWait(str[i]);
	}
	//UART_SendBusyWait(str[i]);
}

void UART_ReceiveString(u8*str)
{
	u8 i=0;
	str[i]=UART_ReceiveBusyWait();
	while(str[i]!=0x0d)
	{
		i++;
		str[i]=UART_ReceiveBusyWait();
	}
	str[i]=0;
}

void UART_SendNumber(u32 num)
{
	UART_SendBusyWait((u8)num);
	UART_SendBusyWait((u8)(num>>8));
	UART_SendBusyWait((u8)(num>>16));
	UART_SendBusyWait((u8)(num>>24));
}
void UART_SendNumber2(u32 num)
{
	u8*p=(u8*)&num;
	UART_SendBusyWait(p[0]);
	UART_SendBusyWait(p[1]);
	UART_SendBusyWait(p[2]);
	UART_SendBusyWait(p[3]);
}

u32 UART_ReceiveNumber(void)
{
	u32 num;
	u8 b0=UART_ReceiveBusyWait();
	u8 b1=UART_ReceiveBusyWait();
	u8 b2=UART_ReceiveBusyWait();
	u8 b3=UART_ReceiveBusyWait();
	num=(u32)b0|((u32)b1<<8)|((u32)b2<<16)|((u32)b3<<24);
	return num;
}
u16 UART_ReceiveNumber_u16(void)
{
	u32 num;
	u8 b0=UART_ReceiveBusyWait()-'0';
	u8 b1=UART_ReceiveBusyWait()-'0';
	num=b0*10+b1;
	return num;
}
u32 UART_ReceiveNumber2(void)
{
	u32 num;
	u8*p=(u8*)&num;
	p[0]=UART_ReceiveBusyWait();
	p[1]=UART_ReceiveBusyWait();
	p[2]=UART_ReceiveBusyWait();
	p[3]=UART_ReceiveBusyWait();
	return num;
}
u32 UART_ReceiveNumber3(void)
{
	u8 arr[4];
	arr[0]=UART_ReceiveBusyWait();
	arr[1]=UART_ReceiveBusyWait();
	arr[2]=UART_ReceiveBusyWait();
	arr[3]=UART_ReceiveBusyWait();
	return  (((u32)arr[0])|(((u32)arr[1])<<8)|(((u32)arr[2])<<16)|(((u32)arr[3])<<24));
}

Endian_t Endian_Check(void)
{
	Endian_t EndianCheck=LITTLE_ENDIAN;
	u32 num=1;
	u8*p=(u8*)&num;
	if(*p==1)
	{
		EndianCheck=LITTLE_ENDIAN;
	}
	else
	{
		EndianCheck=BIG_ENDIAN;
	}
	return EndianCheck;
}

u32 Endian_Convert2_u32(u32 num)
{
	u32 NumConvert;
	u8*p=(u8*)&num;
	NumConvert=((u32)p[3])|((u32)p[2]<<8)|((u32)p[1]<<16)|((u32)p[0]<<24);
	return NumConvert;
}
u32 endian_convert_u32(u32 num)
{
	//(num>>24)|(num<<24)|(((num<<16)<<24)<<16)|((num<<8)&0x00ff0000);
	return ((num>>24)|(num<<24)|((num>>8)&0x0000ff00)|((num<<8)&0x00ff0000));
}

u32 endian_convert_u16(u16 num)
{
	return (num>>8)|(num<<8);
}

void UART_SendStringCheckSum(u8*str)
{
	/*CRC check
	 *1-send length
	 *2-send byte by byte (send string)
	 *3-send sum of char
	 */
	u8 i,l;
	u16 sum=0;
	for(l=0;str[l];l++)
	{
		sum+=str[l];
	}
	UART_SendBusyWait(l);
	for(i=0;str[i];i++)
	{
		UART_SendBusyWait(str[i]);
	}
	UART_SendBusyWait((u8)sum);
	UART_SendBusyWait((u8)(sum>>8));
}
u8 UART_RecieveStringCheckSum(u8*str)
{
	u8 l,i,b0,b1;
	u16 sum_rec=0,sum_calc=0;
	l=UART_ReceiveBusyWait();
	for(i=0;i<l;i++)
	{
		str[i]=UART_ReceiveBusyWait();
		sum_calc+=str[i];
	}
	b0=UART_ReceiveBusyWait();
	b1=UART_ReceiveBusyWait();
	sum_rec=(b0|((u16)b1>>8));
	if(sum_rec==sum_calc)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/**********************************************Asynchronous Function**********************************************/
//static volatile u8 str_send[QUEUE_SIZE][20]=NULLPTR;
static volatile s32 front = -1, rear = -1,flag;
static volatile u8 QueueStr[QUEUE_SIZE][20];

static void UART_SendCB(void)
{
	static u8 i=1;
	
	if(QueueStr[front][i])
	{
		UART_SendNoBlock(QueueStr[front][i]);
		i++;
	}
	else if(front<=rear)
	{
		front++;
		i=1;
		UART_SendBusyWait(QueueStr[front][0]);
	}
}

void UART_SendString_Asynch(void)
{
	/*Do the job in the background */
	UART_SendBusyWait(QueueStr[front][0]);
	UART_TXInterruptEnable();
	UART_TXCallBack(UART_SendCB);
	
}
// Check if the queue is full
int isFull() {
	if ((front == rear + 1) || (front == 0 && rear == QUEUE_SIZE - 1)) return 1;
	return 0;
}

// Check if the queue is empty
int isEmpty() {
	if (front == -1) return 1;
	return 0;
}
QUEUE_t enQueueString(char*str)
{
	int i;
	if (isFull())
	{
		return QUEUE_Full;
	}
	else
	{
		if(front==-1)
		{
			front=0;
		}
		rear =(rear+1)%QUEUE_SIZE;
		for(i=0;str[i];i++)
		{
			QueueStr[rear][i]=str[i];
		}
		QueueStr[rear][i]=0;
		return QUEUE_Done;
	}
}

QUEUE_t deQueueString(char*str)
{
	int i;
	if (isEmpty())
	{
		return QUEUE_Empty;
	}
	else
	{
		for(i=0;QueueStr[front][i];i++)
		{
			str[i]=QueueStr[front][i];
		}
		str[i]=0;
		if(front==rear)
		{
			front=-1;
			rear=-1;
		}
		else
		{
			front=(front+1)%QUEUE_SIZE;
		}
		return QUEUE_Done;
	}
}


/******************************************UART_Frame_By_Software**********************************************/


u16 UART_Frame(u8 data)
{
	u16 DataFrame=0;
	u8 parity,ones=0,Copy_data=data;
	while(Copy_data)
	{
		ones+=Copy_data&1;
		Copy_data>>=1;
	}
	if(PARITY==EVEN)
	{
		if(ones%2==0)
		{
			parity=0;
		}
		else
		{
			parity=1;
		}
	}
	else
	{
		if(ones%2==0)
		{
			parity=1;
		}
		else
		{
			parity=0;
		}
	}
	DataFrame=(DataFrame|((u16)data<<1)|((u16)parity<<9)|((u16)1<<10));
	return 	DataFrame;
}
void UART_SendFrame(u16 Frame)
{
	u8 i;
	for(i=0;i<11;i++)
	{
		DIO_WritePin(PINC0,READ_BIT(Frame,i));
		/*baud rate is 9600 This means the number of bits in one second
		 *i have 8 data , 1 stop ,1 parity ,1 start
		 *total bits=11
		 *time of one bits=one bit/baud rate=1/9600=1000000/9600=104us
		 */
		_delay_us(104);
	}
}