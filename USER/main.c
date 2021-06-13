#include "usart.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "spwm.h"
#include "key.h"
#include "adc.h"
#include "pid.h"
#include "oled.h"
#include "hlw8032.h"
#include "pid.h"
//#define Voltage_Set 2.93776025
#define Voltage_Set 2.958624839



float ACVotage,ACCurrent,ActivePower,ApparentPower,PowerFactor,ElectricEnergy;
u8 Pre_Data_Updata_Reg;
u16 UpdataCount;
u32 PFCount,PF_OneDegree;

//OLED��ʾ
u16 Voltage_Int,Modulation_Int;//����
float Voltage_Num,Modulation_Degree;//С��



 int main(void)
 {
	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();	    	 //��ʱ������ʼ��
	uart_init(115200);
	uart2_init(4800);	 	//���ڳ�ʼ��Ϊ4800
	LED_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ�
	  
	key_init();
	PID_Init();
	spwm_init(spot,0);
	
	 
//	 tim2_init(10000-1,7200-1);

	 OLED_Init();
	 OLED_ShowChinese2424(0,0,0,16);//��ѹ��
	 OLED_ShowChinese2424(18,0,1,16);
	 OLED_ShowChar(34,0,':',16,1);

	 OLED_ShowChinese2424(0,20,4,16);//���ƶȣ�
	 OLED_ShowChinese2424(18,20,5,16);
	 OLED_ShowChinese2424(34,20,6,16);
	 OLED_ShowChar(50,20,':',16,1);
	 
	 OLED_Refresh_Gram();
	 
	 while(1)
	 {
			ACVotage=(Voltage_Parameter_Reg*Voltage_Set/Voltage_Reg);//��ѹ��Чֵ

			if(ACVotage<=2) ACVotage=0;
			
			ACCurrent=Current_Parameter_Reg*1.0/Current_Reg;//������Чֵ
			if(ACCurrent<0.1)//�޵�ѹʱ������Ʈ��
			{
				ACCurrent=0;
				ApparentPower=0;
				ActivePower=0;
				PowerFactor=0;
			}
			if(ACVotage>2&&ACCurrent>0.1)//����ʱ������Ʈ��
			{
				ApparentPower=ACVotage*ACCurrent;//���ڹ���
				ActivePower=Power_Parameter_Reg*3.006/Power_Reg;//�й�����
				PowerFactor=ActivePower/ApparentPower;//��������
			}
//			printf ("��ѹ��Чֵ��%f\r\n",ACVotage);
//			printf ("������Чֵ��%f\r\n",ACCurrent);
//			printf ("���ڹ��ʣ�%f\r\n",ApparentPower);
//			printf ("�й����ʣ�%f\r\n",ActivePower);
//			printf ("����������%f\r\n",PowerFactor);
			if(Pre_Data_Updata_Reg!=(Data_Updata_Reg&0x80))
			{
				UpdataCount++;
				Pre_Data_Updata_Reg=Data_Updata_Reg&0x80;
			}
			PFCount=UpdataCount*65536+PF_Reg;//�����źŸ���
			PF_OneDegree=1000000000/Power_Parameter_Reg;
			PF_OneDegree=PF_OneDegree*3600/3.006;//1�ȵ��Ӧ�����������
			ElectricEnergy=(float)PFCount/(float)PF_OneDegree;//����
//			printf ("���ܣ�%f\r\n",ElectricEnergy);
//			delay_ms(500);	

//			printf("%f\n",k);
			printf("%f,%f,%f,%f\t\n",k,Current.SetSpeed,Current.ActualSpeed,Current.out);
	
//��ʾ��ѹ�Ĵ�������ֵ			 
			OLED_ShowNum(67,0,Voltage_Reg,5,16);//С������
			OLED_Refresh_Gram();

			
//���㽻����Ч��ѹֵ������OLED��ʾ
			Voltage_Num = ACVotage;
			Voltage_Int = Voltage_Num;
			OLED_ShowNum(45,0,Voltage_Int,2,16);//��������
			Voltage_Num -= Voltage_Int;
			Voltage_Num *= 1000;

//������ƶȣ�����OLED��ʾ
			Modulation_Degree = k;
			Modulation_Int = Modulation_Degree;
			OLED_ShowNum(60,20,Modulation_Int,2,16);//��������
			Modulation_Degree -= Modulation_Int;
			Modulation_Degree *= 100;

////��Ч��ѹ��ʾ����������
//			OLED_ShowChar(60,0,'0'-2,16,1);
//			OLED_ShowNum(67,0,Voltage_Num,3,16);//С������
//			OLED_ShowChar(100,0,'V',16,1);
////���ƶ���ʾ
//			OLED_ShowChar(76,20,'0'-2,16,1);
//			OLED_ShowNum(86,20,Modulation_Degree,2,16);//С������			
//			OLED_Refresh_Gram();
		 
	 }
	 
 }

