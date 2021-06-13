#include "sys.h"
#include "hlw8032.h"	  
////////////////////////////////////////////////////////////////////////////////// 	 

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
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
 
#if EN_USART2_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART2_RX_STA=0;       //接收状态标记	  
  
void uart2_init(u32 bound)
{
  //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	
	//USART1_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2
   
	//USART1_RX	  GPIOA.3初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3  

  //Usart1 NVIC 配置
 	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//接收模式

	USART_Init(USART2, &USART_InitStructure); //初始化串口1
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);  //开启中断  
	USART_Cmd(USART2, ENABLE);                    //使能串口1 

}

void USART2_IRQHandler(void)                	//串口1中断服务程序
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
		else if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
				{
					Res =USART_ReceiveData(USART2);	//读取接收到的数据				
					USART2_RX_BUF[USART2_RX_STA]=Res ;
					USART2_RX_STA++;
					if(USART2_RX_STA>(USART2_REC_LEN-1))USART2_RX_STA=0;//接收数据错误,重新开始接收	  							 
				} 
} 
#endif	

