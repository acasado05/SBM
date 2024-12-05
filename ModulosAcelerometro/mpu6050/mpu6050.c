#include "mpu6050.h"                          // CMSIS RTOS header file
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_mpu6050;                        // thread id
osMessageQueueId_t mid_MsgQueue_MPU;           // Identificador de la cola

MSGQUEUE_MPU6050_OBJ_t txMsg;

extern ARM_DRIVER_I2C            Driver_I2C1; 
static ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C1;
int8_t status_I2C;
 
void Thread_MPU6050 (void *argument);                   // thread function
int8_t Init_I2C (void);
void Callback_I2C (uint32_t event);
void Timer_Callback_1s (void);
void PWR_ON_MPU6050 (void);
static float RD_ACCEL_MPU (uint8_t reg_address);
static float RD_TEMP_MPU (uint8_t reg_address);
void read_MPU6050 (void);

bool comienzoLectura = true;

uint8_t txDATA [2];
uint8_t rxBuffer[8];
float accel[3];
float temperatura;

 
int Init_MPU6050 (void) {
 
  tid_mpu6050 = osThreadNew(Thread_MPU6050, NULL, NULL);
  if (tid_mpu6050 == NULL) {
    return(-1);
  }
  
  mid_MsgQueue_MPU = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_MPU6050_OBJ_t), NULL);
  
  return(0);
}
 
void Thread_MPU6050 (void *argument) {
  
  PWR_ON_MPU6050 ();
 
  while (1) {
    // Insert thread code here...
    
    if(comienzoLectura){
      //read_MPU6050 ();
      txMsg.ejeX = RD_ACCEL_MPU(ACCEL_XOUT_H);
      txMsg.ejeY = RD_ACCEL_MPU(ACCEL_YOUT_H);
      txMsg.ejeZ = RD_ACCEL_MPU(ACCEL_ZOUT_H);
      txMsg.temp = RD_TEMP_MPU(TEMP_OUT_H);
      
      osMessageQueuePut(mid_MsgQueue_MPU, &txMsg, 0U, 0U);
      
      osDelay(1000); //Lectura cada segundo
      
    }else{
      osThreadYield ();
    }
  }
}

//Funcion Callback del I2C
void Callback_I2C (uint32_t event){
  uint32_t mask = ARM_I2C_EVENT_TRANSFER_DONE;
  if(event & mask){
    osThreadFlagsSet(tid_mpu6050, INIT_FLAG); 
  }
}

void PWR_ON_MPU6050 (void){
  
  uint8_t byte[2];
  
  I2Cdrv->Initialize   (Callback_I2C);
  I2Cdrv->PowerControl (ARM_POWER_FULL);
  I2Cdrv->Control      (ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
  I2Cdrv->Control      (ARM_I2C_BUS_CLEAR, 0);
  
  byte[0] = PWR_MGMT_1;
  byte[1] = 0x00; //Info de la primera transferencia (Wake Up)
  
  I2Cdrv->MasterTransmit(ADDR_MPU, byte, 2, false);
  osThreadFlagsWait(INIT_FLAG, osFlagsWaitAll, osWaitForever);
  
  byte[0] = SMPLRT_DIV;
  byte[1] = 0x00;
  
  I2Cdrv->MasterTransmit(ADDR_MPU, byte, 2, false);
  osThreadFlagsWait(INIT_FLAG, osFlagsWaitAll, osWaitForever);
  
  byte[0] = ACCEL_CNFG;
  byte[1] = 0x00;
  
  I2Cdrv->MasterTransmit(ADDR_MPU, byte, 2, true);
  osThreadFlagsWait(INIT_FLAG, osFlagsWaitAll, osWaitForever);
}

static float RD_ACCEL_MPU (uint8_t reg_address){
  
  uint8_t read_H;
  uint8_t read_L;
  float totalAccel;
  
  I2Cdrv->MasterTransmit(ADDR_MPU, &reg_address, 1, false);
  osThreadFlagsWait(INIT_FLAG, osFlagsWaitAll, osWaitForever);
  
  I2Cdrv->MasterReceive(ADDR_MPU, &read_H, 1, true);
  osThreadFlagsWait(INIT_FLAG, osFlagsWaitAll, osWaitForever);
  
  reg_address += 1; //Son posiciones consecutivas
  
  I2Cdrv->MasterTransmit(ADDR_MPU, &reg_address, 1, false);
  osThreadFlagsWait(INIT_FLAG, osFlagsWaitAll, osWaitForever);
  
  I2Cdrv->MasterReceive(ADDR_MPU, &read_L, 1, true);
  osThreadFlagsWait(INIT_FLAG, osFlagsWaitAll, osWaitForever);
  
  totalAccel = (float)(((int16_t)((read_H << 8) | read_L))/(float)16384);
  
  return totalAccel;
}

static float RD_TEMP_MPU (uint8_t reg_address){
  uint8_t read_H;
  uint8_t read_L;
  float totalTemp;
  
  I2Cdrv->MasterTransmit(ADDR_MPU, &reg_address, 1, false);
  osThreadFlagsWait(INIT_FLAG, osFlagsWaitAll, osWaitForever);
  
  I2Cdrv->MasterReceive(ADDR_MPU, &read_H, 1, true);
  osThreadFlagsWait(INIT_FLAG, osFlagsWaitAll, osWaitForever);
  
  reg_address += 1; //Son posiciones consecutivas
  
  I2Cdrv->MasterTransmit(ADDR_MPU, &reg_address, 1, false);
  osThreadFlagsWait(INIT_FLAG, osFlagsWaitAll, osWaitForever);
  
  I2Cdrv->MasterReceive(ADDR_MPU, &read_L, 1, true);
  osThreadFlagsWait(INIT_FLAG, osFlagsWaitAll, osWaitForever);
  
  totalTemp = (float)(((int16_t)(read_H << 8) | read_L)) / (float)340 + (float)36.53;
  
  return totalTemp;
}

void initRead (void){
  comienzoLectura = true;
}

void stopRead (void){
  comienzoLectura = false;
}

