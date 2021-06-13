#ifndef __PID_H
#define __PID_H
#include "sys.h"
#include  "adc.h"

typedef struct
{
    /* data */
	float SetSpeed;//�趨ֵ
	float ActualSpeed;//ʵ��ֵ
	float err;
	float err_last;
	float Kp;
	float Ki;
	float Kd;
	float out;//ִ�����ı���
	float integral;//����ֵ
	float Umax;          //�������������ֵ
	float Umin;          //��󸺱�������ֵ
}PID_Typedef;

extern PID_Typedef Current;


void PID_Init(void);
float PID_realize(float ActualSpeed);









#endif


