#ifndef __CONTROL_H__
#define __CONTROL_H__

extern uint8_t CNT_1;
extern float Med_Angle;//��е��ֵ--��ʹ��С������ƽ��ס�ĽǶȡ�
extern float Pitch,Roll,Yaw;		//�Ƕ�
extern int16_t AX, AY, AZ, GX, GY, GZ;		//AX,AY,AZΪ�Ǽ��ٶȣ� GX,GY,GZΪ���ٶ�
extern int MOTO1,MOTO2;

int Vertical(float Med,float Angle,float gyro_Y);
int Velocity(int Target, int Encoder_Left,int Encoder_Right);
int Turn(int gyro_Z, int RC);
void EXTI15_10_IRQHandler(void);

#endif
