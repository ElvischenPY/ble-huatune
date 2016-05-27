#include <driverlib/timer.h>
#include <ti/sysbios/family/arm/cc26xx/Power.h>
#include <ti/sysbios/family/arm/cc26xx/PowerCC2650.h>
#include <ti/drivers/pin/PINCC26XX.h>

#include "Board.h"
#include "ht_board_led.h"
#include "ht_board_uart.h"
#include "ht_board_audio.h"
#include <ti/sysbios/knl/Clock.h>
#include "util.h"
#include <ti/sysbios/knl/Task.h>

//#define DBG_ON

#ifndef DBG_ON
#define pr_err   uart_print
#define pr_info(x...)
#else
#define pr_err   uart_print
#define pr_info  uart_print
#endif

static PIN_State  Audio_Pins;
static PIN_Handle hAudio_Pins = NULL;

static Clock_Struct PWMClockHight;
static Clock_Struct PWMClockLow;

extern const tNote MyScore_liangzhu[] = {  
  {L3, T/4},  
  {L5, T/8+T/16},  
  {L6, T/16},  
  {M1, T/8+T/16},  
  {M2, T/16},  
  {L6, T/16},  
  {M1, T/16}, 
  {L5, T/8},  
  {M5, T/8+T/16},  
  {H1, T/16},  
  {M6, T/16},  
  {M5, T/16},  
  {M3, T/16},  
  {M5, T/16},  
  {M2, T/2},  
  { 0, 0}  
};
extern const tNote MyScore_hetang[]=
{
  {M1,TT/8},{M1,TT/4},{L6,TT/8},{L5,TT/4},{L6,TT/4},{M1,TT/4},{M1,TT/8},{M2,TT/8},{M3,TT/2},{M2,TT/8},{M2,TT/4},{M1,TT/8},{M2,TT/4},{M2,TT/8},{M5,TT/8},{M5,TT/8},{M3,TT/8},
  {M3,TT/8},{M2,TT/8},{M3,TT/2},{M1,TT/8},{M1,TT/4},{L6,TT/8},{L5,TT/4},{M5,TT/4},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/2},{M2,TT/8},{M2,TT/4},//苍茫的天涯是我的爱。。。最呀最摇
  {M1,TT/8},{M2,TT/8},{M2,TT/4},{M3,TT/8},{M2,TT/8},{M1,TT/8},{L6,TT/8},{M2,TT/8},{M1,TT/2},//剪一段时光。。。美丽的琴音就落在我身旁
  {M1,TT/8},{M1,TT/4},{L6,TT/8},{L5,TT/4},{L6,TT/4},{M1,TT/8},{M1,TT/4},{M2,TT/8},{M3,TT/2},{M2,TT/8},{M2,TT/4},{M1,TT/8},{M2,TT/4},{M2,TT/8},{M5,TT/8},{M5,TT/8},{M3,TT/8},
  {M3,TT/8},{M2,TT/8},{M3,TT/2},{M1,TT/8},{M1,TT/8},{M1,TT/8},{L6,TT/8},{L5,TT/4},{M5,TT/4},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/2},{M2,TT/8},{M2,TT/4},//苍茫的天涯是我的爱。。。最呀最摇
  {M1,TT/8},{M2,TT/8},{M2,TT/4},{M3,TT/8},{M2,TT/8},{M1,TT/8},{L6,TT/8},{M2,TT/8},{M1,TT/2},//萤火虫。。。谁采下那一朵昨夜的忧伤
  {M3,TT/8},{M5,TT/4},{M5,TT/8},{M5,TT/4},{M5,TT/4},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/2},{M6,TT/8},{H1,TT/8},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},
  {L6,TT/8},{M2,TT/4},{M2,TT/8},{M3,TT/8},{M3,TT/8},{M2,TT/4+TT/8},{M3,TT/8},{M5,TT/4},{M5,TT/8},{M5,TT/4},{M5,TT/4},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},//苍茫的天涯是我的爱。。。最呀最摇
  {M1,TT/2},{L6,TT/8},{M1,TT/8},{L6,TT/8},{L5,TT/8},{M2,TT/4},{M3,TT/4},{M1,TT/2+TT/4},	//我像只鱼儿。。。等你宛在水中央
  {M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT},{M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},{M1,TT/8},{M2,TT/8},{M2,TT},{M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},
  {M2,TT/8},{M1,TT/8},{L6,TT/2},{L6,TT/8},{L5,TT/8},{M1,TT/8},{M2,TT/8},{M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},{M1,TT/8},{L6,TT/8},{M1,TT},
  {M1,TT/8},{M1,TT/4},{L6,TT/8},{L5,TT/4},{L6,TT/4},{M1,TT/8},{M1,TT/4},{M2,TT/8},{M3,TT/2},{M2,TT/8},{M2,TT/4},{M1,TT/8},{M2,TT/4},{M2,TT/8},{M5,TT/8},{M5,TT/8},{M3,TT/8},
  {M3,TT/8},{M2,TT/8},{M3,TT/2},{M1,TT/8},{M1,TT/8},{M1,TT/8},{L6,TT/8},{L5,TT/4},{M5,TT/4},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/2},{M2,TT/8},{M2,TT/4},//苍茫的天涯是我的爱。。。最呀最摇
  {M1,TT/8},{M2,TT/8},{M2,TT/4},{M3,TT/8},{M2,TT/8},{M1,TT/8},{L6,TT/8},{M2,TT/8},{M1,TT/2},//萤火虫。。。谁采下那一朵昨夜的忧伤
  {M3,TT/8},{M5,TT/4},{M5,TT/8},{M5,TT/4},{M5,TT/4},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/2},{M6,TT/8},{H1,TT/8},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},
  {L6,TT/8},{M2,TT/4},{M2,TT/8},{M3,TT/8},{M3,TT/8},{M2,TT/4+TT/8},{M3,TT/8},{M5,TT/4},{M5,TT/8},{M5,TT/4},{M5,TT/4},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},//苍茫的天涯是我的爱。。。最呀最摇
  {M1,TT/2},{L6,TT/8},{M1,TT/8},{L6,TT/8},{L5,TT/8},{M2,TT/4},{M3,TT/4},{M1,TT/2+TT/4},	//我像只鱼儿。。。等你宛在水中央
  {M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT},{M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},{M1,TT/8},{M2,TT/8},{M2,TT},{M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},
  {M2,TT/8},{M1,TT/8},{L6,TT/2},{L6,TT/8},{L5,TT/8},{M1,TT/8},{M2,TT/8},{M1,TT/4+TT/8},{M5,TT/8},{M1,TT/8},{M5,TT/8},{M1,TT/8},{L6,TT/8},{M1,TT},
  
  {0,0},
};
extern const tNote MyScore_gongxi[]=
{
  {L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M3,TT/8},{M3,TT/4},
  {L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M6,TT/8},{M6,TT/2},
  {M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},{M1,TT/8},{L6,TT/4},
  {M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},{M1,TT/8},{M2,TT/4},
  {M2,TT/8+TT/16},{M1,TT/8},{M1,TT/4},{M2,TT/4},{M3,TT/4},{M5,TT/4},
  {M6,TT},{M6,TT/8+TT/16},{M5,TT/16},{M3,TT/8},{M5,TT/8},{M6,TT/4}, //恭喜你发财。。。礼多人不怪
  
  {L3,TT/8},{L6,TT/4},{L6,TT/8},{L5,TT/8},{L6,TT/8},{L3,TT/8},{L3,TT/8},
  {L5,TT/8},{L6,TT/8},{M1,TT/8},{L6,TT/8},{L5,TT/8},{L6,TT/4},{L3,TT/8},
  {L5,TT/8},{M1,TT/4},{M1,TT/8},{M1,TT/8},{M2,TT/8},{M2,TT/8},{M1,TT/8},
  {M2,TT/8},{M3,TT/2},{L3,TT/8},{M2,TT/4},{M2,TT/8},{M1,TT/8},{M2,TT/8},
  {L6,TT/8},{L6,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},
  {M2,TT/8},{M1,TT/8},{L6,TT/8},{M1,TT/8},{M3,TT/8},{M3,TT/8},{M2,TT/8},
  {M1,TT/8},{M3,TT/4},{L5,TT/4},{L6,TT/2},//我祝满天下的女孩。。。智商充满你脑袋
  
  {L3,TT/8},{L6,TT/4},{L6,TT/8},{L5,TT/8},{L6,TT/8},{L3,TT/8},{L3,TT/8},
  {L5,TT/8},{L6,TT/8},{M1,TT/8},{L6,TT/8},{L5,TT/8},{L6,TT/4},{L3,TT/8},
  {L5,TT/8},{M1,TT/4},{M1,TT/8},{M1,TT/8},{M2,TT/8},{M2,TT/8},{M1,TT/8},
  {M2,TT/8},{M3,TT/2},{L3,TT/8},{M2,TT/4},{M2,TT/8},{M1,TT/8},{M2,TT/8},
  {L6,TT/8},{L6,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},
  {M2,TT/8},{M1,TT/8},{L6,TT/8},{M1,TT/8},{M3,TT/8},{M3,TT/8},{M2,TT/8},
  {M1,TT/8},{M3,TT/4},{L5,TT/4},{L6,TT/2},//我祝满天下的女孩。。。智商充满你脑袋
  
  {L5,TT/4},{M1,TT/2+TT/4},{M1,TT/8},{M2,TT/8},{M3,TT/2+TT/4},{M3,TT/8},
  {M5,TT/8},{M5,TT/4+TT/8},{M3,TT/8},{M2,TT/4},{M1,TT/4},{M2,TT/2},
  {M2,TT/4+TT/8},{L6,TT/8},{M2,TT/4},{M3,TT/4},{M4,TT/8+TT/16},
  {M5,TT/16},{M4,TT/8},{M3,TT/8},{M2,TT/2},{M5,TT/8},{M5,TT/8},
  {M3,TT/8},{M2,TT/8},{M1,TT/4},{L5,TT/8},{L6,TT/2},//大摇大摆。。。要喊得够豪迈
  
  {L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M3,TT/8},{M3,TT/4},
  {L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M6,TT/8},{M6,TT/2},
  {M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},{M1,TT/8},{L6,TT/4},
  {M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},{M1,TT/8},{M2,TT/4},
  {M2,TT/8+TT/16},{M1,TT/8},{M1,TT/4},{M2,TT/4},{M3,TT/4},{M5,TT/4},
  {M6,TT},{M6,TT/8+TT/16},{M5,TT/16},{M3,TT/8},{M5,TT/8},{M6,TT/4}, //恭喜你发财。。。礼多人不怪
  
  {L3,TT/8},{L6,TT/4},{L6,TT/8},{L5,TT/8},{L6,TT/8},{L3,TT/8},{L3,TT/8},
  {L5,TT/8},{L6,TT/8},{M1,TT/8},{L6,TT/8},{L5,TT/8},{L6,TT/4},{L3,TT/8},
  {L5,TT/8},{M1,TT/4},{M1,TT/8},{M1,TT/8},{M2,TT/8},{M2,TT/8},{M1,TT/8},
  {M2,TT/8},{M3,TT/2},{L3,TT/8},{M2,TT/4},{M2,TT/8},{M1,TT/8},{M2,TT/8},
  {L6,TT/8},{L6,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},
  {M2,TT/8},{M1,TT/8},{L6,TT/8},{M1,TT/8},{M3,TT/8},{M3,TT/8},{M2,TT/8},
  {M1,TT/8},{M3,TT/4},{L5,TT/4},{L6,TT/2},//我祝满天下的女孩。。。智商充满你脑袋
  
  {L5,TT/4},{M1,TT/2+TT/4},{M1,TT/8},{M2,TT/8},{M3,TT/2+TT/4},{M3,TT/8},
  {M5,TT/8},{M5,TT/4+TT/8},{M3,TT/8},{M2,TT/4},{M1,TT/4},{M2,TT/2},
  {M2,TT/4+TT/8},{L6,TT/8},{M2,TT/4},{M3,TT/4},{M4,TT/8+TT/16},{M5,TT/16},
  {M4,TT/8},{M3,TT/8},{M2,TT/2},{M5,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},
  {M1,TT/4},{L5,TT/8},{L6,TT/2},//大摇大摆。。。要喊得够豪迈
  
  {L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M3,TT/8},{M3,TT/4},
  {L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M6,TT/8},{M6,TT/2},
  {M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},{M1,TT/8},{L6,TT/4},
  {M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},{M1,TT/8},{M2,TT/4},
  {M2,TT/8+TT/16},{M1,TT/8},{M1,TT/4},{M2,TT/4},{M3,TT/4},{M5,TT/4},
  {M6,TT},{M6,TT/8+TT/16},{M5,TT/16},{M3,TT/8},{M5,TT/8},{M6,TT/4}, //恭喜你发财。。。礼多人不怪
  
  {L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M3,TT/8},{M3,TT/4},
  {L3,TT/8},{M6,TT/4},{M5,TT/4},{M6,TT/4},{M5,TT/8},{M6,TT/8},{M6,TT/2},
  {M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},{M1,TT/8},{L6,TT/4},
  {M3,TT/8},{M2,TT/8+TT/16},{M3,TT/16},{M2,TT/8},{M1,TT/8},{M2,TT/4},
  {M2,TT/8+TT/16},{M1,TT/8},{M1,TT/4},{M2,TT/4},{M3,TT/4},{M5,TT/4},
  {M6,TT},{M6,TT/8+TT/16},{M5,TT/16},{M3,TT/8},{M5,TT/8},{M6,TT/4}, //恭喜你发财。。。礼多人不怪
  {0,0},
};
extern const tNote MyScore_zuixuan[]=
{
  {L6,TT/4},{L3,TT/8},{L5,TT/8},{L6,TT/4},{L6,TT/8},{M1,TT/8},{M1,TT/4},{M2,TT/8},
  {M1,TT/8},{L6,TT/2},{M1,TT/4},{M1,TT/8},{L5,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/8},
  {M5,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/4},{M3,TT/2},{M6,TT/8},{M6,TT/8},{M6,TT/8},
  {M5,TT/8},{M3,TT/8},{M3,TT/4},{M1,TT/8},{L6,TT/8},{L6,TT/8},{L6,TT/8},{M3,TT/8},//苍茫的天涯是我的爱。。。最呀最摇
  
  {M2,TT/2},{M3,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},
  {M1,TT/8},{L6,TT/4},{L5,TT/4},{L6,TT/2}, //摆，什么样的歌声才是最开怀
  {L6,TT/4},{L3,TT/8},{L5,TT/8},{L6,TT/4},{L6,TT/8},{M1,TT/8},{M1,TT/4},{M2,TT/8},
  {M1,TT/8},{L6,TT/2},{M1,TT/4},{M1,TT/8},{L5,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/8},
  {M5,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/4},{M3,TT/2},{M6,TT/8},{M6,TT/8},{M6,TT/8},
  {M5,TT/8},{M3,TT/8},{M3,TT/4},{M1,TT/8},{L6,TT/8},{L6,TT/8},{L6,TT/8},{M3,TT/8},//苍茫的天涯是我的爱。。。最呀最摇
  
  {M2,TT/2},{M3,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/8},
  {M1,TT/8},{L6,TT/4},{L5,TT/4},{L6,TT/2}, //摆，什么样的歌声才是最开怀
  
  {M3,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M3,TT/8},{M5,TT/8},{M5,TT/8},{M6,TT/8},
  {H1,TT/8},{M6,TT/8},{M5,TT/4},{M6,TT/2},{L6,TT/4},{L6,TT/8},{L5,TT/8},{L6,TT/4},
  {M1,TT/4},{M2,TT/8},{M3,TT/16},{M2,TT/16},{M1,TT/8},{M2,TT/8},{M3,TT/2},{L6,TT/8},
  {M6,TT/8},{M6,TT/8},{M5,TT/8},{M2,TT/8},{M3,TT/16},{M2,TT/16},{M1,TT/8},{M2,TT/8},{M3,TT/2},//留下来
  
  {M1,TT/8},{L6,TT/8},{L6,TT/8},{M1,TT/8},{M2,TT/4},{L5,TT/8},{L5,TT/8},{M3,TT/8},
  {M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/2},{L6,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/8},
  {M2,TT/8},{M1,TT/8},{L5,TT/8},{L3,TT/8},{L6,TT/2},{L6,TT/4},{L6,TT/8},{L5,TT/8},
  {L6,TT/4},{M1,TT/4},{M2,TT/8},{M3,TT/16},{M2,TT/16},{M1,TT/8},{M2,TT/8},{M3,TT/2},
  {L6,TT/8},{M6,TT/8},{M6,TT/8},{M5,TT/8},{M2,TT/8},{M3,TT/16},{M2,TT/16},{M1,TT/8},
  {M2,TT/8},{M3,TT/2},//留下来
  
  {M1,TT/8},{L6,TT/8},{L6,TT/8},{M1,TT/8},{M2,TT/4},{L5,TT/8},{L5,TT/8},{M3,TT/8},
  {M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/4+TT/8},{M1,TT/8},{L6,TT/8},{M1,TT/8},
  {M2,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M3,TT/8},{M5,TT/8},{M6,TT/2},{M6,TT/2},
  {L6,TT/4},{L6,TT/8},{L5,TT/8},{L6,TT/4},{L6,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/16},
  {M2,TT/16},{M1,TT/8},{M2,TT/8},{M3,TT/2},{M6,TT/8},{M5,TT/8},{M3,TT/8},{M2,TT/8},
  {M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{M1,TT/2},//登上天外云霄的舞台
  
  {L6,TT/4},{L6,TT/8},{L5,TT/8},{L6,TT/4},{M1,TT/4},{M2,TT/8},{M3,TT/16},{M2,TT/16},
  {M1,TT/8},{M2,TT/8},{M3,TT/2},{L6,TT/8},{M6,TT/8},{M6,TT/8},{M5,TT/8},{M2,TT/8},
  {M3,TT/16},{M2,TT/16},{M1,TT/8},{M2,TT/8},{M3,TT/2},//留下来
  
  {M1,TT/8},{L6,TT/8},{L6,TT/8},{M1,TT/8},{M2,TT/4},{L5,TT/8},{L5,TT/8},{M3,TT/8},
  {M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/2},{L6,TT/8},{M1,TT/8},{M2,TT/8},{M3,TT/8},
  {M2,TT/8},{M1,TT/8},{L5,TT/8},{L3,TT/8},{L6,TT/2},{L6,TT/4},{L6,TT/8},{L5,TT/8},
  {L6,TT/4},{M1,TT/4},{M2,TT/8},{M3,TT/16},{M2,TT/16},{M1,TT/8},{M2,TT/8},{M3,TT/2},
  {L6,TT/8},{M6,TT/8},{M6,TT/8},{M5,TT/8},{M2,TT/8},{M3,TT/16},{M2,TT/16},{M1,TT/8},
  {M2,TT/8},{M3,TT/2},//留下来
  
  {M1,TT/8},{L6,TT/8},{L6,TT/8},{M1,TT/8},{M2,TT/4},{L5,TT/8},{L5,TT/8},{M3,TT/8},
  {M5,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/4+TT/8},{M1,TT/8},{L6,TT/8},{M1,TT/8},
  {M2,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/8},{M3,TT/8},{M5,TT/8},{M6,TT/2},{M6,TT/2},
  {0,0},
};

// PIN configuration structure to set all LED pins as output
PIN_Config audioPinsCfg[] =
{
  Board_AUDIO_PWM    | PIN_GPIO_OUTPUT_EN   | PIN_GPIO_LOW    | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  Board_AUDIO_SD_SELECT | PIN_GPIO_OUTPUT_EN | PIN_INPUT_DIS |  PIN_PULLDOWN,
  PIN_TERMINATE
};

static void Board_init_audio(void)
{
  hAudio_Pins = PIN_open(&Audio_Pins, audioPinsCfg);
  
}

static void board_pwm_init()
{
  
  Power_setDependency(PERIPH_GPT1);
  TimerConfigure(GPT1_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_CAP_COUNT);
}

void init_audio(void)
{
  if(hAudio_Pins == NULL){
    Board_init_audio();
    board_pwm_init();
    PIN_setOutputValue(hAudio_Pins, Board_AUDIO_SD_SELECT,1); 
  }
}


static Clock_Struct audioClockStruct_H;
static Clock_Handle audioClockHandle_H = 0;
static Clock_Struct audioClockStruct_L;
static Clock_Handle audioClockHandle_L = 0;

void pwm_setio(UArg arg0)
{
  if(PIN_getOutputValue(Board_AUDIO_PWM)){
    PIN_setOutputValue(hAudio_Pins, Board_AUDIO_PWM,0);
    Clock_start(audioClockHandle_L);
    Clock_stop(audioClockHandle_H);     
  }else{  
    PIN_setOutputValue(hAudio_Pins, Board_AUDIO_PWM,1);
    Clock_start(audioClockHandle_H);
    Clock_stop(audioClockHandle_L);  
  }
}

void board_pwm_audio(unsigned long freq_Hz, uint8 duration)
{    
  uint32_t PWM_FREQ = freq_Hz;
  uint32_t PWM_DIV_FACTOR = 48000000/(PWM_FREQ);
  uint32_t TIMER_LOADSET = (PWM_DIV_FACTOR-1);
  uint32_t TIMER_MATCH;
  uint32_t Hight_time;
  uint32_t Low_time;
  //  uint32_t time = 1000000/(PWM_FREQ);
  Clock_Params clockParams_h;
  Clock_Params clockParams_l;
  
  pr_info("Hz=%d ",freq_Hz);
  if(duration > 100)
    duration = 100;
  
  if(freq_Hz < 750){
    switch(PWM_FREQ){
    case L1: PWM_FREQ = 268;break;
    case L2: PWM_FREQ = 301;break;
    case L3: PWM_FREQ = 340;break;
    case L4: PWM_FREQ = 361;break;
    case L5: PWM_FREQ = 405;break;
    case L6: PWM_FREQ = 457;break;
    case M1: PWM_FREQ = 542;break;
    case M2: PWM_FREQ = 620;break;
    case M3: PWM_FREQ = 690;break;
    case M4: PWM_FREQ = 745;break;
    default : break;
    }
    Hight_time = ((2*(1000000*duration)/(PWM_FREQ*100))+1)/2;
    Low_time = 1000000/(PWM_FREQ) -Hight_time;    
    stop_pwm();
    PINCC26XX_setMux(hAudio_Pins, Board_AUDIO_PWM, IOC_PORT_GPIO); 
    pr_info("Hight_time=%lu  Hight_time=%lu   Clock_tickPeriod=%lu\n",Hight_time,Low_time,Clock_tickPeriod);
    Clock_Params_init(&clockParams_h);
    clockParams_h.arg = 0;
    clockParams_h.period = 0;
    clockParams_h.startFlag = false;
    Clock_construct(&audioClockStruct_H, pwm_setio ,((2*Hight_time/(Clock_tickPeriod)+1)/2),&clockParams_h);
    audioClockHandle_H = Clock_handle(&audioClockStruct_H);      
    
    Clock_Params_init(&clockParams_l);
    clockParams_l.arg = 0;
    clockParams_l.period = 0;
    clockParams_l.startFlag = false;
    Clock_construct(&audioClockStruct_L,pwm_setio,((2*Low_time/(Clock_tickPeriod)+1)/2), &clockParams_l);
    audioClockHandle_L = Clock_handle(&audioClockStruct_L);
    if (!Clock_isActive(audioClockHandle_H)){
      Clock_start(audioClockHandle_H);
    }
  }else{
    TIMER_MATCH = (PWM_DIV_FACTOR*(100-duration)/100-1);
    pr_info("freq_Hz=%d    duration=%d  len=%d\n",freq_Hz,duration,sizeof(PWM_DIV_FACTOR));
    pr_info("PWM_DIV_FACTOR=%#x TIMER_LOADSET=%#x  TIMER_MATCH=%#x \n",PWM_DIV_FACTOR,TIMER_LOADSET,TIMER_MATCH);
    //TimerPrescaleSet(GPT1_BASE,TIMER_A,1);
    
    TimerLoadSet(GPT1_BASE,TIMER_A,TIMER_LOADSET);  
    TimerDisable(GPT1_BASE, TIMER_A);
    PINCC26XX_setMux(hAudio_Pins, Board_AUDIO_PWM, IOC_PORT_MCU_PORT_EVENT2); 
    TimerMatchSet(GPT1_BASE,TIMER_A,TIMER_MATCH);
    //TimerEventControl(GPT1_BASE, TIMER_A, TIMER_EVENT_BOTH_EDGES);
    TimerEnable(GPT1_BASE,TIMER_A);
    
  } 
}
void stop_pwm(void)
{
  TimerDisable(GPT1_BASE, TIMER_A);
  Power_setConstraint(Power_STANDBY);
  // PIN_setOutputValue(hAudio_Pins, Board_AUDIO_PWM,0);
  
  if((audioClockHandle_L!=0) && (audioClockHandle_H!=0)){
    if(Clock_isActive(audioClockHandle_L))
      Clock_stop(audioClockHandle_L);
    if(Clock_isActive(audioClockHandle_H))
      Clock_stop(audioClockHandle_H);
  }
  
  
}


#define DING_LONG    0
#define PLAY_MUSIC   1
void play_music(const tNote MyScore[])
{
#if DING_LONG  
  uint8 j=5;
  while(j--){
    board_pwm_audio(515,8);
    // board_pwm_audio(PWM_1K_HZ,80);
    Task_sleep(400*1000/Clock_tickPeriod);
    stop_pwm();
    board_pwm_audio(700,10);
    Task_sleep(500*1000/Clock_tickPeriod);
    stop_pwm();
    
    
    delay_ms(2*1000);
  }
#endif 
  /*
  if(MyScore_hetang[0].mTime == 0)
  return;
  pr_info("feq=%d  time=%d\n\n",MyScore_hetang[index].mName,MyScore_hetang[index].mTime);
  board_pwm_audio(MyScore_hetang[0].mName,8);
  Util_constructClock(&playClock,playHandler,MyScore_hetang[0].mTime,0,true,0);
  if(!Util_isActive(&playClock)){
  pr_info("start 0");
  Util_startClock(&playClock);
}
  */
#if PLAY_MUSIC  
  uint8 i=0;
  stop_pwm();
  for(;;){
    if(MyScore[i].mTime == 0)
      break;
    pr_info("feq = %d   time=%d \n\n",MyScore[i].mName,MyScore[i].mTime);
    board_pwm_audio(MyScore[i].mName,5);
    Task_sleep((MyScore[i].mTime)*1000/Clock_tickPeriod);
    i++;
    stop_pwm();
    delay_ms(10);
  } 
#endif
}


