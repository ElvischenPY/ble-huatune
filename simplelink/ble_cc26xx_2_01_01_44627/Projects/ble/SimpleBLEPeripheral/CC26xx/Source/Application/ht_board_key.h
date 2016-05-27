
#ifndef HT_BOARD_KEY_H
#define HT_BOARD_KEY_H

#ifdef __cplusplus
extern "C" {
#endif
  
#define KEY_OPEN            0x0001
  
  
  // Debounce timeout in milliseconds
#define KEY_DEBOUNCE_TIMEOUT  200
  
  typedef void (*keysPressedCB_t)(uint8 keysPressed);
  extern  void ht_Board_initKeys(keysPressedCB_t appKeyCB);
  
  
#ifdef __cplusplus
}
#endif

#endif /* HT_BOARD_KEY_H */
