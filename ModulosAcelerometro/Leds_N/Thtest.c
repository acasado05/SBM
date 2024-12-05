#include "leds_N.h"

/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread;                        // thread id
 
void Thread (void *argument);                   // thread function

osTimerId_t tim_timer_id;
static uint32_t exec;

uint8_t flag = 0;

static void Timer_Callback(void){
	
	flag++;
	
	switch(flag){
		case 1:
			osThreadFlagsSet(tid_ThLeds, LED1_ON);
			osThreadFlagsSet(tid_ThLeds, LED2_ON);
			osThreadFlagsSet(tid_ThLeds, LED3_ON);
		break;
		
		case 2:
			osThreadFlagsSet(tid_ThLeds, LED1_OFF);
			osThreadFlagsSet(tid_ThLeds, LED3_ON);
		break;
			
		case 3:
			osThreadFlagsSet(tid_ThLeds, LED2_OFF);
		break;
		
		case 4:
			osThreadFlagsSet(tid_ThLeds, LED2_ON);
			osThreadFlagsSet(tid_ThLeds, LED1_ON);
		break;
			
		case 5:
			osThreadFlagsSet(tid_ThLeds, LED3_OFF);
		break;
		
		case 6:
			osThreadFlagsSet(tid_ThLeds, LED3_ON);
			flag = 0;
		break;
	}
	
	
}

int Init_Thread (void) {
 
  tid_Thread = osThreadNew(Thread, NULL, NULL);
  if (tid_Thread == NULL) {
    return(-1);
  }
	
	osStatus_t status;
	
  exec = 2U;
  tim_timer_id=  osTimerNew((osTimerFunc_t)&Timer_Callback, osTimerPeriodic, &exec, NULL);
  if(tim_timer_id != NULL){
    //Se inicializa con un intervalo de xxx ms
    //status = osTimerStart(tim_timer_id,xxxU);
    if(status != osOK){
      return -1;
    }
  }
	
  return(0);
}
 
void Thread (void *argument) {
	
	osTimerStart(tim_timer_id, 2000);
	
  while (1) {
    // Insert thread code here...
		
		//osThreadYield();                            // suspend thread
  }
}
