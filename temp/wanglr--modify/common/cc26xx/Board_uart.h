#ifndef BOARD_UART_H
#define BOARD_UART_H

#ifdef __cplusplus
extern "C" {
#endif


#define uint8    unsigned char

void Uart_Init();
extern void print_msg(const char *fmt, ...);

extern void UART_WriteTransport (uint8 *str, uint8 len);


#ifdef __cplusplus
}
#endif

#endif
