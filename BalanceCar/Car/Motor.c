#include "stm32f10x.h"                  // Device header
#include "Motor.h"
#include "sys.h"
/*�����ʼ������*/
void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//����ʱ��

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;//��ʼ��GPIO--PA4,PA5_MotorA;GPIO--PA0,PA1_MotorB;Ϊ�������
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5;;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);	
}

/*�޷�����*/
void Limit(int* motoA,int* motoB)
{
	if(*motoA>PWM_MAX){*motoA=PWM_MAX;}
	if(*motoA<PWM_MIN){*motoA=PWM_MIN;}
	
	if(*motoB>PWM_MAX){*motoB=PWM_MAX;}
	if(*motoB<PWM_MIN){*motoB=PWM_MIN;}
}

/*����ֵ����*/
int GFP_abs(int p)
{
	int q;
	q=p>0?p:(-p);
	return q;
}

/*��ֵ����*/
/*��ڲ�����PID������ɺ������PWMֵ*/
void Load(int moto1,int moto2)//moto1=-200����ת200������
{
	//1.�о������ţ���Ӧ����ת
	if(moto1>0)	
	{
		Ain1_1,Ain2_0;			//��ת
	}
	else 
	{
		Ain1_0,Ain2_1; 			//��ת
	}
	//2.�о�PWMֵ
	TIM_SetCompare1(TIM1,GFP_abs(moto1));		//���ö�ʱ��1ͨ��1��CCR
	
	if(moto2>0)	
	{
		Bin1_1,Bin2_0; 			//��ת
	}
	else
	{
		Bin1_0,Bin2_1;			//��ת
	}		
	TIM_SetCompare2(TIM1,GFP_abs(moto2));		//���ö�ʱ��1ͨ��2��CCR
}


