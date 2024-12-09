#ifndef __COMPC_H
#define __COMPC_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Driver_USART.h"
#include "string.h"
#include "stdio.h"

/* Exported types ------------------------------------------------------------*/
typedef enum{
  InitState,
  DefaultState
} EstadosCom_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
#define INIT_USART_RX 0x01U
#define INIT_USART_TX 0x01U
#define MAX_FRAME 50
/* Exported functions ------------------------------------------------------- */
int Init_com (void);
  /* Exported thread functions,  
  Example: extern void app_main (void *arg); */

#endif /* __COMPC_H */
