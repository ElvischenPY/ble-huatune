
#ifndef HT_BOARD_PWM_H
#define HT_BOARD_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

#define PWM_EXAMPLE
  
#define HAL_MOTOR_AIN1 0x01
#define HAL_MOTOR_AIN2 0x02
  
extern void board_pwm_init(uint8_t pin);  //HAL_MOTOR_AIN1  HAL_MOTOR_AIN2
//占空比 duration   取值  1~99, 百分比
extern void board_pwm_start(unsigned long freq_Hz, uint8 duration,uint8_t pin_id);

extern void board_pwm_stop();


#ifdef __cplusplus
}
#endif

#endif /* HT_BOARD_PWM_H */
