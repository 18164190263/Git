#include "pid.h"

PID_Typedef Current;

void PID_Init(void)
{
    Current.SetSpeed = 18.5;
    Current.ActualSpeed = 0.0;
    Current.err = 0.0;
    Current.err_last = 0.0;
    Current.out = 0.0;
    Current.integral = 0.0;
    Current.Umax = 1;
    Current.Umin = 0.75;
    Current.Kp = 0.0087;
    Current.Ki = 0.0025;
    Current.Kd = 0.07;

}

////////////////////////////////////////
//      ???PID??                 //
//      ????????????      //
//      ??????????          //
////////////////////////////////////////
float PID_realize(float ActualSpeed)
{
	Current.ActualSpeed=ActualSpeed;
    Current.err=Current.SetSpeed-Current.ActualSpeed;	

//??????????????????????????????????????
    if(Current.out >= Current.Umax )
	{
		if(Current.err <= 0)
		{
			Current.integral += Current.err ;
		}
	}

//??????????????????????????????????????
	else if(Current.out <= Current.Umin )
	{
		if(Current.err >= 0)
		{
			Current.integral += Current.err ;
		}
	}

 //???????????????????????????   
	else
	{
		Current.integral += Current.err ;
	}

    Current.out=Current.Kp*Current.err+Current.Ki*Current.integral+Current.Kd*(Current.err-Current.err_last);
	if(Current.out<=Current.Umin)Current.out=Current.Umin;
	if(Current.out>=Current.Umax)Current.out=Current.Umax;
    Current.err_last=Current.err;

    return Current.out;


}





