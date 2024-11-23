#ifndef __THLCD_H
#define __THLCD_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "string.h"                             // Libreria que contiene la funcion strlen
#include "stdio.h"                                // Libreria que contiene la funcion sprintf
#include "Driver_SPI.h"
#include <stdbool.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
int Init_Thread (void);
  /* Exported thread functions,  
  Example: extern void app_main (void *arg); */

#endif /* __THLCD_H */
