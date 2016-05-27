
#ifndef HT_BOARD_BATTERY_H
#define HT_BOARD_BATTERY_H

#ifdef __cplusplus
extern "C" {
#endif
#define uint8    unsigned char
  
#define delay_ms(i) ( CPUdelay(12000*(i)) )
#define delay_us(i) ( CPUdelay(8*(i)) )

#define PIN_STATUS_LOW    0  
#define PIN_STATUS_HIGHT  1
  
//1.5A
#define  BATTERY_CUTOFF_CURRENT_20  0x01    
#define  BATTERY_CUTOFF_CURRENT_30  0x02
#define  BATTERY_CUTOFF_CURRENT_40  0x03
#define  BATTERY_CUTOFF_CURRENT_50  0x04
#define  BATTERY_CUTOFF_CURRENT_60  0x05
#define  BATTERY_CUTOFF_CURRENT_70  0x06
#define  BATTERY_CUTOFF_CURRENT_80  0x07
#define  BATTERY_CUTOFF_CURRENT_90  0x08
  
#define READ_ADC_LENGTH 7  
  
  void Board_initBattery();
  static void  Board_batteryCallback(PIN_Handle hPin, PIN_Id pinId);
  static uint32_t AdcOneShotRead(uint8_t auxIo);
  static uint8_t BinSearch(const uint32_t sSource[], uint8_t array_size, uint32_t key);
  static uint32_t ADC_to_Value(uint32_t adcValue);
  static void arry_rank(uint32_t arry[],uint8_t length);
  extern void Battery_Init( void );
  extern void set_cutoff_current(uint8 count);
  extern uint32_t getBattValue(uint8_t auxIo);
  
#ifdef __cplusplus
}
#endif

#endif /* HT_BOARD_BATTERY_H */
