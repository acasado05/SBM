#ifndef __ALTAVOZ_H
#define __ALTAVOZ_H

/* Liber?as */
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"
#include "stdbool.h"

int Init_Altavoz (void);
extern osMessageQueueId_t idQueueAltavoz (void);

#endif /* __ALTAVOZ_H */