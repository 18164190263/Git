#ifndef  __KEY_H
#define  __KEY_H
#include "sys.h"
#include "delay.h"
#include "spwm.h"

#define key0   GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)
#define key1   GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)
#define key2   GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)

void key_init(void);







#endif

