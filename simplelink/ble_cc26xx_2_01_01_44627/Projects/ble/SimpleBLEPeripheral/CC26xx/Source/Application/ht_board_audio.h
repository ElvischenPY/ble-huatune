#ifndef HT_BOARD_AUDIO_H
#define HT_BOARD_AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif
  
#define PWM_100_HZ 100
#define PWM_1K_HZ 1000 
#define PWM_10K_HZ 10*1000 
#define PWM_100K_HZ 100*1000 
  
#define delay_ms(i) ( CPUdelay(12000*(i)) )
#define delay_us(i) ( CPUdelay(8*(i)) )
  
  //低音 （hz）
#define L1 262  //c
#define L2 294  //d
#define L3 330  //e
#define L4 349  //f
#define L5 392  //g
#define L6 440  //a1
#define L7 944  //b1
  
  // 定义中音音名 
#define M1 523 // c1  
#define M2 587 // d1  
#define M3 659 // e1  
#define M4 698 // f1 
#define M5 784 // g1  
#define M6 880 // a2  
#define M7 988 // b2 
  
  // 定义高音音名 
#define H1 1047 // c2  
#define H2 1175 // d2  
#define H3 1319 // e2  
#define H4 1397 // f2  
#define H5 1568 // g2  
#define H6 1760 // a3  
#define H7 1976 // b3
  
  // 定义时值单位，决定演奏速度（数值单位：ms）  
#define T 3600
#define TT 2000
  // 定义音符结构 
  typedef struct{  
	short mName; // 音名：取值L1～L7、M1～M7、H1～H7分别表示低音、中音、高音的 
    // 1234567，取值0表示休止符  
	short mTime; // 时值：取值T、T/2、T/4、T/8、T/16、T/32分别表示全音符、 
    // 二分音符、四分音符、八分音符⋯，取值0表示演奏结束 
  }tNote; 
  
  
  // 定义乐曲：《化蝶》（梁祝）  
  extern const tNote MyScore_zuixuan[];
  extern const tNote MyScore_gongxi[];
  extern const tNote MyScore_hetang[];
  extern const tNote MyScore_liangzhu[];
  
  static void Board_init_audio(void);
  static void board_pwm_init();
  extern void init_audio(void);
  void board_pwm_audio(unsigned long freq_Hz, uint8 duration);
  extern void play_music(const tNote MyScore[]);
  void stop_pwm(void);
  
  
#ifdef __cplusplus
}
#endif

#endif /* HT_BOARD_KEY_H */
