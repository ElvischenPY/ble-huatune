#ifndef HT_BOARD_UART_H
#define HT_BOARD_UART_H

#ifdef __cplusplus
extern "C" {
#endif
  
  
#define uint8    unsigned char
  
  extern void Uart_Init();
  static void HT_UART_Write (const char *str, uint8 len);
  extern void uart_print(const char *fmt,...);
  
#ifdef __cplusplus
}
#endif

#endif
