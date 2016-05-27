
/*********************************************************************
 * INCLUDES
 */
#include <string.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>

#include "hci_tl.h"
#include "gatt.h"
#include "gapgattserver.h"
#include "gattservapp.h"
#include "devinfoservice.h"
#include "simpleGATTprofile.h"

#if defined(SENSORTAG_HW)
#include "bsp_spi.h"
#endif // SENSORTAG_HW

#if defined(FEATURE_OAD) || defined(IMAGE_INVALIDATE)
#include "oad_target.h"
#include "oad.h"
#endif //FEATURE_OAD || IMAGE_INVALIDATE

#include "peripheral.h"
#include "gapbondmgr.h"

#include "osal_snv.h"
#include "ICallBleAPIMSG.h"

#include <driverlib/ioc.h>
#include <driverlib/gpio.h>
#include <driverlib/timer.h>


#include "Board.h"
#include "ht_board_pwm.h"
#include "ht_board_led.h"
#include "ht_board_uart.h"

#include <ti/sysbios/family/arm/cc26xx/Power.h>
#include <ti/sysbios/family/arm/cc26xx/PowerCC2650.h>
#include <driverlib/timer.h>
#include <ti/drivers/pin/PINCC26XX.h>


#define DBG_ON

#ifndef DBG_ON
#define pr_err   uart_print
#define pr_info(x...)
#else
#define pr_err   uart_print
#define pr_info  uart_print
#endif

#ifdef PWM_EXAMPLE
// 系统时钟为48M
//#define PWM_FREQ    2400   //pwm频率,  2.4k比较适合无源蜂鸣器，声音响亮，用一个透明胶封住小圆孔，更响亮--amomcu--2015.08.15
//#define PWM_DIV_FACTOR (48000000/PWM_FREQ) 
//#define TIMER_LOADSET (PWM_DIV_FACTOR-1)
//#define TIMER_MATCH (PWM_DIV_FACTOR/2-1)
#endif //PWM_EXAMPLE


//#define PWM0_IOID           IOID_17
//#define PWM1_IOID           IOID_16
 // Board_MOTOR_AIN1  | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_INPUT_DIS | PIN_DRVSTR_MAX,

PIN_Config timerPinTable[] = {
//  PWM0_IOID | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_INPUT_DIS | PIN_DRVSTR_MAX, //PIN_INV_INOUT, 
//  PWM1_IOID | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_INPUT_DIS | PIN_DRVSTR_MAX, //PIN_INV_INOUT,
  Board_MOTOR_AIN1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  Board_MOTOR_AIN2 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  PIN_TERMINATE 
};

PIN_Handle hPin;
PIN_State pinState;
//static uint8_t timer_a_enabled = 0;
//static uint8_t PWM_enabled = 0;

void board_pwm_init(uint8_t pin)
{
  
  //RTOS: Enable peripheral domain and clocks for timer
  Power_setDependency(PERIPH_GPT0);
  //RTOS: Disallow standby while timer is running
  //Power_setConstraint  (Power_SB_DISALLOW);-----------------------------------
  //RTOS: Open and assign pins through PIN driver
  hPin = PIN_open(&pinState, timerPinTable);
  // RTOS: Route pin to timer0A module
  // IOC Port events 0-7 are mapped to GPT timers 0A,0B,1A,1B...3B
 /* 
  if(pin == HAL_MOTOR_AIN1){
    PINCC26XX_setMux(hPin, PWM0_IOID, IOC_PORT_MCU_PORT_EVENT0);
  }else if(pin == HAL_MOTOR_AIN2){
    PINCC26XX_setMux(hPin, PWM1_IOID, IOC_PORT_MCU_PORT_EVENT0);
  }  
 */
 // PINCC26XX_setMux(hPin, PWM0_IOID, IOC_PORT_MCU_PORT_EVENT0);
  
  // Timer A in PWM mode, don't care about Timer B but ASSERTS in driverLib requires it
  // to be set to something
  //TimerConfigure(GPT0_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_CAP_COUNT);
  TimerConfigure(GPT0_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_PWM);
  
  // For debug: Stall Timer A when halting in debug mode / single stepping
  //TimerStallControl(GPT0_BASE, TIMER_A, true);
//  TimerLoadSet(GPT0_BASE,
//               TIMER_A,
//               TIMER_LOADSET);

//  TimerMatchSet(GPT0_BASE,
//                TIMER_A,
//                TIMER_MATCH);


//  TimerEventControl(GPT0_BASE, TIMER_A, TIMER_EVENT_NEG_EDGE);

  //TimerEnable(GPT0_BASE,TIMER_A); 
  
}


//pwm频率,  2.4k比较适合无源蜂鸣器，声音响亮，用一个透明胶封住小圆孔，更响亮--amomcu--2015.08.15
//占空比 duration   取值  1~100, 百分比
//pin_id   HAL_MOTOR_AIN1    HAL_MOTOR_AIN2 
void board_pwm_start(unsigned long freq_Hz, uint8 duration,uint8_t pin_id)
{    
  unsigned long PWM_FREQ = freq_Hz;
  unsigned long PWM_DIV_FACTOR = 48000000/PWM_FREQ;
  unsigned long TIMER_LOADSET = (PWM_DIV_FACTOR-1);
  unsigned long TIMER_MATCH;
  
  if(duration > 100)
    duration = 100;
  //PWM_enabled = true;
  TIMER_MATCH = (PWM_DIV_FACTOR*(100-duration)/100-1);
  
  TimerLoadSet(GPT0_BASE,TIMER_BOTH,TIMER_LOADSET);  
  //Power_setConstraint(Power_SB_DISALLOW);
  
  TimerDisable(GPT0_BASE, TIMER_BOTH);

  switch(pin_id){
    case HAL_MOTOR_AIN1:
      {
        HalLedSet(HAL_LED_1, HAL_LED_MODE_FLASH |(3<<4));
        PINCC26XX_setMux(hPin, Board_MOTOR_AIN1, IOC_PORT_MCU_PORT_EVENT0);   
        TimerMatchSet(GPT0_BASE,TIMER_A,TIMER_MATCH);
        //TimerEventControl(GPT0_BASE, TIMER_A, TIMER_EVENT_NEG_EDGE);
        TimerEnable(GPT0_BASE,TIMER_A);
        break;
      }
  case HAL_MOTOR_AIN2:
    {
      HalLedSet(HAL_LED_2, HAL_LED_MODE_FLASH |(3<<4));
      PINCC26XX_setMux(hPin, Board_MOTOR_AIN2, IOC_PORT_MCU_PORT_EVENT1);
      TimerMatchSet(GPT0_BASE,TIMER_B,TIMER_MATCH);
      //TimerEventControl(GPT0_BASE, TIMER_B, TIMER_EVENT_NEG_EDGE);
      TimerEnable(GPT0_BASE,TIMER_B);
      break;
    }
   default:
       break;
    
  }
}

void board_pwm_stop()
{/*
    PIN_setOutputValue(hPin, Board_MOTOR_AIN1,0);
    PIN_setOutputValue(hPin, Board_MOTOR_AIN2,0);
    TimerDisable(GPT0_BASE,TIMER_BOTH);
    Power_setConstraint(Power_STANDBY);
  */
}









