#include "principal.h"

/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread;                        // thread id

MSGQUEUE_OBJ_JOY_t rxMsg;
uint8_t pos_joy = 0;
 
void Thread (void *argument);                   // thread function
 
int Init_Thread (void) {
 
  tid_Thread = osThreadNew(Thread, NULL, NULL);
  if (tid_Thread == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread (void *argument) {
 
  while (1) {
    // Insert thread code here...
    osMessageQueueGet(mid_MsgQueue_joystick, &rxMsg, NULL, osWaitForever);
    pos_joy = rxMsg.direccion;
    osThreadYield();                            // suspend thread
  }
}
