#include "compc.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
/* Identificadores hilos */
osThreadId_t tid_compc_rx;                        // thread id
osThreadId_t tid_compc_tx;                        // thread id

/* Identificadores Colas */
osMessageQueueId_t id_MsgQueue_RX;
osMessageQueueId_t id_MsgQueue_TX;

/* Driver UASART */
extern ARM_DRIVER_USART Driver_USART3;
static ARM_DRIVER_USART * USARTdrv = &Driver_USART3;
 
void myUART_Thread_rx(void* args);                   // thread function
void myUART_Thread_tx(void* args);                   // thread function
void myUSART_Callback(uint32_t event);
void processingFrame (uint32_t flagRecepcion);

EstadosCom_t estadoCom = InitState;
uint32_t statusFlag = 0;

static char byte;
static char frame[MAX_FRAME];
static int i = 0;

const uint8_t SOH = 0x01;
const uint8_t EOT = 0xFE;
 
int Init_com (void) {
 
  tid_compc_rx = osThreadNew(myUART_Thread_rx, NULL, NULL);
  if (tid_compc_rx == NULL) {
    return(-1);
  }
  
  tid_compc_tx = osThreadNew(myUART_Thread_tx, NULL, NULL);
  if (tid_compc_tx == NULL) {
    return(-1);
  }
  
  id_MsgQueue_RX = osMessageQueueNew(10, sizeof(frame), NULL);
  id_MsgQueue_TX = osMessageQueueNew(10, sizeof(frame), NULL);
 
  return(0);
}

void myUART_Thread_tx(void* args){
  
  static char txMessage[MAX_FRAME];
  
  while(1){
    osMessageQueueGet(id_MsgQueue_TX, &txMessage, NULL, osWaitForever);
//    osMessageQueueGet(id_MsgQueue_RX, &txMessage, NULL, osWaitForever); //Pongo esta para comprobar si funciona
    
    //Se envia al ordenador
    USARTdrv->Send(&txMessage, sizeof(txMessage));          /* Get byte from UART */
    osThreadFlagsWait(ARM_USART_EVENT_SEND_COMPLETE, osFlagsWaitAny, osWaitForever);
    //memset(txMessage, 0x00, 50);
  }
}
 
/* CMSIS-RTOS Thread - UART command thread */
void myUART_Thread_rx(void* args)
{
  /*Initialize the USART driver */
  USARTdrv->Initialize(myUSART_Callback);
  /*Power up the USART peripheral */
  USARTdrv->PowerControl(ARM_POWER_FULL);
  /*Configure the USART to 115200 Bits/sec */
  USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |
                    ARM_USART_DATA_BITS_8 |
                    ARM_USART_PARITY_NONE |
                    ARM_USART_STOP_BITS_1 |
                    ARM_USART_FLOW_CONTROL_NONE, 115200);
   
  /* Enable Receiver and Transmitter lines */
  USARTdrv->Control (ARM_USART_CONTROL_TX, 1);
  USARTdrv->Control (ARM_USART_CONTROL_RX, 1);
   
  while (1)
  {
    //Esto está recibiendo constantemente
    USARTdrv->Receive(&byte, 1);          /* Get byte from UART */
    statusFlag = osThreadFlagsWait(ARM_USART_EVENT_RECEIVE_COMPLETE, osFlagsWaitAny, osWaitForever);
    
    processingFrame(statusFlag);
    
  }
}

void myUSART_Callback(uint32_t event)
{
  uint32_t mask;
 
  mask = ARM_USART_EVENT_RECEIVE_COMPLETE  |
         ARM_USART_EVENT_TRANSFER_COMPLETE |
         ARM_USART_EVENT_SEND_COMPLETE     |
         ARM_USART_EVENT_TX_COMPLETE       ;
 
  if (event & ARM_USART_EVENT_RECEIVE_COMPLETE) {
    /* Success: Wakeup Thread */
    osThreadFlagsSet(tid_compc_rx, ARM_USART_EVENT_RECEIVE_COMPLETE);
  }
  
  if (event & ARM_USART_EVENT_SEND_COMPLETE) {
    /* Success: Wakeup Thread */
    osThreadFlagsSet(tid_compc_tx, ARM_USART_EVENT_SEND_COMPLETE);
  }
}

osMessageQueueId_t idQueueRX (void){
  return id_MsgQueue_RX;
}

osMessageQueueId_t idQueueTX (void){
  return id_MsgQueue_TX;
}

void processingFrame (uint32_t flagRecepcion){
  
  if(flagRecepcion == ARM_USART_EVENT_RECEIVE_COMPLETE){
    if(estadoCom == InitState){
      if(byte == SOH){
        i = 0;
        estadoCom = DefaultState;
        frame[i] = byte;
        i++;
      }
    }else{
      if(byte != EOT){
        frame[i] = byte;
        i++;
      }else{
        estadoCom = InitState;
        frame[i] = byte; //Poner BP aqui 
        
        if(frame[2] == strlen(frame)){
          osMessageQueuePut(id_MsgQueue_RX, &frame, 0U , 0U);
          memset(frame, 0x00, 50);
        }
      }
    }
  }
}
  
