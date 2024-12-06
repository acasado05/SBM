#include "Thread.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread;                        // thread id
osTimerId_t id_tim_1s;
 
void Thread (void *argument);                   // thread function
void Timer_Callback_1s (void);
static void MODO_REPOSO (void);
static void MODO_ACTIVO (void);
static void MODO_PROG (void);

uint8_t modo = 0;
 
int Init_Thread (void) {
 
  tid_Thread = osThreadNew(Thread, NULL, NULL);
  if (tid_Thread == NULL) {
    return(-1);
  }
  
  id_tim_1s = osTimerNew((osTimerFunc_t)&Timer_Callback_1s, osTimerPeriodic, NULL, NULL);
  if (id_tim_1s != NULL){
    return -1;
  }
 
  return(0);
}
 
void Thread (void *argument) {
 
  osTimerStart(id_tim_1s, 1000U);
  while (1) {
    // Insert thread code here...
    if(modo >= 1 && modo == 5){
      MODO_REPOSO ();
      
    }else if(modo >= 6 && modo == 10){
      MODO_ACTIVO ();
      
    }else if(modo >= 10 && modo == 15){
      MODO_PROG ();
    }
    osThreadYield();                            // suspend thread
  }
}

void Timer_Callback_1s (void){
  modo += 1;
}

static void MODO_REPOSO (void){
  
  uint8_t hor = 21;
  uint8_t min = 14;
  uint8_t seg = 32;
  
  MSGQUEUE_LCD_t infoReposo;
  char texto[13] = "     SBM 2024";
  infoReposo.state = M_REPOSO;
  
  infoReposo.line = 1;
  sprintf(infoReposo.info, "%s", texto); //5 espacios
  osMessageQueuePut(mid_MsgQueueLCD, &infoReposo, 0U, 0U);
  
  infoReposo.line = 2;
  sprintf(infoReposo.info, "      %u%u:%u%u:%u%u", hor/10, hor%10, min/10, min%10, seg/10, seg%10);
  osMessageQueuePut(mid_MsgQueueLCD, &infoReposo, 0U, 0U);
}

static void MODO_ACTIVO (void){
  
  MSGQUEUE_LCD_t infoActivo;
  char texto[17] = "  ACTIVO-- T:";
  infoActivo.state = M_MANUAL;
  
  double ejeX = -0.9;
  double ejeY = -0.9;
  double ejeZ = -0.9;
  double temp = 21.5;
  
  infoActivo.line = 1;
  sprintf(infoActivo.info, "%s %.1f *C",texto, temp);
  osMessageQueuePut(mid_MsgQueueLCD, &infoActivo, 0U, 0U);
  
  infoActivo.line = 2;
  sprintf(infoActivo.info, "   X:%01.1f Y:%01.1f Z:%01.1f", round(ejeX/0.5)*0.5, round(ejeY/0.5)*0.5, round(ejeZ/0.5)*0.5);
  osMessageQueuePut(mid_MsgQueueLCD, &infoActivo, 0U, 0U);
}

static void MODO_PROG (void){
  
  uint8_t hor = 21;
  uint8_t min = 14;
  uint8_t seg = 32;
  
  MSGQUEUE_LCD_t infoProg;
  char texto[18] = "       ---P&D---";
  infoProg.state = M_PROG_DEP;
  
  infoProg.line = 1;
  sprintf(infoProg.info, "%s", texto);
  osMessageQueuePut(mid_MsgQueueLCD, &infoProg, 0U, 0U);
  
  infoProg.line = 2;
  sprintf(infoProg.info, "      H: %u%u:%u%u:%u%u", hor/10, hor%10, min/10, min%10, seg/10, seg%10);
  osMessageQueuePut(mid_MsgQueueLCD, &infoProg, 0U, 0U);
}
