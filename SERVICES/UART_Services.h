

#ifndef UART_SERVICES_H_
#define UART_SERVICES_H_


typedef enum{
	BIG_ENDIAN,
	LITTLE_ENDIAN
	}Endian_t;


void UART_SendString(u8*str);
void UART_ReceiveString(u8*str);
void UART_SendNumber(u32 num);
void UART_SendNumber1(u32 num);
u32 UART_ReceiveNumber(void);
u32 UART_ReceiveNumber1(void);
u32 UART_ReceiveNumber2(void);
Endian_t Endian_Check(void);
u32 Endian_Convert2_u32(u32 num);
u32 endian_convert_u16(u16 num);
u32 endian_convert_u32(u32 num);
u32 endian_convert2_u32(u32 num);
void UART_SendStringCheckSum(u8*str);
u8 UART_RecieveStringCheckSum(u8*str);

u16 UART_ReceiveNumber_u16(void);

/**********************************************Asynchronous Function**********************************************/
typedef enum{
	QUEUE_Done,
	QUEUE_Empty,
	QUEUE_Full
}QUEUE_t;
#define QUEUE_SIZE 6
void UART_SendString_Asynch(void);
QUEUE_t enQueueString(char*str);
QUEUE_t deQueueString(char*str);

/**************************For_UART_FrameBySoftware*********************************/
#define EVEN 1
#define ODD  0
#define PARITY   EVEN

u16 UART_Frame(u8 data);
void UART_SendFrame(u16 Frame);


#endif /* UART_SERVICES_H_ */