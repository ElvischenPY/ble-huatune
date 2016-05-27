/*********************************************************************
* INCLUDES
*/
#include <ti/drivers/pin/PINCC26XX.h>
#include "ht_board_led.h"
#include "Board.h"
#include "ht_board_uart.h"
#include "ht_board_battery.h"
#include <driverlib/aux_adc.h>  
#include <driverlib/aux_wuc.h>

//#define DBG_ON

#ifndef DBG_ON
#define pr_err   uart_print
#define pr_info(x...)
#else
#define pr_err   uart_print
#define pr_info  uart_print
#endif

const static uint32_t refer_Value[]={
  2403,2449,2493,2535,2575,
  2612,2651,2687,2721,2756,
  2786,2818,2846,2875,2903,
  2933,2957,2982,3007,3033,
};
const static uint32_t refer_ADC_Value[]={	
  2045,2090,2135,2181,2226,
  2273,2317,2362,2408,2452,
  2499,2543,2590,2635,2680,
  2723,2770,2815,2860,2903,
};

static PIN_State  BatteryPins;
static PIN_Handle hBatteryPins = NULL;

// PIN configuration structure to set all LED pins as output
PIN_Config batteryPinsCfg[] =
{
  Board_BATTERY_STAT  | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  |  PIN_PULLUP,
  Board_BATTERY_CTRL  | PIN_GPIO_OUTPUT_EN | PIN_INPUT_DIS |  PIN_PULLDOWN,
  Board_BATTERY_ENN   | PIN_GPIO_OUTPUT_EN | PIN_INPUT_DIS |  PIN_PULLDOWN,
  //    Board_BATTERY_TEMP
  PIN_TERMINATE
};

void Board_initBattery()
{
  hBatteryPins = PIN_open(&BatteryPins, batteryPinsCfg);
}

void Battery_Init( void )
{
  if(NULL == hBatteryPins)
  {
    Board_initBattery();
    PIN_registerIntCb(hBatteryPins,Board_batteryCallback);
    PIN_setConfig(hBatteryPins,PIN_BM_IRQ,Board_BATTERY_STAT | PIN_IRQ_BOTHEDGES );
    /*
#ifdef POWER_SAVING
    PIN_setConfig(hKeyPins, PINCC26XX_BM_WAKEUP, Board_KEY_OPEN | (PINCC26XX_WAKEUP_POSEDGE));
#endif
    */  
  }
}

