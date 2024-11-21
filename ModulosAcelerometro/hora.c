#include "ThClock.h"
 
/*----------------------------------------------------------------------------
 *                             MODULO HORA
 *           Módulo encargado de la gestión del reloj del sistema
 *---------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 *                                TIMER
 *--------------------------------------------------------------------------*/

osTimerId_t segundero;                        // identificador del timer encargado de gestionar el tiempo transcurrido
static uint32_t exec;

extern uint8_t segundos;
extern uint8_t minutos;
extern uint8_t horas;

static void Segundero_Callback(void const *arg){	// Callback del timer que se va a ejecutar cada segundo
	
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

int Init_Segundero(void){
	
	osStatus_t status;
	
	// Creacion de un timer
	exec = 2U;
	segundero = osTimerNew((osTimerFunc_t)&Segundero_Callback, osTimerPeriodic, &exec, NULL);
	if(segundero != NULL){	// Si el timer se ha creado correctamente
		// Se arranca el timer con un periodo de 1 segundo
		// status = osTimerStart(segundero, 1000);
		if(status != osOK){
			
			return -1;
		}
		
	}
	
	return NULL;
}

/*--------------------------------------------------------------------------
 *                                HILO
 *--------------------------------------------------------------------------*/
 
osThreadId_t tid_ThClock;                        // identificador del hilo encargado de gestionar la hora
 
void ThClock (void *argument);                   // funcion principal del hilo encargado  de gestionar la hora
 
int Init_ThClock (void) {
 
  tid_ThClock = osThreadNew(ThClock, NULL, NULL);
  if (tid_ThClock == NULL) {
    return(-1);
  }
	
	Init_Segundero();
  return(0);
}
 
void ThClock (void *argument) {
	
	osTimerStart(segundero, 1000);
	
  while (1) {
    ; // Insert thread code here...
    osThreadYield();                            // suspend thread
  }
}
