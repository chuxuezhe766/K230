
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "Board_LED.h"                  // ::Board Support:LED

/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_LED': LED thread
 *---------------------------------------------------------------------------*/
 
void Thread_LED (void const *argument);                 // thread function
osThreadId tid_Thread_LED;                              // thread id
osThreadDef(Thread_LED, osPriorityNormal, 1, 0);        // thread object

int Init_Thread_LED (void) {

  tid_Thread_LED = osThreadCreate(osThread(Thread_LED), NULL);
  if (!tid_Thread_LED) return(-1);
  
  return(0);
}

void Thread_LED (void const *argument) {
  uint32_t led_cnt = LED_GetCount();
  uint32_t led_num = 0;

  while (1) {
    osSignalWait(1U, osWaitForever);    // Wait for signal
    LED_On(led_num);                    // Turn specified LED on
    osSignalWait(1U, osWaitForever);    // Wait for signal
    LED_Off(led_num);                   // Turn specified LED off

    led_num++;                          // Change LED number
    if (led_num >= led_cnt) {
      led_num = 0;                      // Restart with first LED
    }
  }
}
