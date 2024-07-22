#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "OLED.h"
#include "mpu6050.h"
#include "Serial.h"
#include "Encoder.h"
#include "PWM.h"
#include "Exti.h"
#include "Motor.h"
#include "Control.h"
#include "inv_mpu.h"
#include "sys.h"

int PWM_MAX = 7200,PWM_MIN = -7200;

uint8_t RxData;
uint8_t res;
uint8_t ID;

int main(void)
{
	delay_init();
	OLED_Init();
	USART2_Init(4800);
	
	MPU6050_Init();
	
	L_Encoder_TIM3_Init();
	R_Encoder_TIM4_Init();
	Motor_Init();
	PWM_Init_TIM1(7200 - 1, 1-1);
	OLED_ShowString(1,5,".");
	OLED_ShowString(2,5,".");
	OLED_ShowString(3,5,".");
	
	MPU6050_DMP_Init();
	MPU6050_EXTI_Init();
	while (1)
	{
		OLED_ShowSignedNum(1,1,Pitch,3);
		OLED_ShowNum(1,6,(int8_t)(Pitch*100)%100,2);
		OLED_ShowSignedNum(2,1,Roll,3);
		OLED_ShowNum(2,6,(int8_t)(Roll*100)%100,2);
		OLED_ShowSignedNum(3,1,Yaw,3);
		OLED_ShowNum(3,6,(int8_t)(Yaw*100)%100,2);		
		OLED_ShowNum(4, 1, CNT_1, 2);
		
		OLED_ShowHexNum(4,5,RxData,2);
	}
}


/*
0x00 :刹车
0x01 :前进
0x05 :后退
0x03 :左转
0x07 :右转
*/

uint8_t Fore, Back, Left, Right;

void USART2_IRQHandler(void) //串口2中断服务函数
{
	if(USART_GetITStatus(USART2,USART_IT_RXNE)) 
     {
         RxData= USART_ReceiveData(USART2); 
         USART_SendData(USART2,RxData);   
		 if (RxData == 0x00) {Fore = 0, Back = 0, Left = 0, Right = 0;}
		 else if (RxData == 0x01) {Fore = 1, Back = 0, Left = 0, Right = 0;}
		 else if (RxData == 0x05) {Fore = 0, Back = 1, Left = 0, Right = 0;}
		 else if (RxData == 0x03) {Fore = 0, Back = 0, Left = 1, Right = 0;}
		 else if (RxData == 0x07) {Fore = 0, Back = 0, Left = 0, Right = 1;}
		 else {Fore		 = 0, Back = 0, Left = 0, Right = 0;}
		 
      }
	 USART_ClearITPendingBit(USART2, USART_IT_RXNE);
}


