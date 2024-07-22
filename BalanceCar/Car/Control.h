#ifndef __CONTROL_H__
#define __CONTROL_H__

extern uint8_t CNT_1;
extern float Med_Angle;//机械中值--能使得小车真正平衡住的角度。
extern float Pitch,Roll,Yaw;		//角度
extern int16_t AX, AY, AZ, GX, GY, GZ;		//AX,AY,AZ为角加速度， GX,GY,GZ为角速度
extern int MOTO1,MOTO2;

int Vertical(float Med,float Angle,float gyro_Y);
int Velocity(int Target, int Encoder_Left,int Encoder_Right);
int Turn(int gyro_Z, int RC);
void EXTI15_10_IRQHandler(void);

#endif
