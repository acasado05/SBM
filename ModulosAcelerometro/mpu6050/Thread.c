#include "Thread.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread;                        // thread id

MSGQUEUE_MPU6050_OBJ_t rxMsg;

float lecX = 0.0;
float lecY = 0.0;
float lecZ = 0.0;
float temp = 0.0;
 
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
    osMessageQueueGet(mid_MsgQueue_MPU, &rxMsg, NULL, osWaitForever);
    lecX = rxMsg.ejeX;
    lecY = rxMsg.ejeY;
    lecZ = rxMsg.ejeZ;
    temp = rxMsg.temp;
    
    //osThreadYield();                            // suspend thread
  }
}