static uint32_t AdcOneShotRead(uint8_t auxIo)
{
  uint32_t turnedOnClocks = 0;
  //config clock
  turnedOnClocks|=AUXWUCClockStatus(AUX_WUC_ADC_CLOCK)?0:AUX_WUC_ADC_CLOCK;
  turnedOnClocks|=AUXWUCClockStatus(AUX_WUC_ADI_CLOCK)?0:AUX_WUC_ADI_CLOCK;  
  turnedOnClocks|=AUXWUCClockStatus(AUX_WUC_SOC_CLOCK)?0:AUX_WUC_SOC_CLOCK; 
  // Enable clocks and wait for ready  
  AUXWUCClockEnable(turnedOnClocks); 
  while(AUX_WUC_CLOCK_OFF == AUXWUCClockStatus(turnedOnClocks));
  /////// Seclect auxIO  /////////////  
  AUXADCSelectInput(auxIo);  
  ////////// Enable ///////////  
  AUXADCEnableSync(AUXADC_REF_FIXED, AUXADC_SAMPLE_TIME_2P7_US, AUXADC_TRIGGER_MANUAL);  
  delay_ms(10);  
  //Scaling disable  
  AUXADCDisableInputScaling();     
  AUXADCGenManualTrigger();       // Trigger sample
  uint32_t adcValue = AUXADCReadFifo();     
  AUXADCDisable();//Power_Saving  
  return adcValue;
}
static uint8_t BinSearch(const uint32_t sSource[], uint8_t array_size, uint32_t key)  
{
  uint8_t low = 0, high = array_size - 1, mid;
  // pr_info(" array_size=%d  key=%d \n\n\n",array_size,key);
  while(low <= high){
    mid = (low + high) / 2; 
    //pr_info(" mid=%d  sSource[mid]=%lu \n\n\n",mid,sSource[mid]);
    if((sSource[mid] <= key) && (sSource[mid+1] >= key))            
      return mid; 
    if(sSource[mid+1] < key)     
      low = mid + 1;            
    else  
      high = mid - 1;
  }     
  return -1;    
}
static uint32_t ADC_to_Value(uint32_t adcValue)
{
  uint8_t index = 0;
  uint32_t referValue = 0;
  uint32_t Value = 0;
  uint8_t len = sizeof(refer_ADC_Value)/sizeof(uint32_t);
  
  if(adcValue <= refer_ADC_Value[0]){
    referValue = refer_Value[0];
  }else if(adcValue >= refer_ADC_Value[len-1]){
    referValue = refer_Value[len-1];
  }else{
    index = BinSearch(refer_ADC_Value,len,adcValue);
    referValue = ((2 * (refer_Value[index+1] - refer_Value[index]) * (adcValue - refer_ADC_Value[index])) / (refer_ADC_Value[index+1] - refer_ADC_Value[index])+1)/2 + refer_Value[index];
  }
  Value = ((2*adcValue*referValue/0xFFF)+1)/2;
  //pr_info("index=%d,refer_ADC_Value=%d,refer_ADC_Value_1=%d\n",index,refer_ADC_Value[index],refer_ADC_Value[index+1]);
  //pr_info("index=%d,refer_Value=%d,refer_Value_1=%d\n",index,refer_Value[index],refer_Value[index+1]);
  //pr_info("adc=%d,index=%d,referValue=%d,Value=%d\n",adcValue,index,referValue,Value);
  return Value;	
}
static void arry_rank(uint32_t arry[],uint8_t length)
{
  uint8_t i, j;
  for(i=1;i<length-1;i++){
    for(j=0;j<length-i;j++){
      if(arry[j]>arry[j+1]){
        //arry[j] = arry[j+1] + arry[j] - (arry[j+1] = arry[j]);
        arry[j]   = arry[j] + arry[j+1];
        arry[j+1] = arry[j] - arry[j+1];
        arry[j]   = arry[j] - arry[j+1];
      }
    }
  }
}
uint32_t getBattValue(uint8_t auxIo)
{
  uint32_t adcValue = 0;
  uint32_t battValue = 0;
  uint32_t adcValue_arry[READ_ADC_LENGTH];
  uint8_t i;
  
  for(i=0;i<READ_ADC_LENGTH;i++){
    adcValue_arry[i]=AdcOneShotRead(auxIo);
  }
  arry_rank(adcValue_arry,READ_ADC_LENGTH);
  /*
  for(i=0;i<READ_ADC_LENGTH;i++){
  pr_info("%lu \t",adcValue_arry[i]);
}*/
  if(READ_ADC_LENGTH%2){
    adcValue = (2*(adcValue_arry[READ_ADC_LENGTH/2-1]+adcValue_arry[READ_ADC_LENGTH/2]+adcValue_arry[READ_ADC_LENGTH/2+1])/3 + 1)/2;
  }else{
    adcValue = (adcValue_arry[READ_ADC_LENGTH/2-1] + adcValue_arry[READ_ADC_LENGTH/2] + 1)/2;
  }
  battValue = ADC_to_Value(adcValue);
  pr_info("adc= %d value= %d mV\n",adcValue,battValue*2); 
  return battValue*2;
}
void set_cutoff_current(uint8 count)
{
  pr_info("count=%d\n\n",count);
  if(count >0 && count <8){
    PIN_setOutputValue(hBatteryPins, Board_BATTERY_CTRL,0);
    delay_ms(1);
    while(count--){
      PIN_setOutputValue(hBatteryPins, Board_BATTERY_CTRL, 1);
      delay_us(2);
      PIN_setOutputValue(hBatteryPins, Board_BATTERY_CTRL, 0);
      delay_us(2);
    }
  }
} 
static void  Board_batteryCallback(PIN_Handle hPin, PIN_Id pinId)
{
  if(PIN_getInputValue(Board_BATTERY_STAT) == PIN_STATUS_LOW){
    HalLedSet(HAL_LED_ALL,HAL_LED_MODE_OFF);
    HalLedSet(HAL_LED_GREEN,HAL_LED_MODE_ON);
  }else{
    HalLedSet(HAL_LED_ALL,HAL_LED_MODE_OFF);
  }
}
