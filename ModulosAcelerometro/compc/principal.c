#include "principal.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_compc_rx_test;                        // thread id
osThreadId_t tid_compc_tx_test;                        // thread id
 
void myUART_Thread_tx_test(void* args);                   // thread function
void myUART_Thread_rx_test(void* args);                   // thread function
 
int Init_Thread (void) {
 
  tid_compc_rx_test = osThreadNew(myUART_Thread_rx_test, NULL, NULL);
  if (tid_compc_rx_test == NULL) {
    return(-1);
  }
  
  tid_compc_tx_test = osThreadNew(myUART_Thread_tx_test, NULL, NULL);
  if (tid_compc_tx_test == NULL) {
    return(-1);
  }
 
  return(0);
}
 //Aqui mandamos la trama que simula lo enviado por el hilo principal al teraterm
 //pero previamente pasa por el hilo del COMPC para hacer el send.
void myUART_Thread_tx_test(void* args){
  
  static char txMessageF1[MAX_FRAME] = {0x01,0x20,0x0C,0x31,0x35,0x3A,0x32,0x30,0x3A,0x31,0x35,0xFE}; 
  static char txMessageF2[MAX_FRAME] = {0x01,0x25,0x08,0x30,0x2E,0x37,0x35,0xFE};
  
  osDelay(5000);
  osMessageQueuePut(idQueueTX (), &txMessageF1, 0U , 0U);
  osDelay(5000);
  osMessageQueuePut(idQueueTX (), &txMessageF2, 0U , 0U);
  
  while(1){
    osThreadYield();
  }
}

//Aqui recibe lo que se envia por el TERATERM y se mandará al principal para procesarla!!
void myUART_Thread_rx_test(void* args){
  static char rxMessageTest[MAX_FRAME];
  
  while(1){
    osMessageQueueGet(idQueueRX (), &rxMessageTest, NULL, osWaitForever);
    osDelay(5000); //BP aqui para ver las tramas de recepción
  }
}
