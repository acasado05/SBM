#include "principal.h"
#include "hora.h"
#include "joystick.h"
#include "leds_N.h"
#include "lcd.h"
#include "mpu6050.h"
#include "compc.h"
#include "altavoz.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Init_Principal': Hilo Principal del Proyecto
 *---------------------------------------------------------------------------*/
 
 /* Identificadores Hilos*/
osThreadId_t tid_principal;                        // thread id
 
 /* Funciones Hilos */
void principal (void *argument);                   // thread function

void infoReposo (void);
void infoActivo (void);
void infoProgramacion (void);
void mensajeERROR (void);
void encendidoLeds (MSGQUEUE_MPU6050_OBJ_t valores);
void apagadoLeds (void);
void apagarAltavoz (void);
void saveCircBuff(MSGQUEUE_MPU6050_OBJ_t buff);
void updateTime (MSGQUEUE_JOY_t pulsacion);
void updateReference (MSGQUEUE_JOY_t pulsacion);
uint8_t setUnidades (uint8_t uni);
uint8_t setDecenas (uint8_t dec);
void editClock (MSGQUEUE_JOY_t joy);
void processFrame (char array[]);
void cleanCircBuff(void);
void sendCircBuff (void);

float refEjeX = 1.0;
float refEjeY = 1.0;
float refEjeZ = 1.0;
uint8_t hhEdit, mmEdit, ssEdit = 0;
double xEdit, yEdit, zEdit = 0.0;
char circBuff[10][43];

StatesPrincipal_t currentState = REPOSO; //Por defecto
EdicionHora_t editadoHora = DEC_HOR;
EdicionRef_t edicionReferencia = INTEGER;
DepurationMode_t modoEdit = TIME;

float vRefEdit = 0.0;

MSGQUEUE_LCD_t msgLCDtx;


