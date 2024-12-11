#include "principal.h"
#include "hora.h"
#include "joystick.h"
#include "leds_N.h"
#include "lcd.h"
#include "mpu6050.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Init_Principal': Hilo Principal del Proyecto
 *---------------------------------------------------------------------------*/
 
 /* Identificadores Hilos*/
osThreadId_t tid_principal;                        // thread id
osThreadId_t tid_progJoy;                        // thread id
 
 /* Funciones Hilos */
void principal (void *argument);                   // thread function
void progJoy (void *argument);                   // thread function

void infoReposo (void);
void infoActivo (void);
void infoProgramacion (void);
void mensajeERROR (void);
void encendidoLeds (MSGQUEUE_MPU6050_OBJ_t valores);
void saveCircBuff(MSGQUEUE_MPU6050_OBJ_t buff);

float refEjeX = 1.0;
float refEjeY = 1.0;
float refEjeZ = 1.0;
char circBuff[10][43];

typedef enum{
  REPOSO,
  ACTIVO,
  PROGRAMACION,
  N_MODOS
} StatesPrincipal_t;

StatesPrincipal_t currentState = REPOSO; //Por defecto
MSGQUEUE_LCD_t msgLCDtx;
 
int Init_Principal (void) {
 
  tid_principal = osThreadNew(principal, NULL, NULL);
  if (tid_principal == NULL) {
    return(-1);
  }
  
  //Hilo para que esté recibiendo constantemente del joystick sin que bloquee al principal
  tid_progJoy = osThreadNew(progJoy, NULL, NULL);
  if (tid_progJoy == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void principal (void *argument) {
  
  infoReposo ();
  MSGQUEUE_MPU6050_OBJ_t infoMPUrx;
  uint32_t statusMPU = 0;
 
  while (1) {
    // Insert thread code here...
    
    switch(currentState){
      
      case REPOSO:
        
        msgLCDtx.line = 2; 
        sprintf(msgLCDtx.info, "      %u%u:%u%u:%u%u", hh/10, hh%10, mm/10, mm%10, ss/10, ss%10);
        osMessageQueuePut(idQueueLCD(), &msgLCDtx, 0U, 0U);
      
        //osDelay(1000);
      
      break;
      
      case ACTIVO:
        statusMPU = osMessageQueueGet(idQueueMPU(), &infoMPUrx, NULL, osWaitForever);
        
        if(statusMPU == osOK){
          encendidoLeds(infoMPUrx);
          saveCircBuff(infoMPUrx);
          msgLCDtx.line = 1;
          sprintf(msgLCDtx.info, " ACTIVO-- T: %.1f *C", infoMPUrx.temp); //El asterisco es el grado
          osMessageQueuePut(idQueueLCD(), &msgLCDtx, 0U, 0U);
      
          msgLCDtx.line = 2;
          sprintf(msgLCDtx.info, "  X:%1.1f Y:%1.1f Z:%1.1f", infoMPUrx.ejeX, infoMPUrx.ejeY, infoMPUrx.ejeZ);
          osMessageQueuePut(idQueueLCD(), &msgLCDtx, 0U, 0U);
          
        }else{
          mensajeERROR ();
        }
        
        break;
      
      case PROGRAMACION:
        break;
      
      default:
        break;
    }
  }
}

/* Ocurre una única vez cuando pulsamos */
void progJoy (void *argument) {
  
  MSGQUEUE_JOY_t colaJoy;

  while (1) {
    
    osMessageQueueGet(idQueueJoystick(), &colaJoy, NULL, osWaitForever);
    
    if(colaJoy.direccion == J_CNT_LNG){
      
      currentState = (currentState + 1) % N_MODOS;
      
      switch(currentState){
        
        case REPOSO:
          infoReposo ();
          startClock ();
          stopMPU ();
        break;
        
        case ACTIVO:
          infoActivo ();
          startMPU ();
        break;
        
        case PROGRAMACION:
          //infoProgramacion ();
          //stopClock();
        break;
        
        default:
        break;
      }
      
    }
    memset(&colaJoy, 0, sizeof(colaJoy));
  }
}

void infoReposo (void){
  msgLCDtx.line = 1;
  sprintf(msgLCDtx.info, "   -- SBM 2024 --");
  osMessageQueuePut(idQueueLCD(), &msgLCDtx, 0U, 0U);
}

void infoActivo (void){
  msgLCDtx.line = 1;
  sprintf(msgLCDtx.info, " ACTIVO-- T:");
  osMessageQueuePut(idQueueLCD(), &msgLCDtx, 0U, 0U);
}

void infoProgramacion (void){
  msgLCDtx.line = 1;
  sprintf(msgLCDtx.info, " --P&D--:");
  osMessageQueuePut(idQueueLCD(), &msgLCDtx, 0U, 0U);
}

void mensajeERROR (void){
  msgLCDtx.line = 1;
  sprintf(msgLCDtx.info, "   SYSTEM ERROR");
  osMessageQueuePut(idQueueLCD(), &msgLCDtx, 0U, 0U);
}

void encendidoLeds(MSGQUEUE_MPU6050_OBJ_t valores){
  
  if(valores.ejeX > refEjeX){
    osThreadFlagsSet(idLeds(), LD1_ON);
    
  }
  
  if(valores.ejeX < refEjeX){
    osThreadFlagsSet(idLeds(), LD1_OFF);
  }
  
  if(valores.ejeY > refEjeY){
    osThreadFlagsSet(idLeds(), LD2_ON);
    
  }
  
  if(valores.ejeY < refEjeY){
    osThreadFlagsSet(idLeds(), LD2_OFF);
  }
  
  if(valores.ejeZ > refEjeZ){
    osThreadFlagsSet(idLeds(), LD3_ON);
    
  }
  
  if(valores.ejeZ < refEjeZ){
    osThreadFlagsSet(idLeds(), LD3_OFF);
  }
}

void saveCircBuff(MSGQUEUE_MPU6050_OBJ_t buff){
  
  static uint8_t iter = 0;
  
  sprintf(circBuff[iter], "%u%u:%u%u:%u%u--Tm:%.1f*C-Ax:%1.1f-Ay:%1.1f-Az:%1.1f", hh/10, hh%10, mm/10, mm%10, ss/10, ss%10, buff.temp, buff.ejeX, buff.ejeY, buff.ejeZ);
  
  iter = (iter < 9) ? iter + 1 : 0;
}
