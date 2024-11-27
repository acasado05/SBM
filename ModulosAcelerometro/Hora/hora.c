#include "hora.h"
 
/*----------------------------------------------------------------------------
 *                             MODULO HORA
 *           Módulo encargado de la gestión del reloj del sistema
 *---------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 *                                HILO
 *--------------------------------------------------------------------------*/
osTimerId_t segundero;                        // identificador del timer encargado de gestionar el tiempo transcurrido
osStatus_t status_segundero;
static uint32_t exec;
 
osThreadId_t tid_hora;                        // identificador del hilo encargado de gestionar la hora
 
void ThClock (void *argument);                   // funcion principal del hilo encargado  de gestionar la hora

uint8_t segundos;
uint8_t minutos;
uint8_t horas;

void Segundero_Callback(void const *arg);
 
int Init_Hora (void) {
 
  tid_hora = osThreadNew(ThClock, NULL, NULL);
  if (tid_hora == NULL) {
    return(-1);
  }
  
  segundero = osTimerNew((osTimerFunc_t)&Segundero_Callback, osTimerPeriodic, &exec, NULL);
  if(segundero != NULL){	// Si el timer se ha creado correctamente
  // Se arranca el timer con un periodo de 1 segundo
  // status = osTimerStart(segundero, 1000);
  if(status_segundero != osOK){
    return -1;
  }
}
  return(0);
}
 
void ThClock (void *argument) {
  
  osTimerStart(segundero, 1000);
  
  while (1) {
    // Insert thread code here...
    osThreadYield();                            // suspend thread
  }
}

void Segundero_Callback(void const *arg){ // Callback del timer que se va a ejecutar cada segundo
  
  segundos++;
  
  if(segundos == 60){
    segundos = 0;
    minutos++;
  }
  
  if(minutos == 60){
    minutos = 0;
    horas++;
  }
  
  if(horas == 24){
    horas = 0;
  }
}

