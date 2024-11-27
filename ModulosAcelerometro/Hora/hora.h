#ifndef __CLOCK_H
#define __CLOCK_H

#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"

int Init_Hora (void);

extern uint8_t segundos;
extern uint8_t minutos;
extern uint8_t horas;

#endif /* __CLOCK_H */
