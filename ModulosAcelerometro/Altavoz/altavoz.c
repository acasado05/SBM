#include "altavoz.h"                          // CMSIS RTOS header file
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_altavoz;                        // thread id
osMessageQueueId_t id_MsgQueue_Altavoz;

TIM_HandleTypeDef htim2;
TIM_OC_InitTypeDef TIM_Channel_InitStruct;
 
void altavoz (void *argument);                   // thread function
void initTimer2_OC (void);
 
int Init_Altavoz (void) {
 
  tid_altavoz = osThreadNew(altavoz, NULL, NULL);
  if (tid_altavoz == NULL) {
    return(-1);
  }
  
  id_MsgQueue_Altavoz = osMessageQueueNew(3, sizeof(bool), NULL);
 
  return(0);
}
 
void altavoz (void *argument) {
 
  bool spk = false;
  
  initTimer2_OC ();
  
  while (1) {
    // Insert thread code here...
    osMessageQueueGet(id_MsgQueue_Altavoz, &spk, NULL, osWaitForever);
    
    if(spk){
      HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_1);
    }else{
      HAL_TIM_OC_Stop(&htim2, TIM_CHANNEL_1);
    }
    osThreadYield();                            // suspend thread
  }
}

void initTimer2_OC (void){
  
  GPIO_InitTypeDef GPIO_InitStruct;
  
  //Timer normal en FA
  __HAL_RCC_GPIOA_CLK_ENABLE ();
  
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  //Timer OC
  __HAL_RCC_TIM2_CLK_ENABLE();
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 4199;
  htim2.Init.Period = 19;
  HAL_TIM_OC_Init(&htim2);
  
  TIM_Channel_InitStruct.OCMode = TIM_OCMODE_TOGGLE;
  TIM_Channel_InitStruct.OCPolarity = TIM_OCPOLARITY_LOW;
  TIM_Channel_InitStruct.OCFastMode = TIM_OCFAST_DISABLE;
  
  HAL_TIM_OC_ConfigChannel(&htim2, &TIM_Channel_InitStruct, TIM_CHANNEL_1);
}

osMessageQueueId_t idQueueAltavoz (void){
  return id_MsgQueue_Altavoz;
}
