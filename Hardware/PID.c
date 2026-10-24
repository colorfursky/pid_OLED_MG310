#include "stm32f1xx_hal.h"

#include "PID.h"

// 初始化PID
void PID_Init(PID_TypeDef *PID, float Kp, float Ki, float Kd){
	PID->Kp = Kp;
	PID->Ki = Ki;
	PID->Kd = Kd;
	PID->SP = 0.0f;
	
	PID->t_k_1 = 0;
	PID->err_k_1 = 0.0f;
	PID->err_int_k_1 = 0.0f;
}

// 改变设定值
void PID_ChangeSP(PID_TypeDef *PID, float SP){
	PID->SP = SP;
}

// 查看设定值
float PID_GetSP(PID_TypeDef *PID){
	return PID->SP;
}

// 执行一次PID运算
float PID_Compute(volatile PID_TypeDef *PID, volatile float FB, TickType_t cur_tick){
	// err = SP - FB
	volatile float err =(float) PID->SP - FB;
	
	volatile float deltaT = (cur_tick - PID->t_k_1)* 1.0e-3f;
	volatile float err_dev = (err - PID->err_k_1) / deltaT;
	volatile float err_int = PID->err_int_k_1 + (err + PID->err_k_1) * deltaT * 0.5f;
	
	volatile float COp, COi, COd, CO;
	COp = err * PID->Kp;
	COi = err_int * PID->Ki;
	COd = err_dev * PID->Kd;
	CO = COp + COi + COd;
	
	// update
	PID->t_k_1 = cur_tick;
	PID->err_k_1 = err;
	PID->err_int_k_1 = err_int;
	return CO;
}
