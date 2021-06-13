#include "spwm.h"


u8 pfreq = 50;           //默认频率50Hz
__IO float Period_percent;
float k = 0.75;



const u32 spwm[600] ={
2000,2020,2041,2062,2083,2104,2125,2146,2167,2188,2209,2229,2250,2271,2292,2312,
2333,2354,2374,2395,2415,2436,2456,2477,2497,2517,2537,2557,2578,2598,2618,2637,
2657,2677,2697,2716,2736,2755,2775,2794,2813,2832,2851,2870,2889,2907,2926,2945,
2963,2981,3000,3018,3036,3053,3071,3089,3106,3124,3141,3158,3175,3192,3209,3225,
3242,3258,3274,3290,3306,3322,3338,3353,3369,3384,3399,3414,3428,3443,3457,3472,
3486,3500,3513,3527,3541,3554,3567,3580,3593,3605,3618,3630,3642,3654,3665,3677,
3688,3699,3710,3721,3732,3742,3752,3762,3772,3782,3791,3800,3809,3818,3827,3835,
3843,3851,3859,3867,3874,3881,3888,3895,3902,3908,3914,3920,3926,3931,3937,3942,
3947,3951,3956,3960,3964,3968,3971,3975,3978,3981,3984,3986,3989,3991,3992,3994,
3996,3997,3998,3999,3999,3999,4000,3999,3999,3999,3998,3997,3996,3994,3992,3991,
3989,3986,3984,3981,3978,3975,3971,3968,3964,3960,3956,3951,3947,3942,3937,3931,
3926,3920,3914,3908,3902,3895,3888,3881,3874,3867,3859,3851,3843,3835,3827,3818,
3809,3800,3791,3782,3772,3762,3752,3742,3732,3721,3710,3699,3688,3677,3665,3654,
3642,3630,3618,3605,3593,3580,3567,3554,3541,3527,3513,3500,3486,3472,3457,3443,
3428,3414,3399,3384,3369,3353,3338,3322,3306,3290,3274,3258,3242,3225,3209,3192,
3175,3158,3141,3124,3106,3089,3071,3053,3036,3018,3000,2981,2963,2945,2926,2907,
2889,2870,2851,2832,2813,2794,2775,2755,2736,2716,2697,2677,2657,2637,2618,2598,
2578,2557,2537,2517,2497,2477,2456,2436,2415,2395,2374,2354,2333,2312,2292,2271,
2250,2229,2209,2188,2167,2146,2125,2104,2083,2062,2041,2020,2000,1979,1958,1937,
1916,1895,1874,1853,1832,1811,1790,1770,1749,1728,1707,1687,1666,1645,1625,1604,
1584,1563,1543,1522,1502,1482,1462,1442,1421,1401,1381,1362,1342,1322,1302,1283,
1263,1244,1224,1205,1186,1167,1148,1129,1110,1092,1073,1054,1036,1018,1000,981,
963,946,928,910,893,875,858,841,824,807,790,774,757,741,725,709,
693,677,661,646,630,615,600,585,571,556,542,527,513,499,486,472,
458,445,432,419,406,394,381,369,357,345,334,322,311,300,289,278,
267,257,247,237,227,217,208,199,190,181,172,164,156,148,140,132,
125,118,111,104,97,91,85,79,73,68,62,57,52,48,43,39,
35,31,28,24,21,18,15,13,10,8,7,5,3,2,1,0,
0,0,0,0,0,0,1,2,3,5,7,8,10,13,15,18,
21,24,28,31,35,39,43,48,52,57,62,68,73,79,85,91,
97,104,111,118,125,132,140,148,156,164,172,181,190,199,208,217,
227,237,247,257,267,278,289,300,311,322,334,345,357,369,381,394,
406,419,432,445,458,472,486,499,513,527,542,556,571,585,600,615,
630,646,661,677,693,709,725,741,757,774,790,807,824,841,858,875,
893,910,928,946,963,981,1000,1018,1036,1054,1073,1092,1110,1129,1148,1167,
1186,1205,1224,1244,1263,1283,1302,1322,1342,1362,1381,1401,1421,1442,1462,1482,
1502,1522,1543,1563,1584,1604,1625,1645,1666,1687,1707,1728,1749,1770,1790,1811,
1832,1853,1874,1895,1916,1937,1958,1979

};



