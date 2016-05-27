
/*********************************************************************
* INCLUDES
*/
#include <ti/drivers/UART.h>
#include "ht_board_uart.h"
#include "Board.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static UART_Handle SbpUartHandle;
static UART_Params SbpUartParams;  


static void HT_UART_Write (const char *str, uint8 len)
{  
  UART_write(SbpUartHandle, str, len);
}

void Uart_Init(void)
{
  UART_Params_init(&SbpUartParams);
  SbpUartHandle = UART_open(CC2650_UART0, &SbpUartParams);
  
}

void uart_print(const char *fmt,...)
{
  va_list ap;
  char string[200];
  
  va_start(ap,fmt); 
  vsprintf(string,fmt,ap);
  HT_UART_Write(string,strlen(string));
  va_end(ap); 
  
}




