
#ifndef HT_BOARD_LED_H
#define HT_BOARD_LED_H

#ifdef __cplusplus
extern "C" {
#endif
#define uint8    unsigned char
/* LEDS - The LED number is the same as the bit position */
#define HAL_LED_GREEN     0x01 
#define HAL_LED_RED     0x02
#define HAL_LED_1     0x01 
#define HAL_LED_2     0x02
#define HAL_LED_ALL   (HAL_LED_1 | HAL_LED_2)

/* Modes */
#define HAL_LED_MODE_OFF     0x00
#define HAL_LED_MODE_ON      0x01
  //#define HAL_LED_MODE_BLINK   0x02
#define HAL_LED_MODE_FLASH   0x04
#define HAL_LED_MODE_TOGGLE  0x08
  
  
#define HAL_LED_FLASH_1    (0x1<<4) 
#define HAL_LED_FLASH_2    (0x2<<4)
#define HAL_LED_FLASH_3    (0x3<<4)
#define HAL_LED_FLASH_4    (0x4<<4)
#define HAL_LED_FLASH_5    (0x5<<4)
#define HAL_LED_FLASH_6    (0x6<<4)
#define HAL_LED_FLASH_7    (0x7<<4)
#define HAL_LED_FLASH_8    (0x8<<4)
#define HAL_LED_FLASH_9    (0x9<<4)
#define HAL_LED_FLASH_10   (0xa<<4)
#define HAL_LED_FLASH_11   (0xb<<4)
#define HAL_LED_FLASH_12   (0xc<<4)
#define HAL_LED_FLASH_13   (0xd<<4)
#define HAL_LED_FLASH_14   (0xe<<4)
#define HAL_LED_FLASH_15   (0xf<<4)
  
  
  
  static void Board_initLeds(void);
  extern void HalLedInit( void );
  extern uint8 HalLedSet( uint8 led, uint8 mode);
  
  
#ifdef __cplusplus
}
#endif

#endif /* HT_BOARD_LED_H */
