#include "stm32f10x.h"                  // Device header
#include "Motor.h"
#include "sys.h"
/*电机初始化函数*/
void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//开启时钟

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;//初始化GPIO--PA4,PA5_MotorA;GPIO--PA0,PA1_MotorB;为推挽输出
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5;;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);	
}

/*限幅函数*/
void Limit(int* motoA,int* motoB)
{
	if(*motoA>PWM_MAX){*motoA=PWM_MAX;}
	if(*motoA<PWM_MIN){*motoA=PWM_MIN;}
	
	if(*motoB>PWM_MAX){*motoB=PWM_MAX;}
	if(*motoB<PWM_MIN){*motoB=PWM_MIN;}
}

/*绝对值函数*/
int GFP_abs(int p)
{
	int q;
	q=p>0?p:(-p);
	return q;
}

/*赋值函数*/
/*入口参数：PID运算完成后的最终PWM值*/
void Load(int moto1,int moto2)//moto1=-200：反转200个脉冲
{
	//1.研究正负号，对应正反转
	if(moto1>0)	
	{
		Ain1_1,Ain2_0;			//正转
	}
	else 
	{
		Ain1_0,Ain2_1; 			//反转
	}
	//2.研究PWM值
	TIM_SetCompare1(TIM1,GFP_abs(moto1));		//设置定时器1通道1的CCR
	
	if(moto2>0)	
	{
		Bin1_1,Bin2_0; 			//正转
	}
	else
	{
		Bin1_0,Bin2_1;			//反转
	}		
	TIM_SetCompare2(TIM1,GFP_abs(moto2));		//设置定时器1通道2的CCR
}


