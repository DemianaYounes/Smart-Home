

#ifndef SMARTHOMEMASTER_PRIVATE_H_
#define SMARTHOMEMASTER_PRIVATE_H_

static Password_Status_t SMART_HOMEPasswordCheck(u8 password,u8 PassCheck);
static void SMART_HOMESetPassword(u8 key,u8 mode);
static void Master_TimeOut(void);
static void PasswordTries(u8 TryNumbers);
static void EnterPasswordMode(u8 key,u8 PassNum);
static void FirstTimeSettingDisplayAdmin(void);
static void FirstTimeSettingDisplayGuest(void);
static void SelectModeDisplay(void);
static void SmartHomeStartDisplay(void);
static void EEPROM_StorePassword(void);
static void SelectAdminModeDisplay(void);
static void SavePasswordDisplay(void);
static u8 EnterPasswordForAdmin(u8 key,u8 Mode);
static void SelectGuestModeDisplay(void);
static void SelectGuestModeDisplay(void);
static void PasswordSetting(u8 Mode);

static void AdminOptions(void);
static void AdminModeDispalyPart1(void);
static void AdminModeDispalyPart2(void);
static void ROOMStatus(u8 RoomNum,u8 ROOMStatus);
static void TVStatus(u8 TVStatus);
static void AirConditioningDisplay(void);
static void Control_AirConditioningDisplay(void);
static u8 InputKey(void);
static u8 SetTemperture(u8 TempValue);
static u8 InputTemp(void);

static void GuestOptions(void);
static void GuestOptionsDisplay(void);
static void RGBColors(void);


#endif /* SMARTHOMEMASTER_PRIVATE_H_ */