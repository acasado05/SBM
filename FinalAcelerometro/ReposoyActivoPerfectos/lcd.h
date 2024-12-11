#ifndef __LCD_H
#define __LCD_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "string.h"                             // Libreria que contiene la funcion strlen
#include "stdio.h"                                // Libreria que contiene la funcion sprintf
#include "Driver_SPI.h"
#include <stdbool.h>
#include <math.h>

/* Exported types ------------------------------------------------------------*/
extern osMessageQueueId_t mid_MsgQueueLCD;

typedef struct{
  uint8_t line;
  char info[256];
}MSGQUEUE_LCD_t;

/* Exported constants --------------------------------------------------------*/
#define S_TRANS_DONE_SPI  0x01
#define M_REPOSO          0x04
#define M_MANUAL          0x05
#define M_PROG_DEP        0x06

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
int Init_LCD (void);
extern osMessageQueueId_t idQueueLCD (void);
  /* Exported thread functions,  
  Example: extern void app_main (void *arg); */

#endif /* __LCD_H */
