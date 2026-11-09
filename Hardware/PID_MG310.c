#include "stm32f1xx_hal.h"

#include "PID_MG310.h"
#include "PID.h"
#include "MG310_Driver.h"

//  PID调参
#define		PID_P		900.0f
#define		PID_I		800.0f
#define		PID_D		0.0f

//  速度上限设置
#define		SPEED_LIMIT		2.5f

static PID_TypeDef pid_motor;

/**
* @brief 初始化电机调速
* @param argument: Not used
* @retval None
*/
void PID_MG310_Init(void){
	PID_Init(&pid_motor, PID_P, PID_I, PID_D);		// 设置pid参数
}

/**
* @brief 电机调速系统进程
* @param argument: TickType_t cur_tick  当前时刻
* @retval None
*/
void PID_MG310_Proc(TickType_t cur_tick){
	float speed = MG310_GetCurSpeed();			// 获取当前速度
	
	float co= PID_Compute(&pid_motor, speed, cur_tick);		// 计算获取当前电机输出占空比
	
	int32_t pwm_value = (uint32_t) co;		// 强转为整型
	if(pwm_value >= 1000)		// 提高程序的健壮性
		pwm_value = 1000;
	if(pwm_value < 0)
		pwm_value = 0;
	MG310_SetSpeed_Percent((uint16_t) pwm_value);		// 设置输出比较
}

/**
* @brief 设定电机调速值
* @param argument: float speed_m  设定速度
* @retval None
*/
void PID_MG310_SetSpeed(float speed_m){
	PID_ChangeSP(&pid_motor, speed_m);		// 设置目标值
}

/**
* @brief 改变电机调速值
* @param argument: float speed_change  改变量
* @retval None
*/
void PID_MG310_ChangeSpeed(float speed_change){
	float MG310_SpeedSetting = pid_motor.SP + speed_change;			// 计算调整速度目标
	if(MG310_SpeedSetting < 0)		// 提高程序的健壮性
		MG310_SpeedSetting = 0.0f;
	else if(MG310_SpeedSetting >= SPEED_LIMIT)	// 速度上限
		MG310_SpeedSetting = SPEED_LIMIT;
	PID_ChangeSP(&pid_motor, MG310_SpeedSetting);			// 设定目标值
}

/**
* @brief 查看设定值
* @param argument: Not used
* @retval None
*/
float PID_MG310_GetSP(void){
	return PID_GetSP(&pid_motor);			// 驱动函数
}
