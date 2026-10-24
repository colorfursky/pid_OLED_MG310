#include "stm32f1xx_hal.h"

#include "PID_MG310.h"
#include "PID.h"
#include "MG310_Driver.h"

static PID_TypeDef pid_motor;

// 初始化电机调速
void PID_MG310_Init(void){
	PID_Init(&pid_motor, 1200.0f, 650.0f, 10.0f);
}

// 电机调速系统进程
void PID_MG310_Proc(TickType_t cur_tick){
	float speed = MG310_GetCurSpeed();
	
	float co= PID_Compute(&pid_motor, speed, cur_tick);
	
	int32_t pwm_value = (uint32_t) co;
	if(pwm_value >= 1000)
		pwm_value = 1000;
	if(pwm_value < 0)
		pwm_value = 0;
	MG310_SetSpeed_Percent((uint16_t) pwm_value);
}

void PID_MG310_SetSpeed(float speed_m){
	PID_ChangeSP(&pid_motor, speed_m);
}

void PID_MG310_ChangeSpeed(float speed_change){
	float MG310_SpeedSetting = pid_motor.SP + speed_change;
	if(MG310_SpeedSetting < 0)
		MG310_SpeedSetting = 0.0f;
	else if(MG310_SpeedSetting >= 2.0f)
		MG310_SpeedSetting = 2.0f;
	PID_ChangeSP(&pid_motor, MG310_SpeedSetting);
}

float PID_MG310_GetSP(void){
	return PID_GetSP(&pid_motor);
}
