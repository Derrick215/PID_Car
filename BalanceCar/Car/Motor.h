#ifndef __MOTOR_H__
#define __MOTOR_H__

#define Ain1_1 				GPIO_SetBits(GPIOA,GPIO_Pin_4)
#define Ain1_0				GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define Ain2_1  			GPIO_SetBits(GPIOA,GPIO_Pin_5)
#define Ain2_0				GPIO_ResetBits(GPIOA,GPIO_Pin_5)

#define Bin1_1  			GPIO_SetBits(GPIOA,GPIO_Pin_0)
#define Bin1_0				GPIO_ResetBits(GPIOA,GPIO_Pin_0)
#define Bin2_1  			GPIO_SetBits(GPIOA,GPIO_Pin_1)
#define Bin2_0				GPIO_ResetBits(GPIOA,GPIO_Pin_1)

extern int PWM_MAX,PWM_MIN;

void Motor_Init(void);
void Limit(int* motoA,int* motoB);
int GFP_abs(int p);
void Load(int moto1,int moto2);


#endif
