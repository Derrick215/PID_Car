#include "stm32f10x.h"                  // Device header
#include "Encoder.h"
#include "Exti.h"
#include "Motor.h"
#include "PWM.h"
#include "Control.h"
#include "Motor.h"
#include "inv_mpu.h"
#include "mpu6050.h"
#include "sys.h"

#define Speed_Y	30
#define Speed_Z	35

uint8_t Stop = 0;
uint8_t CNT_1;
uint8_t i;
float Pitch,Roll,Yaw;		//�Ƕ�
int16_t AX, AY, AZ, GX, GY, GZ;		//AX,AY,AZΪ�Ǽ��ٶȣ� GX,GY,GZΪ���ٶ�

int Encoder_Left, Encoder_Right;
int Vertical_Out, Velocity_Out, Turn_Out;

float
	Speed_Target = 0,
	Speed_Turn = 0,
	Med_Angle = 0.5;//��е��ֵ--��ʹ��С������ƽ��ס�ĽǶȡ�
	
float 
	Vertical_Kp = 400, 		//330
	Vertical_Kd = 1.25;		//1.05
float 
	Velocity_Kp = 0.35,
	Velocity_Ki = 0.35/200;
float 
	Turn_Kp = 40,
	Turn_Kd = 1.1;
//ת��KP,KD

int MOTO1,MOTO2;								//���װ�ر���

/*
0x00 :ɲ��
0x01 :ǰ��
0x05 :����
0x03 :��ת
0x07 :��ת
*/
void EXTI15_10_IRQHandler(void)
{
	int PWM_Out;
	if(EXTI_GetFlagStatus(EXTI_Line14) == SET)
	{
		//1.�ɼ�����������&MPU6050�Ƕ���Ϣ��
		Encoder_Left = Read_Speed(3);			//�������԰�װ��Ϊ�˱������������һ�¡�
		Encoder_Right = Read_Speed(4);
		MPU6050_DMP_Get_Data(&Pitch, &Roll, &Yaw);			//�Ƕ�
		MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);		//�����ǣ����ٶ�
		//2.������ѹ��ջ������У�����������������
		
		//ǰ��
		if (Fore == 0 && Back == 0) {Speed_Target = 0;}    	//Ϊ���յ�ǰ������ָ�������>�ٶ����㣬����ԭ��
		if (Fore == 1) {Speed_Target +=1;}
		if (Back == 1) {Speed_Target -=1;}
		Speed_Target = Speed_Target > Speed_Y ? Speed_Y:(Speed_Target < -Speed_Y ? -Speed_Y:Speed_Target);//�޷�
		
		//����
		if (Left == 0 && Right == 0) {Speed_Turn = 0;} 
		if (Left == 1) {Speed_Turn -=7;}
		if (Right == 1) {Speed_Turn +=7;}
		Speed_Turn = Speed_Turn > Speed_Z ? Speed_Z:(Speed_Turn < -Speed_Z ? -Speed_Z:Speed_Turn);//�޷�
		
		//ת��Լ��
		if (Left == 0 && Right == 0) {Turn_Kd = 1.1;}			//����ת��ָ�����ת��Լ��
		else if (Left == 1 || Right == 1) {Turn_Kd = 0;}		//����ת��ָ��ر�ת��Լ��
		
		
		Velocity_Out = Velocity(Speed_Target, Encoder_Left, Encoder_Right);
		Vertical_Out=Vertical(Med_Angle,Pitch,GY);
		Turn_Out = Turn(GZ, Speed_Turn);
		PWM_Out=Vertical_Out + Vertical_Kp * Velocity_Out;
		//3.�ѿ�����������ص�����ϣ�������յĵĿ��ơ�
		MOTO1 = PWM_Out - Turn_Out;
		MOTO2 = PWM_Out + Turn_Out;
		Limit(&MOTO1,&MOTO2);//PWM�޷�			
		Load(MOTO1,MOTO2);//���ص�����ϡ�
		
		//ˤ������
		if(Pitch > 40 || Pitch < -40)
		{
			Load(0, 0);
			Fore = 0, Back = 0, Left = 0, Right = 0;
			Stop = 1;
		}
		else
		{
			Load(MOTO1,MOTO2);
		}
		
		i++; 
		if (i >= 100)
		{
			CNT_1 ++;
			i = 0;
		}
		if (CNT_1 >= 10)
		{
			CNT_1 =0;
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line14);
}



/*********************
ֱ����PD��������Kp*Ek+Kd*Ek_D

��ڣ������Ƕ�Med����ʵ�Ƕ�Angle����ʵ���ٶ�gyro_Y
���ڣ�ֱ�������
*********************/
int Vertical(float Med,float Angle,float gyro_Y)
{
	int PWM_out;
	
	PWM_out=Vertical_Kp * (Angle-Med) + Vertical_Kd * (gyro_Y - 0);//��1��
	return PWM_out;
}



/*********************
�ٶȻ�PI��Kp*Ek+Ki*Ek_S
*********************/
int Velocity(int Target, int Encoder_Left,int Encoder_Right)
{
	static int PWM_out, Encoder_Err, Encoder_S, Encoder_Low_out, Encoder_Low_out_Last;
	
	float a = 0.7;
	//1.�����ٶ�ƫ��
	Encoder_Err = ((Encoder_Left + Encoder_Right) - Target);
	//2.���ٶ�ƫ����е�ͨ�˲�
	//low_out=(1-a)*Ek+a*low_out_last;
	Encoder_Low_out = a * Encoder_Err + (1 - a) * Encoder_Low_out_Last;		//ʹ�ò��θ���ƽ�����˳���Ƶ���ţ���ֹ�ٶ�ͻ�䡣
	Encoder_Low_out_Last = Encoder_Low_out;							//��ֹ�ٶȹ����Ӱ��ֱ����������������
	//3.���ٶ�ƫ����֣����ֳ�λ��
	Encoder_S += Encoder_Low_out;
	//4.�����޷�
	Encoder_S=Encoder_S>10000?10000:(Encoder_S<(-10000)?(-10000):Encoder_S);
	//5.�ٶȻ������������
	
	if(Stop == 1) {Encoder_S=0,Stop=0;}//����⵽ˤ��ʱ���ͻ�������
	
	PWM_out = Velocity_Kp * Encoder_Low_out + Velocity_Ki * Encoder_S;
	return PWM_out;
}

/*********************
ת�򻷣�ϵ��*Z����ٶ�
*********************/
int Turn(int gyro_Z, int RC)
{
	int PWM_out;
	//Kd��Ե���ת���Լ������Kp��Ե���ң�ص�ת��
	PWM_out= Turn_Kd *gyro_Z + Turn_Kp * RC;
	return PWM_out;
}

