#ifndef __JOYSTICK_H
#define __JOYSTICK_H

#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"

#define J_UP    0x01
#define J_RGHT  0x02
#define J_DOWN  0x03
#define J_LEFT  0x04
#define J_CNT   0x05

extern osThreadId_t tid_joystick;                        // thread id

int Init_Joystick (void);

#endif /* __JOYSTICK_H */
