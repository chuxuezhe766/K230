
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "Board_ADC.h"                  // ::Board Support:A/D Converter

extern volatile uint32_t delay_val;

/*----------------------------------------------------------------------------
 *      Thread 2 'Thread_ADC': A/D Converter thread
 *---------------------------------------------------------------------------*/
 
void Thread_ADC (void const *argument);                 // thread function
osThreadId tid_Thread_ADC;                              // thread id
osThreadDef(Thread_ADC, osPriorityNormal, 1, 0);        // thread object

int Init_Thread_ADC (void) {

  tid_Thread_ADC = osThreadCreate(osThread(Thread_ADC), NULL);
  if (!tid_Thread_ADC) return(-1);
  
  return(0);
}

void Thread_ADC (void const *argument) {

  while (1) {
    ADC_StartConversion();                              // start ADC conversion
    osDelay(20);                                        // wait 20ms

    if (ADC_ConversionDone() == 0) {
      delay_val = ((uint32_t)ADC_GetValue() >> 4) + 10U;// scale delay value
    } 
  }
}
