#ifndef __MyIIC_H
#define __MyIIC_H
#include "sys.h"


#define  SDA_IN()   {GPIOA->CRL&=0xfffff0ff;GPIOA->CRL|=(8<<2*4);delay_us(1);}
#define  SDA_OUT()  {GPIOA->CRL&=0xfffff0ff;GPIOA->CRL|=(3<<2*4);delay_us(1);}

#define  IIC_SCL    PAout(1)   //SCL
#define  IIC_SDA    PAout(2)   //SDA
#define  READ_SDA   PAin(2)  //ÊäÈëSDA 

void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
u8 IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
void IIC_Send_Byte(u8 data);
u8 IIC_Read_Byte(u8 ack);




#endif