static u16 i = 0;       //取样点
u16 TimerPeriod = 0;   //重装载值
u16 Channel3Pulse = 0;

float ACVotage_Last;

void spwm_init(u16 arr,u16 pse)
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_BDTRInitTypeDef TIM_BDTRInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	TimerPeriod = (SystemCoreClock / (arr*pfreq) ) - 1;
	Channel3Pulse = (u16)((u32)(50  * (TimerPeriod - 1)) / 100 );	//占空比50%
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM1,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;                       //推挽输出模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = TimerPeriod;
	TIM_TimeBaseInitStruct.TIM_Prescaler = pse;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);
	
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;//重置输出比较空闲状态
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;//PWM输出模式1
	TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;//互补通道重置输出比较空闲状态
	TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Enable;//互补输出使能，使能CHxN的PWM输出
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//使能CHx的PWM输出
	
	TIM_OCInitStruct.TIM_Pulse = Channel3Pulse;
	TIM_OC3Init(TIM1,&TIM_OCInitStruct);
	
	TIM_BDTRInitStruct.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;//自动输出使能
	TIM_BDTRInitStruct.TIM_Break = TIM_Break_Disable;//禁止刹车使能
	TIM_BDTRInitStruct.TIM_BreakPolarity = TIM_BreakPolarity_High;//刹车极性为高
	TIM_BDTRInitStruct.TIM_DeadTime = 0x54;//死区发生器 死去持续时间                        
	TIM_BDTRInitStruct.TIM_LOCKLevel = TIM_LOCKLevel_OFF;//锁定关闭
	TIM_BDTRInitStruct.TIM_OSSIState = TIM_OSSIState_Disable;//工作状态下关闭状态为低电平
	TIM_BDTRInitStruct.TIM_OSSRState = TIM_OSSRState_Disable;//空闲状态下关闭状态为低电平
	TIM_BDTRConfig(TIM1,&TIM_BDTRInitStruct);
	
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
	
	TIM_OC3PreloadConfig(TIM1,TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM1,ENABLE);
	TIM_Cmd(TIM1,ENABLE);
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
	
}


void TIM1_UP_IRQHandler(void)
{
	
	float temp;
	float Period_percent;
	static u8 j=0;
	temp = (SystemCoreClock / (spot*pfreq) ) - 1;
	Period_percent = temp / 4000.0;
	
	if(TIM_GetITStatus(TIM1,TIM_IT_Update)!=RESET)
	{
		TIM_SetAutoreload(TIM1,temp);
		TIM_SetAutoreload(TIM2,temp);
		TIM_SetCompare3(TIM1,(u16)(k*Period_percent*spwm[i++]));
		if(i==600)	
		{
//			j++;
//			if(j==7)
//			{
//				if(ACVotage<=30)
//				{
//					ACVotage_Last = ACVotage;
//				}
//				
//				k = PID_realize(ACVotage_Last);
//				j=0;
//			}

			i=0;
		}
		
		
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
	
}


void tim2_init(u16 arr,u16 pse)
{
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = arr;
	TIM_TimeBaseInitStruct.TIM_Prescaler = pse;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM2,ENABLE);
	
}

//50ms中断一次
void TIM2_IRQHandler(void)
{
	
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
	{
//		pid.Pv = (float)( Get_Adc(ADC_Channel_0)*(3.3/4096) );
//		pid_calculation();
//		k = pid.OUT ;
//		printf("%f,%f,%f,%f\n",k,pid.Sv,pid.Pv,pid.OUT);
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
	
}


