 

#ifndef SMARTHOMEMASTER_CFG_H_
#define SMARTHOMEMASTER_CFG_H_

#define BLOCK_LED_PIN   PINB0
#define GUEST_MODE_LED  PIND7
#define ADMIN_MODE_LED  PINB1
#define LED_Status_ON   HIGH     
#define LED_Status_OFF  LOW    
#define PASSWORD_TIMEOUT  20


#define ADMIN_ADDRESS   0
#define GUEST_ADDRESS   ADMIN_ADDRESS+4

#define ADMIN_CHECK   0
#define GUEST_CHECK   1
#define DEFAULT   0xff

#define ADMIN_MODE   0
#define GUEST_MODE   1


#endif /* SMARTHOMEMASTER_CFG_H_ */