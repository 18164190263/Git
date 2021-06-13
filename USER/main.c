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

//OLED显示
u16 Voltage_Int,Modulation_Int;//整数
float Voltage_Num,Modulation_Degree;//小数



 int main(void)
 {
	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();	    	 //延时函数初始化
	uart_init(115200);
	uart2_init(4800);	 	//串口初始化为4800
	LED_Init();		  	//初始化与LED连接的硬件接口
	  
	key_init();
	PID_Init();
	spwm_init(spot,0);
	
	 
//	 tim2_init(10000-1,7200-1);

	 OLED_Init();
	 OLED_ShowChinese2424(0,0,0,16);//电压：
	 OLED_ShowChinese2424(18,0,1,16);
	 OLED_ShowChar(34,0,':',16,1);

	 OLED_ShowChinese2424(0,20,4,16);//调制度：
	 OLED_ShowChinese2424(18,20,5,16);
	 OLED_ShowChinese2424(34,20,6,16);
	 OLED_ShowChar(50,20,':',16,1);
	 
	 OLED_Refresh_Gram();
	 
	 while(1)
	 {
			ACVotage=(Voltage_Parameter_Reg*Voltage_Set/Voltage_Reg);//电压有效值

			if(ACVotage<=2) ACVotage=0;
			
			ACCurrent=Current_Parameter_Reg*1.0/Current_Reg;//电流有效值
			if(ACCurrent<0.1)//无电压时电流有飘移
			{
				ACCurrent=0;
				ApparentPower=0;
				ActivePower=0;
				PowerFactor=0;
			}
			if(ACVotage>2&&ACCurrent>0.1)//空载时电流有飘移
			{
				ApparentPower=ACVotage*ACCurrent;//视在功率
				ActivePower=Power_Parameter_Reg*3.006/Power_Reg;//有功功率
				PowerFactor=ActivePower/ApparentPower;//功率因数
			}
//			printf ("电压有效值：%f\r\n",ACVotage);
//			printf ("电流有效值：%f\r\n",ACCurrent);
//			printf ("视在功率：%f\r\n",ApparentPower);
//			printf ("有功功率：%f\r\n",ActivePower);
//			printf ("功率因数：%f\r\n",PowerFactor);
			if(Pre_Data_Updata_Reg!=(Data_Updata_Reg&0x80))
			{
				UpdataCount++;
				Pre_Data_Updata_Reg=Data_Updata_Reg&0x80;
			}
			PFCount=UpdataCount*65536+PF_Reg;//脉冲信号个数
			PF_OneDegree=1000000000/Power_Parameter_Reg;
			PF_OneDegree=PF_OneDegree*3600/3.006;//1度电对应的脉冲个数；
			ElectricEnergy=(float)PFCount/(float)PF_OneDegree;//电能
//			printf ("电能：%f\r\n",ElectricEnergy);
//			delay_ms(500);	

//			printf("%f\n",k);
			printf("%f,%f,%f,%f\t\n",k,Current.SetSpeed,Current.ActualSpeed,Current.out);
	
//显示电压寄存器的数值			 
			OLED_ShowNum(67,0,Voltage_Reg,5,16);//小数部分
			OLED_Refresh_Gram();

			
//计算交流有效电压值，用于OLED显示
			Voltage_Num = ACVotage;
			Voltage_Int = Voltage_Num;
			OLED_ShowNum(45,0,Voltage_Int,2,16);//整数部分
			Voltage_Num -= Voltage_Int;
			Voltage_Num *= 1000;

//计算调制度，用于OLED显示
			Modulation_Degree = k;
			Modulation_Int = Modulation_Degree;
			OLED_ShowNum(60,20,Modulation_Int,2,16);//整数部分
			Modulation_Degree -= Modulation_Int;
			Modulation_Degree *= 100;

////有效电压显示（均方根）
//			OLED_ShowChar(60,0,'0'-2,16,1);
//			OLED_ShowNum(67,0,Voltage_Num,3,16);//小数部分
//			OLED_ShowChar(100,0,'V',16,1);
////调制度显示
//			OLED_ShowChar(76,20,'0'-2,16,1);
//			OLED_ShowNum(86,20,Modulation_Degree,2,16);//小数部分			
//			OLED_Refresh_Gram();
		 
	 }
	 
 }

