#ifndef __LEDS_N_H
#define __LEDS_N_H

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"                          // CMSIS RTOS header file

#define LED1_ON  0X01U
#define LED1_OFF 0X02U
#define LED2_ON  0X04U
#define LED2_OFF 0X08U
#define LED3_ON  0X10U
#define LED3_OFF 0X20U

	extern osThreadId_t tid_ThLeds;                        // thread id
	
	int Init_ThLEDs (void);
	int Init_Thread (void);

#endif /* __LEDS_N_H */