#ifndef __ENCODER_H__
#define __ENCODER_H__

void L_Encoder_TIM3_Init(void);
void R_Encoder_TIM4_Init(void);
int Read_Speed(int TIMx);
	

#endif
