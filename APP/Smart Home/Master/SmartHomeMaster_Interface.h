


#ifndef SMARTHOMEMASTER_INTERFACE_H_
#define SMARTHOMEMASTER_INTERFACE_H_

typedef enum{
	Password_False,
	Password_True,
	Password_NotSet
	}Password_Status_t;

void SMART_HOMEMasterInit(void);
void SMART_HOMERunnable(void);




#endif /* SMARTHOMEMASTER_INTERFACE_H_ */