
#ifndef STOP_WATCH_INTERFACE_H_
#define STOP_WATCH_INTERFACE_H_

void Stop_WatchInit(void);
void Stop_WatchRunning(void);
Button_value ButtonPause_ButtonIncrement(void);
Button_value ButtonStart_ButtonDecrement(void);
Button_value Button_Reset(void);
Button_value Button_Mode(void);

void Stop_WatchRunning_MUX(void);

#endif /* STOP_WATCH_INTERFACE_H_ */