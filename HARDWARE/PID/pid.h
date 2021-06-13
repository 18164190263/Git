#ifndef __PID_H
#define __PID_H
#include "sys.h"
#include  "adc.h"

typedef struct
{
    /* data */
	float SetSpeed;//设定值
	float ActualSpeed;//实际值
	float err;
	float err_last;
	float Kp;
	float Ki;
	float Kd;
	float out;//执行器的变量
	float integral;//积分值
	float Umax;          //最大正饱和上限值
	float Umin;          //最大负饱和下限值
}PID_Typedef;

extern PID_Typedef Current;


void PID_Init(void);
float PID_realize(float ActualSpeed);









#endif


