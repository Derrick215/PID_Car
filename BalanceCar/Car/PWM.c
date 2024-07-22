#include "stm32f10x.h"                  // Device header
#include "sys.h"
void PWM_Init_TIM1(uint16_t Per,uint16_t Psc)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1 | RCC_APB2Periph_AFIO,ENABLE);//����ʱ��
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;//��ʼ��GPIO--PA8��PA9Ϊ�����������
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8 |GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);//��ʼ����ʱ����
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=Per;
	TIM_TimeBaseInitStruct.TIM_Prescaler=Psc;
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);/*��2��*///TIM2
	
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;//��ʼ������Ƚ�
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse=0;
	TIM_OC1Init(TIM1,&TIM_OCInitStruct);
	TIM_OC2Init(TIM1,&TIM_OCInitStruct);
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);//�߼���ʱ��ר��--MOE�����ʹ��
	
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);/*��3��*///ENABLE//OC1Ԥװ�ؼĴ���ʹ��
	TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Enable);//ENABLE//OC2Ԥװ�ؼĴ���ʹ��
	TIM_ARRPreloadConfig(TIM1,ENABLE);//TIM1��ARR��Ԥװ�ؼĴ���ʹ��
	
	TIM_Cmd(TIM1,ENABLE);//����ʱ����
}