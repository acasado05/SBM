#include "joystick.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
/* Variables Thread */
osThreadId_t tid_joystick;                        // thread id

osMessageQueueId_t mid_MsgQueue_joystick;           // Identificador de la cola
MSGQUEUE_OBJ_JOY_t txMsg;


/* Variables Timer */
osTimerId_t id_tim_20ms;
osStatus_t status_20ms;

osTimerId_t id_tim_50ms;
osStatus_t status_50ms;

uint16_t centro = 0;
uint8_t bounce = 0;

GPIO_InitTypeDef GPIO_InitStruct;
 
void ThJoystick (void *argument);                   // thread function
void Timer_Callback_20ms (void const *arg);
void Timer_Callback_50ms (void const *arg);
void Init_Pines (void);
 
int Init_Joystick(void) {
 
  tid_joystick = osThreadNew(ThJoystick, NULL, NULL);
  if (tid_joystick == NULL) {
    return(-1);
  }
  
  /* Timer REBOTES */
  id_tim_20ms = osTimerNew((osTimerFunc_t)&Timer_Callback_20ms, osTimerOnce, NULL, NULL); //Timer REBOTES
  if(id_tim_20ms != NULL){
    if(status_20ms != osOK){
    return -1;
    }
  }
  
  /* Timer PULSACION */
  id_tim_50ms = osTimerNew((osTimerFunc_t)&Timer_Callback_50ms, osTimerPeriodic, NULL, NULL); //Timer PULSACION
  if(id_tim_50ms != NULL){
    if(status_50ms != osOK){
      return -1;
    }
  }
  
  /* Inicialización Cola */
  mid_MsgQueue_joystick = osMessageQueueNew(MSGQUEUE_OBJECTS_JOY, sizeof(MSGQUEUE_OBJ_JOY_t), NULL);
  
  Init_Pines ();
 
  return(0);
}
 
void ThJoystick (void *argument) {
 
  while (1) {
    // Insert thread code here...
    osThreadFlagsWait(PULSACION, osFlagsWaitAny, osWaitForever); //Espero a cualquiera de ellos: 0011 1111 (Tengo esos 5)
    osTimerStart(id_tim_20ms, 20U);
    //osThreadYield();                            // suspend thread
  }
}

void Timer_Callback_20ms (void const *arg){
  
  if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_SET){ //ARRIBA
    bounce += 1;
    txMsg.direccion = J_UP;
  }
  else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_SET){ //DERECHA
    bounce += 1;
    txMsg.direccion = J_RGHT;
  }
  else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12) == GPIO_PIN_SET){ //ABAJO
    bounce += 1;
    txMsg.direccion = J_DOWN;
  }
  else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_14) == GPIO_PIN_SET){ //IZQUIERDA
    bounce += 1;
    txMsg.direccion = J_LEFT;
  }
  else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15) == GPIO_PIN_SET){ //CENTRO
    bounce += 1;
    osTimerStart(id_tim_50ms, 50U);
  }
  
  osMessageQueuePut(mid_MsgQueue_joystick, &txMsg, 0U, 0U);
}

//Lo considero pulsación larga si: 1/50 ms = 20 veces o mayor!
void Timer_Callback_50ms (void const *arg){
  
  
  if(HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_15) == GPIO_PIN_SET){
    centro += 1;
    
    if(centro >= 20){
      osTimerStop(id_tim_50ms);
      txMsg.direccion = J_CNT_LNG;
      osMessageQueuePut(mid_MsgQueue_joystick, &txMsg, 0U, 0U);
      centro = 0;
    }
  }
  
  if(HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_15) == GPIO_PIN_RESET){
    osTimerStop(id_tim_50ms);
    txMsg.direccion = J_CNT_SHRT;
    osMessageQueuePut(mid_MsgQueue_joystick, &txMsg, 0U, 0U);
    centro = 0;
  }
  
}

void Init_Pines (void){
  
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;  //Detecta los flancos de subida
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  
  GPIO_InitStruct.Pin= GPIO_PIN_11; //DERECHO
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = GPIO_PIN_10;//ARRIBA
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = GPIO_PIN_12;//ABAJO
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = GPIO_PIN_14;//IZQUIERDA
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = GPIO_PIN_15;//CENTRO
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn); //Llamada a la interrupci?n del joystick
}