int Init_Principal (void) {
 
  tid_principal = osThreadNew(principal, NULL, NULL);
  if (tid_principal == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void principal (void *argument) {
  
  char frameRX[MAX_FRAME];
  MSGQUEUE_MPU6050_OBJ_t infoMPUrx;
  MSGQUEUE_JOY_t colaJoy;
  uint32_t statusMPU = 0;
  uint32_t statusCom = 0;
 
  while (1) {
    // Insert thread code here...
    osMessageQueueGet(idQueueJoystick(), &colaJoy, NULL, 0U);
    
    if(colaJoy.direccion == J_CNT_LNG){
      
      currentState = (currentState + 1) % N_MODOS;
      
      if(currentState == ACTIVO){
        apagadoLeds();
        osMessageQueueReset(idQueueMPU());
      }else if(currentState == PROGRAMACION){
        infoProgramacion ();
        osMessageQueueReset(idQueueRX ());
      }
    }
    
    switch(currentState){
      case REPOSO:
        
        infoReposo ();
        msgLCDtx.line = 2; 
        msgLCDtx.lowBar = 0;
        sprintf(msgLCDtx.info, "      %u%u:%u%u:%u%u", hh/10, hh%10, mm/10, mm%10, ss/10, ss%10);
        osMessageQueuePut(idQueueLCD(), &msgLCDtx, 0U, 0U);
      
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
          
        }
        
        break;
      
      case PROGRAMACION:
        //RECEPCION DE LA COLA DE LA USART
      //memset(frameRX, 0, sizeof(frameRX));
        statusCom = osMessageQueueGet(idQueueRX(), &frameRX, NULL, 100U);
        
        if(statusCom == osOK){
          processFrame(frameRX);
          memset(frameRX, 0, sizeof(frameRX));
        }
      
        if(colaJoy.direccion != J_CNT_LNG && colaJoy.direccion != 0){
          if(modoEdit == TIME){
            updateTime(colaJoy);
          }else{
            updateReference(colaJoy);
          }
        }
        
        
        osThreadYield();
        break;
        
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
  
  apagadoLeds ();
  apagarAltavoz ();
  hhEdit = hh;
  mmEdit = mm;
  ssEdit = ss;
  vRefEdit = refEjeX;
  
  msgLCDtx.line = 1;
  sprintf(msgLCDtx.info, "      ---P&D---");
  osMessageQueuePut(idQueueLCD(), &msgLCDtx, 0U, 0U);
  
  msgLCDtx.line = 2;
  msgLCDtx.lowBar = 53;
  sprintf(msgLCDtx.info, "     H: %u%u:%u%u:%u%u", hhEdit/10, hhEdit%10, mmEdit/10, mmEdit%10, ssEdit/10, ssEdit%10);
  osMessageQueuePut(idQueueLCD(), &msgLCDtx, 0U, 0U);
}

void encendidoLeds(MSGQUEUE_MPU6050_OBJ_t valores){
  
  bool altavoz = false;
  uint32_t flagAltavoz = 0;
  
  if(valores.ejeX > refEjeX){
    osThreadFlagsSet(idLeds(), LD1_ON);
    flagAltavoz |= LD1_ON;
  }
  
  if(valores.ejeX < refEjeX){
    osThreadFlagsSet(idLeds(), LD1_OFF);
  }
  
  if(valores.ejeY > refEjeY){
    osThreadFlagsSet(idLeds(), LD2_ON);
    flagAltavoz |= LD2_ON;
    
  }
  
  if(valores.ejeY < refEjeY){
    osThreadFlagsSet(idLeds(), LD2_OFF);
  }
  
  if(valores.ejeZ > refEjeZ){
    osThreadFlagsSet(idLeds(), LD3_ON);
    flagAltavoz |= LD3_ON;
    
  }
  
  if(valores.ejeZ < refEjeZ){
    osThreadFlagsSet(idLeds(), LD3_OFF);
  }
  
  altavoz = ((flagAltavoz && LD1_ON) || (flagAltavoz && LD2_ON) || (flagAltavoz && LD3_ON)) ? true : false;
  osMessageQueuePut(idQueueAltavoz (), &altavoz, 0U, 0U);
}

void apagadoLeds (void){
  osThreadFlagsSet(idLeds(), LD1_OFF);
  osThreadFlagsSet(idLeds(), LD2_OFF);
  osThreadFlagsSet(idLeds(), LD3_OFF);
}

void apagarAltavoz (void){
  bool apagar = false;
  
  osMessageQueuePut(idQueueAltavoz (), &apagar, 0U, 0U);
}

void saveCircBuff(MSGQUEUE_MPU6050_OBJ_t buff){
  
  static uint8_t iter = 0;
  
  sprintf(circBuff[iter], "%u%u:%u%u:%u%u--Tm:%.1f*C-Ax:%1.1f-Ay:%1.1f-Az:%1.1f", hh/10, hh%10, mm/10, mm%10, ss/10, ss%10, buff.temp, buff.ejeX, buff.ejeY, buff.ejeZ);
  
  iter = (iter < 9) ? iter + 1 : 0;
}

void updateTime (MSGQUEUE_JOY_t pulsacion){
  
  int mod = 0;
  uint8_t sel = 9;
  
  uint8_t decHH = setDecenas(hhEdit);
  uint8_t uniHH = setUnidades(hhEdit);
  
  uint8_t decMM = setDecenas(mmEdit);
  uint8_t uniMM = setUnidades(mmEdit);
  
  uint8_t decSS = setDecenas(ssEdit);
  uint8_t uniSS = setUnidades(ssEdit);
  
  if(pulsacion.direccion == J_UP){
    mod = 1;
  }else if(pulsacion.direccion == J_DOWN){
    mod = -1; 
  }
  
  if(pulsacion.direccion == J_RGHT){
    editadoHora = (editadoHora < 5) ? (editadoHora + 1) : 0;
  }else if(pulsacion.direccion == J_LEFT){
    editadoHora = (editadoHora > 0) ? (editadoHora - 1) : 5;
  }
  
  if(pulsacion.direccion == J_UP || pulsacion.direccion == J_DOWN){
    switch(editadoHora){
      
      case DEC_HOR:
        decHH = (decHH + mod + 3) % 3;
        hhEdit = decHH*10 + uniHH;
        break;
      
      case UNI_HOR:
        if(decHH != 2){
          uniHH = (uniHH + mod + 10) % 10;
        }else{
          uniHH = (uniHH + mod + 4) % 4;
        }
        hhEdit = decHH*10 + uniHH;
        break;
      
      case DEC_MIN:
        decMM = (decMM + mod + 6) % 6;
        mmEdit = decMM*10 + uniMM;
        break;
      
      case UNI_MIN:
        uniMM = (uniMM + mod + 10) % 10;
        mmEdit = decMM*10 + uniMM;
        break;
      
      case DEC_SS:
        decSS = (decSS + mod + 6) % 6;
        ssEdit = decSS*10 + uniSS;
        break;
      
      case UNI_SS:
        uniSS = (uniSS + mod + 10) % 10;
        ssEdit = decSS*10 + uniSS;
        break;
      
      default:
        break;
    }
  }
  
  if(pulsacion.direccion == J_CNT_SHRT){
    hh = hhEdit;
    mm = mmEdit;
    ss = ssEdit;
    modoEdit = REF_X;
    sprintf(msgLCDtx.info, "     Ref_X: %1.1f", vRefEdit);
  }else{
    sprintf(msgLCDtx.info, "     H: %u%u:%u%u:%u%u", hhEdit/10, hhEdit%10, mmEdit/10, mmEdit%10, ssEdit/10, ssEdit%10);
  }
  
  if(editadoHora == DEC_HOR){
    sel = 53;
  }else if(editadoHora == UNI_HOR){
    sel = 58;
  }else if(editadoHora == DEC_MIN){
    sel = 65;
  }else if(editadoHora == UNI_MIN){
    sel = 70;
  }else if(editadoHora == DEC_SS){
    sel = 77;
  }else if(editadoHora == UNI_SS){
    sel = 83;
  }
  
  msgLCDtx.line = 2;
  msgLCDtx.lowBar = (modoEdit == TIME) ? (sel + editadoHora) : 53;
  osMessageQueuePut(idQueueLCD(), &msgLCDtx, 0U, 0U);
}

void updateReference (MSGQUEUE_JOY_t joystick){
  
  double integer, decimal;
  uint8_t sel = 9;
  
  decimal = modf(vRefEdit,&integer); //Only expects double
  
  if(joystick.direccion == J_UP){
    if(edicionReferencia == INTEGER){
      integer = (integer == 0 && decimal < 0) ? (1+(decimal*-1*2)) : (integer + 1);
    }else{
      decimal = decimal + 0.1;
    }
    vRefEdit = ((integer + decimal) > 2.0) ? 2.0 : integer + decimal;
    
  }else if(joystick.direccion == J_DOWN){
    if(edicionReferencia == INTEGER){
      integer = (integer == 0 && decimal > 0) ? ((decimal*2*-1)-1) : (integer - 1);
    }else{
      decimal = decimal - 0.1;
    }
    vRefEdit = ((integer + decimal) < -2.0) ? -2.0 : integer + decimal;
  }
  
  if(joystick.direccion == J_RGHT){
    edicionReferencia = (edicionReferencia < 1) ? (edicionReferencia + 1) : 0;
  }else if(joystick.direccion == J_LEFT){
    edicionReferencia = (edicionReferencia > 0) ? (edicionReferencia - 1) : 1;
  }
  
  if(joystick.direccion == J_CNT_SHRT){
    if(modoEdit == REF_X){
      refEjeX = vRefEdit;
      vRefEdit = refEjeY;
    }else if(modoEdit == REF_Y){
      refEjeY = vRefEdit;
      vRefEdit = refEjeZ;
    }else if(modoEdit == REF_Z){
      refEjeZ = vRefEdit;
      vRefEdit = refEjeX;
    }
    modoEdit = (modoEdit + 1) % N_REF;
  }
  
  if(modoEdit == REF_X){
    sprintf(msgLCDtx.info, "     Ref_X: %1.1f", vRefEdit);
  }else if(modoEdit == REF_Y){
    sprintf(msgLCDtx.info, "     Ref_Y: %1.1f", vRefEdit);
  }else if(modoEdit == REF_Z){
    sprintf(msgLCDtx.info, "     Ref_Z: %1.1f", vRefEdit);
  }else if(modoEdit == TIME){
    sprintf(msgLCDtx.info, "     H: %u%u:%u%u:%u%u", hhEdit/10, hhEdit%10, mmEdit/10, mmEdit%10, ssEdit/10, ssEdit%10);
  }
  
  msgLCDtx.line = 2;
  osMessageQueuePut(idQueueLCD(), &msgLCDtx, 0U, 0U);
  
  if(modoEdit != TIME){
    msgLCDtx.lowBar = (edicionReferencia == INTEGER) ? 77 : 85;
    if(vRefEdit < 0){
      msgLCDtx.lowBar = (edicionReferencia == INTEGER) ? 78 : 88;
    }
  }else{
    msgLCDtx.lowBar = 53;
  }
  
  osMessageQueuePut(idQueueLCD(), &msgLCDtx, 0U, 0U);
  
}

uint8_t setUnidades (uint8_t uni){
  return uni%10;
}

uint8_t setDecenas (uint8_t dec){
  return dec/10;
}

void cleanCircBuff (void){
  
  uint8_t i, j;
  
  for(i = 0; i < 10; i++){
    for(j = 0; j < 41; j++){
      circBuff[i][j] = 0;
    }
  }
}

void sendCircBuff (void){
  
  uint8_t i, j, n;
  uint8_t length;
  
  char frameCircBuff[MAX_FRAME] = {0};
  
  frameCircBuff[0] = 0x01; 
  frameCircBuff[1] = 0xAF;  
  
  for(i = 0; i < 10; i++){
    memset(frameCircBuff+2, 0, MAX_FRAME);
    if(circBuff[i] != 0){
      for(j = 0; j < 43; j++){
        if(circBuff[i][j] != '\0'){
          frameCircBuff[j+3] = circBuff[i][j];
        }else{
          break;      
        }
      }
      length = strlen(frameCircBuff);
      frameCircBuff[2] = length+1;  
      frameCircBuff[length] = 0xFE;
      osMessageQueuePut(idQueueTX(), &frameCircBuff, 0U, 0U);
    }
  }
}

void processFrame (char array[]){
  
  char frame[MAX_FRAME] = {0};
  uint8_t n = 0;
  
  for(n = 0; n < MAX_FRAME; n++){
    if(array[n] != 0xFE){
      frame[n] = array[n];
      
    }else{
      frame[n] = array[n];
      break; //OUT
    }
  }
  
  switch(frame[1]){
    
    case 0x20:
      
      hh = (frame[3] - 0x30)*10 + (frame[4] - 0x30);
      mm = (frame[6] - 0x30)*10 + (frame[7] - 0x30);
      ss = (frame[9] - 0x30)*10 + (frame[10] - 0x30);
      frame[1] = 0xDF;
    
      sprintf(msgLCDtx.info, "     H: %u%u:%u%u:%u%u", hh/10, hh%10, mm/10, mm%10, ss/10, ss%10);
      modoEdit = TIME;
      hhEdit = hh;
      mmEdit = mm;
      ssEdit = ss;
    
      break;
    
    case 0x25:
      refEjeX = (frame[3] - 0x30)+((double)(frame[5] - 0x30))/10 + ((double)(frame[6] - 0x30))/100;
      vRefEdit = refEjeX;
      sprintf(msgLCDtx.info, "     Ref_X:%1.2f", vRefEdit);
      frame[1] = 0xDA;
      
    break;
    
    case 0x26:
      
      refEjeY = (frame[3] - 0x30)+((double)(frame[5] - 0x30))/10 + ((double)(frame[6] - 0x30))/100;
      vRefEdit = refEjeY;
      sprintf(msgLCDtx.info, "     Ref_Y:%1.2f", vRefEdit);
      frame[1] = 0xD9;
      
    break;
    
    case 0x27:
      
      refEjeZ = (frame[3] - 0x30)+((double)(frame[5] - 0x30))/10 + ((double)(frame[6] - 0x30))/100;
      vRefEdit = refEjeZ;
      sprintf(msgLCDtx.info, "     Ref_Z:%1.2f", vRefEdit);
      frame[1] = 0xD8;
      
    break;
    
    case 0x55:
      sendCircBuff();
    break;
    
    case 0x60:
      cleanCircBuff();
      frame[1] = 0x9F;
      
    break;
  }
  
  if(frame[1] != 0x55){
    msgLCDtx.line = 2;
    osMessageQueuePut(idQueueLCD(), &msgLCDtx, 0U, 0U);
    osMessageQueuePut(idQueueTX(), &frame, 0U, 0U);
    memset(frame, 0x00, 50);
  }
}
