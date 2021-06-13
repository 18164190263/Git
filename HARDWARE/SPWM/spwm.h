#ifndef  __SPWM_H
#define  __SPWM_H
#include "sys.h"
#include "adc.h"
#include "pid.h"
#include "usart.h"


#define spot   600          //一个SPWM周期内规则采样的个数

extern u8 pfreq;
extern float k;
extern float ACVotage;

void spwm_init(u16 pfreq,u16 pse);
void tim2_init(u16 pfreq,u16 pse);









#endif

