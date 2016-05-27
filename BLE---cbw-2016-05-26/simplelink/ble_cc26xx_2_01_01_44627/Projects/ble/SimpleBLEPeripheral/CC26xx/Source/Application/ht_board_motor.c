
#include <driverlib/timer.h>
#include <ti/sysbios/family/arm/cc26xx/Power.h>
#include <ti/sysbios/family/arm/cc26xx/PowerCC2650.h>
#include <ti/drivers/pin/PINCC26XX.h>

#include "Board.h"
#include "ht_board_led.h"
#include "ht_board_uart.h"
#include "ht_board_motor.h"


//#define DBG_ON

#ifndef DBG_ON
#define pr_err   uart_print
#define pr_info(x...)
#else
#define pr_err   uart_print
#define pr_info  uart_print
#endif

static PIN_State  Motor_Pins;
static PIN_Handle hMotor_Pins = NULL;

static void Board_motorCallback(PIN_Handle hPin, PIN_Id pinId);
static void board_pwm_start(unsigned long freq_Hz, uint8 duration,uint8_t pin_id);

// PIN configuration structure to set all LED pins as output
PIN_Config motorPinsCfg[] =
{
  Board_5V0_EN        | PIN_GPIO_OUTPUT_EN   | PIN_GPIO_LOW    | PIN_PUSHPULL,
  Board_MOTOR_AIN1    | PIN_GPIO_OUTPUT_EN   | PIN_GPIO_LOW    | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  Board_MOTOR_AIN2    | PIN_GPIO_OUTPUT_EN   | PIN_GPIO_LOW    | PIN_PUSHPULL | PIN_DRVSTR_MAX,  
  Board_MOTOR_NSLEEP  | PIN_GPIO_OUTPUT_EN   | PIN_INPUT_DIS   | PIN_PULLDOWN,
  Board_MOTOR_NFAULT  | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN    | PIN_PULLUP,
  PIN_TERMINATE
};

void Board_init_motor(void)
{
  hMotor_Pins = PIN_open(&Motor_Pins, motorPinsCfg);
  PIN_registerIntCb(hMotor_Pins, Board_motorCallback);
  
  PIN_setConfig(hMotor_Pins, PIN_BM_IRQ, Board_MOTOR_NFAULT  | PIN_IRQ_BOTHEDGES);
/*
#ifdef POWER_SAVING
  //Enable wakeup
  PIN_setConfig(hMotor_Pins, PINCC26XX_BM_WAKEUP, Board_MOTOR_NFAULT | (PINCC26XX_WAKEUP_POSEDGE));
#endif 
  */
}
void board_pwm_init()
{
  Power_setDependency(PERIPH_GPT0);
  TimerConfigure(GPT0_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_PWM);
}

void init_motor(void)
{
  if(hMotor_Pins == NULL){
    Board_init_motor();
    board_pwm_init();
  }
}

