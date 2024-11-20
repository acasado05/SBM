#include "joystick.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
/* Variables Thread */
osThreadId_t tid_joystick;                        // thread id

/* Variables Timer */
osTimerId_t id_tim20s;
static uint32_t exec; 
uint8_t pin = 0;
uint8_t bounce = 0;

GPIO_InitTypeDef GPIO_InitStruct;
 
void ThJoystick (void *argument);                   // thread function
void Timer_Callback_20ms (void const *arg);
void Init_Pines (void);
void Init_LEDs(void);
 
int Init_Joystick(void) {
 
  tid_joystick = osThreadNew(ThJoystick, NULL, NULL);
  if (tid_joystick == NULL) {
    return(-1);
  }
  
  id_tim20s = osTimerNew((osTimerFunc_t)&Timer_Callback_20ms, osTimerOnce, NULL, NULL); //Solo quieren 3 segundos

  Init_Pines ();
  Init_LEDs ();
 
  return(0);
}
 
void ThJoystick (void *argument) {
 
  while (1) {
    // Insert thread code here...
    osThreadFlagsWait(0x1F, osFlagsWaitAny, osWaitForever); //Espero a cualquiera de ellos: 0001 1111 (Tengo esos 5)
    osTimerStart(id_tim20s, 20U);
    //osThreadYield();                            // suspend thread
  }
}

void Timer_Callback_20ms (void const *arg)
{
  if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_SET){
    pin = J_UP;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    bounce++;
  }
  else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_SET){
    pin = J_RGHT;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    bounce++;
  }
  else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12) == GPIO_PIN_SET){
    pin = J_DOWN;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    bounce++;
  }
  else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_14) == GPIO_PIN_SET){
    pin = J_LEFT;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
    bounce++;
  }
  else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15) == GPIO_PIN_SET){
    pin = J_CNT;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
    bounce++;
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
  
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn); //Llamada a la interrupción del joystick
}

void Init_LEDs(void){ 

  GPIO_InitTypeDef GPIO_InitStruct; 
  
  /*Enable clock to GPIO-B*/
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  /*Set GPIOB pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  

  /*Init GPIOB Pins*/
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);  //VERDE
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);  //AZUL
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); //ROJO
}
