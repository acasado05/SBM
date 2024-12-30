#ifndef __MPU6050_H
#define __MPU6050_H

/* Liber?as */
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"
#include "Driver_I2C.h"
#include <stdbool.h>

/* Macros */
#define INIT_FLAG 0x01 //Flag que da comienzo al reloj

#define ADDR_MPU      0x68
#define PWR_MGMT_1    0x6B
#define SMPLRT_DIV    0x19
#define ACCEL_CNFG    0x1C

#define ACCEL_XOUT_H  0x3B
#define ACCEL_XOUT_L  0x3C

#define ACCEL_YOUT_H  0x3D
#define ACCEL_YOUT_L  0x3E

#define ACCEL_ZOUT_H  0x3F
#define ACCEL_ZOUT_L  0x40

#define TEMP_OUT_H    0x41
#define TEMP_OUT_L    0x42


#define MSGQUEUE_OBJECTS 10

typedef struct{
  float ejeX;
  float ejeY;
  float ejeZ;
  float temp;
}MSGQUEUE_MPU6050_OBJ_t;

extern osThreadId_t tid_mpu6050;                        // thread id
extern osMessageQueueId_t mid_MsgQueue_MPU;           // Identificador de la cola

int Init_MPU6050 (void);

extern osMessageQueueId_t idQueueMPU (void);

#endif /* __MPU6050_H */











