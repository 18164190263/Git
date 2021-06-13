#include "sys.h"
#include "hlw8032.h"	  
////////////////////////////////////////////////////////////////////////////////// 	 

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////

u32 Voltage_Parameter_Reg,Voltage_Reg,Current_Reg,Current_Parameter_Reg,Power_Parameter_Reg,Power_Reg;
u8 Data_Updata_Reg;
u16 PF_Reg;
u8 CmdAnalysis(u8* data,u16 len)
{
	u16 sum;
	u8 cheaksum,i;
	if(len<24)return 1;
	else 
   {
		 for(i=2;i<len-1;i++)
		 {
			sum+=data[i];
		 }
		 cheaksum=sum&0xFF;
		 if(cheaksum==data[len-1])
		 {
			 Voltage_Parameter_Reg=(u32)data[2]<<16|(u32)data[3]<<8|(u32)data[4];
			 Voltage_Reg=(u32)data[5]<<16|(u32)data[6]<<8|(u32)data[7];
			 Current_Parameter_Reg=(u32)data[8]<<16|(u32)data[9]<<8|(u32)data[10];
			 Current_Reg=(u32)data[11]<<16|(u32)data[12]<<8|(u32)data[13];
			 Power_Parameter_Reg=(u32)data[14]<<16|(u32)data[15]<<8|(u32)data[16];
			 Power_Reg=(u32)data[17]<<16|(u32)data[18]<<8|(u32)data[19];
			 Data_Updata_Reg=data[20];
			 PF_Reg=(u16)data[21]<<8|data[22];
		 } 
		 else
			 return 1;
		 return 0;
	 }
}
 
#if EN_USART2_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART2_RX_STA=0;       //����״̬���	  
  
void uart2_init(u32 bound)
{
  //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	
	//USART1_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.2
   
	//USART1_RX	  GPIOA.3��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.3  

  //Usart1 NVIC ����
 	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//����ģʽ

	USART_Init(USART2, &USART_InitStructure); //��ʼ������1
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);  //�����ж�  
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���1 

}

void USART2_IRQHandler(void)                	//����1�жϷ������
	{
	u8 Res;
	u8 res;	
  u16 clear;	
   if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {
			
			 clear=USART2->SR;
			 clear=USART2->DR;
			if(CmdAnalysis(USART2_RX_BUF,USART2_RX_STA)==0)
			{
			
			
			}
				USART2_RX_STA=0;
  	}		
		else if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
				{
					Res =USART_ReceiveData(USART2);	//��ȡ���յ�������				
					USART2_RX_BUF[USART2_RX_STA]=Res ;
					USART2_RX_STA++;
					if(USART2_RX_STA>(USART2_REC_LEN-1))USART2_RX_STA=0;//�������ݴ���,���¿�ʼ����	  							 
				} 
} 
#endif	

