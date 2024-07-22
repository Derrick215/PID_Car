#include "stm32f10x.h"                  // Device header                
#include "sys.h"

void L_Encoder_TIM3_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//L_Encoder
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;			//初始化GPIO
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;//L:PA6 PA7||R:PB6 PB7
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;				//初始化定时器
	TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1; //无用，被编码器接管
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;//无用，被编码器接管
	TIM_TimeBaseInitStruct.TIM_Period=65536-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler=1-1;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
	
	TIM_ICInitTypeDef TIM_ICInitStruct;				//初始化输入捕获
	TIM_ICStructInit(&TIM_ICInitStruct);			//回复缺省配置
	
	TIM_ICInitStruct.TIM_Channel=TIM_Channel_1;//L_A相
	TIM_ICInitStruct.TIM_ICFilter=10;
	TIM_ICInitStruct.TIM_ICPrescaler=TIM_ICPolarity_Rising;
	TIM_ICInit(TIM3,&TIM_ICInitStruct);
	TIM_ICInitStruct.TIM_Channel=TIM_Channel_2;//L_B相
	TIM_ICInitStruct.TIM_ICFilter=10;
	TIM_ICInitStruct.TIM_ICPrescaler=TIM_ICPolarity_Rising;
	TIM_ICInit(TIM3,&TIM_ICInitStruct);
	
	TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);		//配置编码器模式

	TIM_ClearFlag(TIM3,TIM_FLAG_Update);			//清楚溢出更新中断标志位
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);		//设置溢出更新中断标志位
	
	TIM_SetCounter(TIM3,0);							//编码器计数器设置为0
	TIM_Cmd(TIM3,ENABLE);							//开启定时器
}

void R_Encoder_TIM4_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);//R_Encoder
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;//L:PA6 PA7||R:PB6 PB7
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1; 
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=65536-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler=1-1;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStruct);
	
	TIM_ICInitTypeDef TIM_ICInitStruct;
	TIM_ICStructInit(&TIM_ICInitStruct);
	
	TIM_ICInitStruct.TIM_Channel=TIM_Channel_1;//R_A相
	TIM_ICInitStruct.TIM_ICFilter=10;
	TIM_ICInitStruct.TIM_ICPrescaler=TIM_ICPolarity_Rising;				//不反向
	TIM_ICInit(TIM4,&TIM_ICInitStruct);
	TIM_ICInitStruct.TIM_Channel=TIM_Channel_2;//R_B相
	TIM_ICInitStruct.TIM_ICFilter=10;
	TIM_ICInitStruct.TIM_ICPrescaler=TIM_ICPolarity_Rising;
	TIM_ICInit(TIM4,&TIM_ICInitStruct);
	
	TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);

	TIM_ClearFlag(TIM4,TIM_FLAG_Update);			
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);		
	
	TIM_SetCounter(TIM4,0);
	TIM_Cmd(TIM4,ENABLE);
}

/**********************
编码器速度读取函数
入口参数：定时器
**********************/
int Read_Speed(int TIMx)
{
	int value_1;
	switch(TIMx)
	{
		case 3:value_1=(short)TIM_GetCounter(TIM3);TIM_SetCounter(TIM3,0);break;//定时器3，读取编码器并保存，并清零编码器计时器
		case 4:value_1=(short)TIM_GetCounter(TIM4);TIM_SetCounter(TIM4,0);break;
		default:value_1=0;
	}
	return value_1;
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=0)
	{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}
}
	
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=0)
	{
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	}
}