static void board_pwm_start(unsigned long freq_Hz, uint8 duration,uint8_t pin_id)
{    
  unsigned long PWM_FREQ = freq_Hz;
  unsigned long PWM_DIV_FACTOR = 48000000/PWM_FREQ;
  unsigned long TIMER_LOADSET = (PWM_DIV_FACTOR-1);
  unsigned long TIMER_MATCH;
  
  if(duration > 100)
    duration = 100;
  TIMER_MATCH = (PWM_DIV_FACTOR*(100-duration)/100-1);
  
  TimerLoadSet(GPT0_BASE,TIMER_BOTH,TIMER_LOADSET);  
  TimerDisable(GPT0_BASE, TIMER_BOTH);
  
  switch(pin_id){
  case HAL_MOTOR_AIN1:
    {
      PINCC26XX_setMux(hMotor_Pins, Board_MOTOR_AIN1, IOC_PORT_MCU_PORT_EVENT0);   
      TimerMatchSet(GPT0_BASE,TIMER_A,TIMER_MATCH);
      TimerEnable(GPT0_BASE,TIMER_A);
      break;
    }
  case HAL_MOTOR_AIN2:
    {
      PINCC26XX_setMux(hMotor_Pins, Board_MOTOR_AIN2, IOC_PORT_MCU_PORT_EVENT1);
      TimerMatchSet(GPT0_BASE,TIMER_B,TIMER_MATCH);
      TimerEnable(GPT0_BASE,TIMER_B);
      break;
    }
  default:
    break;   
  }
}
/*
on   : HAL_MOTOR_OFF  ;  HAL_MOTOR_ON 
mode :    //  mode:    000             00              0             00
         //          duration       pwm mode2         select         mode1 

    mode1{ HAL_MOTOR_MODE_COAST ; HAL_MOTOR_MODE_REVERSE ; HAL_MOTOR_MODE_FORWARD ; HAL_MOTOR_MODE_BRAKE } 
    mode2-pwm{  HAL_MOTOR_PWM_FORWARD_FAST   ;  HAL_MOTOR_PWM_FORWARD_SLOW  ;  HAL_MOTOR_PWM_REVERSE_FAST  ;  HAL_MOTOR_PWM_REVERSE_SLOW 
                 (HAL_MOTOR_PWM_XXXX | PWM_DURATION_X) }
                
*/
void open_motor(bool on,uint8_t mode)
{
  uint8_t ain1 = 0;
  uint8_t ain2 = 0;
  uint8_t duration = 0;
  uint8_t mode_pwm;
  
  if(!on){
    PIN_setOutputValue(hMotor_Pins, Board_MOTOR_NSLEEP,BOARD_MOTOR_PIN_LOW);
    PIN_setOutputValue(hMotor_Pins, Board_MOTOR_AIN1,BOARD_MOTOR_PIN_LOW);
    PIN_setOutputValue(hMotor_Pins, Board_MOTOR_AIN2,BOARD_MOTOR_PIN_LOW); 
    PIN_setOutputValue(hMotor_Pins, Board_5V0_EN,BOARD_MOTOR_PIN_LOW);
  }else{
    if(mode & (0x01<<2)){
      ain2 = mode & 0x01;
      ain1 = (mode >> 1)  & 0x01;
      pr_info("mode=%#x ain1=%d ain2=%d\r\n",(mode & 0x03),ain1,ain2);
      PIN_setOutputValue(hMotor_Pins, Board_5V0_EN,BOARD_MOTOR_PIN_HIGHT);
      PIN_setOutputValue(hMotor_Pins, Board_MOTOR_NSLEEP,BOARD_MOTOR_PIN_HIGHT);
      PIN_setOutputValue(hMotor_Pins, Board_MOTOR_AIN1,ain1);
      PIN_setOutputValue(hMotor_Pins, Board_MOTOR_AIN2,ain2); 
    }else{
      mode_pwm = mode & (0x03<<3);
      duration = (((mode & (0x07<<5)) >> 5) -1) *15 +5;
      if(!((mode & (0x07<<5)) >> 5)){
        duration = 50;
      }
      pr_info("D_FAST=%#x D_SLOW=%#x E_FAST=%#x E_SLOW=%#x mode=%#x duration=%d \r\n",HAL_MOTOR_PWM_FORWARD_FAST,HAL_MOTOR_PWM_FORWARD_SLOW,HAL_MOTOR_PWM_REVERSE_FAST,HAL_MOTOR_PWM_REVERSE_SLOW,mode_pwm,duration);
      switch(mode_pwm){
      case HAL_MOTOR_PWM_FORWARD_FAST:
        //board_pwm_stop();
        pr_info(" PWM_FORWARD_FAST \r\n");
        PIN_setOutputValue(hMotor_Pins, Board_5V0_EN,BOARD_MOTOR_PIN_HIGHT);
        PIN_setOutputValue(hMotor_Pins, Board_MOTOR_NSLEEP,BOARD_MOTOR_PIN_HIGHT);
        board_pwm_start(PWM_10K_HZ,duration,HAL_MOTOR_AIN1);
        PIN_setOutputValue(hMotor_Pins, Board_MOTOR_AIN2,BOARD_MOTOR_PIN_LOW);
        PIN_setOutputValue(hMotor_Pins, Board_MOTOR_AIN2,BOARD_MOTOR_PIN_LOW);
        break;
      case HAL_MOTOR_PWM_FORWARD_SLOW:
        pr_info(" PWM_FORWARD_SLOW \r\n");
        //   board_pwm_stop();
        PIN_setOutputValue(hMotor_Pins, Board_5V0_EN,BOARD_MOTOR_PIN_HIGHT);
        PIN_setOutputValue(hMotor_Pins, Board_MOTOR_NSLEEP,BOARD_MOTOR_PIN_HIGHT);
        PIN_setOutputValue(hMotor_Pins, Board_MOTOR_AIN1,BOARD_MOTOR_PIN_HIGHT);    
        board_pwm_start(PWM_10K_HZ,duration,HAL_MOTOR_AIN2);
        break;
      case HAL_MOTOR_PWM_REVERSE_FAST:
        pr_info(" PWM_REVERSE_FAST \r\n");
        //board_pwm_stop();
        PIN_setOutputValue(hMotor_Pins, Board_5V0_EN,BOARD_MOTOR_PIN_HIGHT);
        PIN_setOutputValue(hMotor_Pins, Board_MOTOR_NSLEEP,BOARD_MOTOR_PIN_HIGHT);
        PIN_setOutputValue(hMotor_Pins, Board_MOTOR_AIN1,BOARD_MOTOR_PIN_LOW);
        board_pwm_start(PWM_10K_HZ,duration,HAL_MOTOR_AIN2);
        break;
      case HAL_MOTOR_PWM_REVERSE_SLOW:
        pr_info(" PWM_REVERSE_SLOW \r\n");
        // board_pwm_stop();
        PIN_setOutputValue(hMotor_Pins, Board_5V0_EN,BOARD_MOTOR_PIN_HIGHT);
        PIN_setOutputValue(hMotor_Pins, Board_MOTOR_NSLEEP,BOARD_MOTOR_PIN_HIGHT);
        board_pwm_start(PWM_10K_HZ,duration,HAL_MOTOR_AIN1);
        PIN_setOutputValue(hMotor_Pins, Board_MOTOR_AIN2,BOARD_MOTOR_PIN_HIGHT);        
        break;
        //case PWM_FORWARD:       
        //break;
      default:
        break;
      }
    }
  }
}

static void Board_motorCallback(PIN_Handle hPin, PIN_Id pinId)
{
  
  if ( PIN_getInputValue(Board_MOTOR_NFAULT) == 0 ){
    open_motor(HAL_MOTOR_OFF,HAL_MOTOR_MODE_COAST);
    pr_info("NFAULT err\n");
  } 
  
}

