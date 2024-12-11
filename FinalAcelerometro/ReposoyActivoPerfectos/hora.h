#ifndef __HORA_H
#define __HORA_H

#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"

int Init_Hora (void);

extern uint8_t ss;
extern uint8_t mm;
extern uint8_t hh;

void startClock(void);
void stopClock(void);


#endif /* __HORA_H */
