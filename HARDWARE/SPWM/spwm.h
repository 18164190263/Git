#ifndef  __SPWM_H
#define  __SPWM_H
#include "sys.h"
#include "adc.h"
#include "pid.h"
#include "usart.h"


#define spot   600          //һ��SPWM�����ڹ�������ĸ���

extern u8 pfreq;
extern float k;
extern float ACVotage;

void spwm_init(u16 pfreq,u16 pse);
void tim2_init(u16 pfreq,u16 pse);









#endif

