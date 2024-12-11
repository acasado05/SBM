#ifndef __JOYSTICK_H
#define __JOYSTICK_H

/* Liber?as */
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"

/* Macros */
#define J_UP        0x01
#define J_RGHT      0x02
#define J_DOWN      0x03
#define J_LEFT      0x04
#define J_CNT_SHRT  0x05
#define J_CNT_LNG   0x06
#define PULSACION   0xFF

#define MSGQUEUE_OBJECTS_JOY 1                   		// Cantidad de objetos de la cola de mensajes

/* Variables externas */
extern osThreadId_t tid_joystick;                        // thread id
extern osMessageQueueId_t mid_MsgQueue_joystick;           // Identificador de la cola
extern osMessageQueueId_t idQueueJoystick (void);

/* Funciones otros m?dulos */ 
int Init_Joystick (void);

/* Estructura */
typedef struct{
    uint8_t direccion;
}MSGQUEUE_JOY_t;

#endif /* __JOYSTICK_H */
