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
float Pitch,Roll,Yaw;		//角度
int16_t AX, AY, AZ, GX, GY, GZ;		//AX,AY,AZ为角加速度， GX,GY,GZ为角速度

int Encoder_Left, Encoder_Right;
int Vertical_Out, Velocity_Out, Turn_Out;

float
	Speed_Target = 0,
	Speed_Turn = 0,
	Med_Angle = 0.5;//机械中值--能使得小车真正平衡住的角度。
	
float 
	Vertical_Kp = 400, 		//330
	Vertical_Kd = 1.25;		//1.05
float 
	Velocity_Kp = 0.35,
	Velocity_Ki = 0.35/200;
float 
	Turn_Kp = 40,
	Turn_Kd = 1.1;
//转向环KP,KD

int MOTO1,MOTO2;								//电机装载变量

/*
0x00 :刹车
0x01 :前进
0x05 :后退
0x03 :左转
0x07 :右转
*/
void EXTI15_10_IRQHandler(void)
{
	int PWM_Out;
	if(EXTI_GetFlagStatus(EXTI_Line14) == SET)
	{
		//1.采集编码器数据&MPU6050角度信息。
		Encoder_Left = Read_Speed(3);			//电机是相对安装，为了编码器输出极性一致。
		Encoder_Right = Read_Speed(4);
		MPU6050_DMP_Get_Data(&Pitch, &Roll, &Yaw);			//角度
		MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);		//陀螺仪，加速度
		//2.将数据压入闭环控制中，计算出控制输出量。
		
		//前后
		if (Fore == 0 && Back == 0) {Speed_Target = 0;}    	//为接收到前进后退指令————>速度清零，稳在原地
		if (Fore == 1) {Speed_Target +=1;}
		if (Back == 1) {Speed_Target -=1;}
		Speed_Target = Speed_Target > Speed_Y ? Speed_Y:(Speed_Target < -Speed_Y ? -Speed_Y:Speed_Target);//限幅
		
		//左右
		if (Left == 0 && Right == 0) {Speed_Turn = 0;} 
		if (Left == 1) {Speed_Turn -=7;}
		if (Right == 1) {Speed_Turn +=7;}
		Speed_Turn = Speed_Turn > Speed_Z ? Speed_Z:(Speed_Turn < -Speed_Z ? -Speed_Z:Speed_Turn);//限幅
		
		//转向约束
		if (Left == 0 && Right == 0) {Turn_Kd = 1.1;}			//若无转向指令，开启转向约束
		else if (Left == 1 || Right == 1) {Turn_Kd = 0;}		//若无转向指令，关闭转向约束
		
		
		Velocity_Out = Velocity(Speed_Target, Encoder_Left, Encoder_Right);
		Vertical_Out=Vertical(Med_Angle,Pitch,GY);
		Turn_Out = Turn(GZ, Speed_Turn);
		PWM_Out=Vertical_Out + Vertical_Kp * Velocity_Out;
		//3.把控制输出量加载到电机上，完成最终的的控制。
		MOTO1 = PWM_Out - Turn_Out;
		MOTO2 = PWM_Out + Turn_Out;
		Limit(&MOTO1,&MOTO2);//PWM限幅			
		Load(MOTO1,MOTO2);//加载到电机上。
		
		//摔倒保护
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
直立环PD控制器：Kp*Ek+Kd*Ek_D

入口：期望角度Med、真实角度Angle、真实角速度gyro_Y
出口：直立环输出
*********************/
int Vertical(float Med,float Angle,float gyro_Y)
{
	int PWM_out;
	
	PWM_out=Vertical_Kp * (Angle-Med) + Vertical_Kd * (gyro_Y - 0);//【1】
	return PWM_out;
}



/*********************
速度环PI：Kp*Ek+Ki*Ek_S
*********************/
int Velocity(int Target, int Encoder_Left,int Encoder_Right)
{
	static int PWM_out, Encoder_Err, Encoder_S, Encoder_Low_out, Encoder_Low_out_Last;
	
	float a = 0.7;
	//1.计算速度偏差
	Encoder_Err = ((Encoder_Left + Encoder_Right) - Target);
	//2.对速度偏差进行低通滤波
	//low_out=(1-a)*Ek+a*low_out_last;
	Encoder_Low_out = a * Encoder_Err + (1 - a) * Encoder_Low_out_Last;		//使得波形更加平滑，滤除高频干扰，防止速度突变。
	Encoder_Low_out_Last = Encoder_Low_out;							//防止速度过大的影响直立环的正常工作。
	//3.对速度偏差积分，积分出位移
	Encoder_S += Encoder_Low_out;
	//4.积分限幅
	Encoder_S=Encoder_S>10000?10000:(Encoder_S<(-10000)?(-10000):Encoder_S);
	//5.速度环控制输出计算
	
	if(Stop == 1) {Encoder_S=0,Stop=0;}//当检测到摔倒时，就积分清零
	
	PWM_out = Velocity_Kp * Encoder_Low_out + Velocity_Ki * Encoder_S;
	return PWM_out;
}

/*********************
转向环：系数*Z轴角速度
*********************/
int Turn(int gyro_Z, int RC)
{
	int PWM_out;
	//Kd针对的是转向的约束，但Kp针对的是遥控的转向
	PWM_out= Turn_Kd *gyro_Z + Turn_Kp * RC;
	return PWM_out;
}

