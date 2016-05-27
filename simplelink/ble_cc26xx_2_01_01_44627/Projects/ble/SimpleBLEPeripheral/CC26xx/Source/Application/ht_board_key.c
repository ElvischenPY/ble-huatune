
/*********************************************************************
* INCLUDES
*/
#include <stdbool.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>

#include <ti/drivers/pin/PINCC26XX.h>

#include <string.h>

#ifdef USE_ICALL
#include <ICall.h>
#endif

#include <inc/hw_ints.h>
#include "bcomdef.h"

#include "util.h"
#include "Board.h"
#include "ht_board_key.h"
#include "ht_board_uart.h"
#include <ti/drivers/I2C.h>

static void Board_keyChangeHandler(UArg a0);
static void Board_keyCallback(PIN_Handle hPin, PIN_Id pinId);

// Value of keys Pressed
static uint8_t keysPressed;

// Key debounce clock
static Clock_Struct keyChangeClock;

// Pointer to application callback
keysPressedCB_t appKeyChangeHandler = NULL;

// Memory for the GPIO module to construct a Hwi
Hwi_Struct callbackHwiKeys;

// PIN configuration structure to set all KEY pins as inputs with pullups enabled
PIN_Config keyPinsCfg[] =
{   
  Board_KEY_OPEN    | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  |  PIN_PULLUP,
  PIN_TERMINATE
};

PIN_State  keyPins;
PIN_Handle hKeyPins;

/*********************************************************************
* PUBLIC FUNCTIONS
*/
/*********************************************************************
* @fn      Board_initKeys
*
* @brief   Enable interrupts for keys on GPIOs.
*
* @param   appKeyCB - application key pressed callback
*
* @return  none
*/
void ht_Board_initKeys(keysPressedCB_t appKeyCB)
{
  // Initialize KEY pins. Enable int after callback registered
  hKeyPins = PIN_open(&keyPins, keyPinsCfg);
  PIN_registerIntCb(hKeyPins, Board_keyCallback);
  
  
  //修改成双沿中断触发  
  PIN_setConfig(hKeyPins, PIN_BM_IRQ, Board_KEY_OPEN  | PIN_IRQ_BOTHEDGES); 
#ifdef POWER_SAVING
  //Enable wakeup
  PIN_setConfig(hKeyPins, PINCC26XX_BM_WAKEUP, Board_KEY_OPEN | (PINCC26XX_WAKEUP_POSEDGE));
#endif
  
  // Setup keycallback for keys
  Util_constructClock(&keyChangeClock, Board_keyChangeHandler,
                      KEY_DEBOUNCE_TIMEOUT, 0, false, 0);
  
  // Set the application callback
  appKeyChangeHandler = appKeyCB;
}

/*********************************************************************
* @fn      Board_keyCallback
*
* @brief   Interrupt handler for Keys
*
* @param   none
*
* @return  none
*/
static void Board_keyCallback(PIN_Handle hPin, PIN_Id pinId)
{
  keysPressed = 0;
  
  if ( PIN_getInputValue(Board_KEY_OPEN) == 0 )
  {
    keysPressed |= KEY_OPEN;
  }
  
  Util_startClock(&keyChangeClock);
}

/*********************************************************************
* @fn      Board_keyChangeHandler
*
* @brief   Handler for key change
*
* @param   UArg a0 - ignored
*
* @return  none
*/
static void Board_keyChangeHandler(UArg a0)
{
  if (appKeyChangeHandler != NULL)
  {
    // Notify the application
    uint8_t keysPressed2 = 0;
    if(PIN_getInputValue(Board_KEY_OPEN) == 0 ){
      keysPressed2 |= KEY_OPEN;
    }
    if(keysPressed != keysPressed2){
      keysPressed = 0;
    }
    (*appKeyChangeHandler)(keysPressed);
  }
}
/*********************************************************************
*********************************************************************/
