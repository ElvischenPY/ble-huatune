
#ifndef BOARD_PWM_H
#define BOARD_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

#define PWM_EXAMPLE

extern void board_pwm_init();  

//ռ�ձ� duration   ȡֵ  1~99, �ٷֱ�
extern void board_pwm_start(unsigned long freq_Hz, uint8 duration);

extern void board_pwm_stop();


#ifdef __cplusplus
}
#endif

#endif /* BOARD_BUZZER_H */
