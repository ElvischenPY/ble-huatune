
/*********************************************************************
* INCLUDES
*/
#include <string.h>
#include <stdio.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>

#include "osal_snv.h"
#include "ht_board_led.h"
#include "Board.h"
#include "ht_board_uart.h"


#define DBG_ON

#ifndef DBG_ON
#define pr_err   uart_print
#define pr_info(x...)
#else
#define pr_err   uart_print
#define pr_info  uart_print
#endif

static PIN_State  ledPins;
static PIN_Handle hledPins = NULL;

// PIN configuration structure to set all LED pins as output
PIN_Config ledPinsCfg[] =
{
  Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  Board_LED2 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  PIN_TERMINATE
};

static void Board_initLeds(void)
{
  hledPins = PIN_open(&ledPins, ledPinsCfg);
}
 
void HalLedInit( void )
{
  if(NULL == hledPins)
  {
    Board_initLeds();
  }
}
/*

  led : HAL_LED_GREEN (HAL_LED_1) ; HAL_LED_RED(HAL_LED_2)  HAL_LED_ALL
  mode : HAL_LED_MODE_OFF ;  HAL_LED_MODE_ON  ;  HAL_LED_MODE_FLASH (HAL_LED_MODE_FLASH|HAL_LED_FLASH_X); HAL_LED_MODE_TOGGLE  

*/ 
uint8 HalLedSet( uint8 led, uint8 mode )
{
  uint8 i = 0;
  uint8 pin[2] = {Board_LED1, Board_LED2};
  
  if(NULL == hledPins)
  {
    Board_initLeds();
  }
  uint8 flash_count = (mode & 0xf0) >>4;
  mode = mode & 0x0f;
  pr_info("led=%#x mode=%#x flash_count=%#x\n",led,mode,flash_count);
  for(i= 0; i<=1; i++){      
    if(led & (0x1<<i)){
      switch(mode){
      case HAL_LED_MODE_OFF:
        PIN_setOutputValue(hledPins, pin[i], 0);
        break;
      case HAL_LED_MODE_ON:
        PIN_setOutputValue(hledPins, pin[i], 1);
        break;
      case HAL_LED_MODE_FLASH:
        PIN_setOutputValue(hledPins, Board_LED1, 0);
        PIN_setOutputValue(hledPins, Board_LED2, 0);
        if(flash_count != 0){
          while(flash_count--){
            //pr_info("flash_count=%#x\n",flash_count);
            PIN_setOutputValue(hledPins, pin[i],  !PIN_getOutputValue( pin[i]));
            Task_sleep(500*1000/Clock_tickPeriod);
            PIN_setOutputValue(hledPins, pin[i],  !PIN_getOutputValue( pin[i]));
            Task_sleep(500*1000/Clock_tickPeriod);
          }
        }else{
          PIN_setOutputValue(hledPins, pin[i],  1);
          Task_sleep(500*1000/Clock_tickPeriod);
          PIN_setOutputValue(hledPins, pin[i], 0);
        }
        break;
        
      case HAL_LED_MODE_TOGGLE:
        PIN_setOutputValue(hledPins, pin[i],  !PIN_getOutputValue( pin[i]));                
        break;
      }
    }
  }  
  return 0;
}






