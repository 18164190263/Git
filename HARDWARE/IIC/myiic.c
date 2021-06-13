#include "myiic.h"
#include "delay.h"



void IIC_Init(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	IIC_SCL=1;
    IIC_SDA=1;
	
}

//����IIC��ʼ�ź�
//��SCL�ߵ�ƽ״̬�ڼ佫SDA����
void IIC_Start(void)
{
	SDA_OUT();
	IIC_SDA=1;
	IIC_SCL=1;
	delay_us(2);
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=0;   //ǯסI2C���ߣ�׼�����ͻ�������� 
}

//����IICֹͣ�ź�
//��SCL�ߵ�ƽ״̬�ڼ佫SDA����
void IIC_Stop(void)
{
	SDA_OUT();
	IIC_SCL=0;
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SDA=1;  //����I2C���߽����ź�
	delay_us(2);
}

//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 acktime=0;
	IIC_SDA=1;
	delay_us(2);
	SDA_IN();    //SDA����Ϊ����  
	IIC_SCL=1;
	delay_us(2);
	while(READ_SDA)
	{
		acktime++;
		if(acktime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//ʱ�����0 	   
	return 0; 
}

//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}

//IIC����һ���ֽ�
void IIC_Send_Byte(u8 data)
{
	u8 t;
	SDA_OUT();
	IIC_SCL=0;
	for(t=0;t<8;t++)
	{
		IIC_SDA=(data&0x80)>>7;
		data<<=1;
		delay_us(2);
		IIC_SCL=1;
		delay_us(2);
		IIC_SCL=0;
		delay_us(2);
	}
}

//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(u8 ack)
{
	u8 i,receive=0;
	SDA_IN();
	for(i=0;i<8;i++)
	{
		IIC_SCL=0;
		delay_us(2);
		IIC_SCL=1;
		receive<<=1;
		if(READ_SDA)receive++;
		delay_us(2);
	}
	if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}
