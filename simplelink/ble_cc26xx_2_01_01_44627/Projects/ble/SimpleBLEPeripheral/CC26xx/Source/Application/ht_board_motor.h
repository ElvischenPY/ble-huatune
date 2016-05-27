#ifndef HT_BOARD_MOTOR_H
#define HT_BOARD_MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif
  
#define BOARD_MOTOR_PIN_LOW       0
#define BOARD_MOTOR_PIN_HIGHT     1
  
#define HAL_MOTOR_OFF     0x00
#define HAL_MOTOR_ON      0x01
  
#define HAL_MOTOR_AIN1 0x01
#define HAL_MOTOR_AIN2 0x02  
  
#define PWM_1K_HZ 1000 
#define PWM_10K_HZ 10*1000 
#define PWM_100K_HZ 100*1000   
  
  
  //  mode   000         00           0             00
  //        duration   pwm mode2     select         mode1 
  
  //mode 1
#define HAL_MOTOR_MODE_COAST           (0x00  |(0x01<<2))             //Coast / fast decay
#define HAL_MOTOR_MODE_REVERSE         (0x01  |(0x01<<2))       //Reverse //·­×ª
#define HAL_MOTOR_MODE_FORWARD         (0x02  |(0x01<<2))       //Forward//Õý×ª
#define HAL_MOTOR_MODE_BRAKE           (0x03  |(0x01<<2))     //Brake / slow decay
  
  //mode 2 pwm
#define HAL_MOTOR_PWM_FORWARD_FAST     (0x00<<3)
#define HAL_MOTOR_PWM_FORWARD_SLOW     (0x01<<3)
#define HAL_MOTOR_PWM_REVERSE_FAST     (0x02<<3)
#define HAL_MOTOR_PWM_REVERSE_SLOW     (0x03<<3)
  
#define PWM_DURATION_5       (0x01<<5)
#define PWM_DURATION_20      (0x02<<5)
#define PWM_DURATION_35      (0x03<<5)
#define PWM_DURATION_50      (0x04<<5)
#define PWM_DURATION_65      (0x05<<5)
#define PWM_DURATION_80      (0x06<<5)
#define PWM_DURATION_95      (0x07<<5)
  
  
  /*
  typedef enum PWM_Mode {
  PWM_OFF,
  PWM_FORWARD_FAST,
  PWM_FORWARD_SLOW,
  PWM_REVERSE_FAST,
  PWM_REVERSE_SLOW
} PWM_Mode;  
  */
  
  void board_pwm_init();
  void board_pwm_stop();
  void board_pwm_start(unsigned long freq_Hz, uint8 duration,uint8_t pin_id);   
  void Board_init_motor(void);   
  extern void init_motor(void);
  extern void open_motor(bool on,uint8_t mode);
  
  
#ifdef __cplusplus
}
#endif

#endif /* HT_BOARD_MOTOR_H */