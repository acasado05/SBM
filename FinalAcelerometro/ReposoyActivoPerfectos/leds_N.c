#include "leds_N.h"
 
/*--------------------------------------------------------------------------
*                             HILO LEDS
*--------------------------------------------------------------------------*/

float Ax_r, Ay_r, Az_r = 1.0;

osThreadId_t tid_leds;                        // thread id
 
void ThLeds (void *argument);                   // thread function
 
void Init_LEDs(void);
 
int Init_leds_N (void) {
 
  // HILO
  tid_leds = osThreadNew(ThLeds, NULL, NULL);
  if (tid_leds == NULL) {
    return(-1);
  }
 
  Init_LEDs();
  
  return(0);
}
 
void ThLeds (void *argument) {

  uint32_t flag_recibido;
  
  while (1) {
    // Insert thread code here...
    
    flag_recibido = osThreadFlagsWait(0X3F, osFlagsWaitAny, osWaitForever);
    
    if(LD1_ON & flag_recibido){
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    }
    
    if (LD1_OFF & flag_recibido){
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    }
    
    if (LD2_ON & flag_recibido){
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
    }
    
    if (LD2_OFF & flag_recibido){
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
    }
    
    if (LD3_ON & flag_recibido){
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
    }
    
    if (LD3_OFF & flag_recibido){
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    }
    
    //osThreadYield();                            // suspend thread
  }
}

/*--------------------------------------------------------------------------
 *                         HABILITACION LEDS
 *--------------------------------------------------------------------------*/

void Init_LEDs(void){
  
  GPIO_InitTypeDef GPIO_InitStruct;
  
  __HAL_RCC_GPIOB_CLK_ENABLE();	// Habilitar el reloj asociado al puerto de los LEDs. En este caso el reloj del puerto B
  
  // Configuracion de los LEDs VERDE|AZUL|ROJO  = PIN 0|7|14
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
}

osThreadId_t idLeds (void){
  return tid_leds;
}
