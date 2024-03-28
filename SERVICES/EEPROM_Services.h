

#ifndef EEPROM_SERVICES_H_
#define EEPROM_SERVICES_H_

void EEPROM_WriteArray_BusyWait(u16*Copy_u16PAddress,u8*Copy_u8PData);
void EEPROM_WriteArray(u16 *Copy_u16Address, u8 *Copy_u8Data,u8 Copy_u8Size);


#endif /* EEPROM_SERVICES_H_ */