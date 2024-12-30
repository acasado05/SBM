#ifndef __PRINCIPAL_H
#define __PRINCIPAL_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stdbool.h"
#include "math.h"

/* Exported types ------------------------------------------------------------*/
typedef enum{
  REPOSO,
  ACTIVO,
  PROGRAMACION,
  N_MODOS
} StatesPrincipal_t;

typedef enum{
  DEC_HOR,
  UNI_HOR,
  DEC_MIN,
  UNI_MIN,
  DEC_SS,
  UNI_SS,
  N_EDIT
} EdicionHora_t;

typedef enum{
  INTEGER,
  DECIMAL,
  N_NUM
} EdicionRef_t;

typedef enum{
  TIME,
  REF_X,
  REF_Y,
  REF_Z,
  N_REF
} DepurationMode_t;



/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
int Init_Principal (void);
  /* Exported thread functions,  
  Example: extern void app_main (void *arg); */

#endif /* __PRINCIPAL_H */