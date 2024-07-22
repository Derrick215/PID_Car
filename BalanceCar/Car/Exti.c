#include "stm32f10x.h"                  // Device header
#include "sys.h"

void MPU6050_EXTI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);//����ʱ��,AFIO(�ж�����ѡ��)
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;/**��1��**///GPIO_Mode_AF_PP
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_14;//PB14����Ϊ��������
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);	
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource14);//GPIO���ⲿ�ж�ӳ������
	
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line=EXTI_Line14;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	//NVIC
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStruce;
	NVIC_InitStruce.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStruce.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruce.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruce.NVIC_IRQChannelSubPriority = 0;

	NVIC_Init(&NVIC_InitStruce);
}

