#ifndef __LEDS_N_H
#define __LEDS_N_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"                          // CMSIS RTOS header file

/* Exported types ------------------------------------------------------------*/
extern osThreadId_t tid_leds;                        // thread id
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define LD1_ON  0x01U
#define LD1_OFF 0x02U
#define LD2_ON  0x04U
#define LD2_OFF 0x08U
#define LD3_ON  0x10U
#define LD3_OFF 0x20U
/* Exported functions ------------------------------------------------------- */
extern int Init_leds_N (void);
  /* Exported thread functions,  
  Example: extern void app_main (void *arg); */

#endif /* __LEDS_N_H */
