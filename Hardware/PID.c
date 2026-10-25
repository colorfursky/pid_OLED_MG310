#include "stm32f1xx_hal.h"

#include "PID.h"

/**
* @brief 初始化PID
* @param argument: PID_TypeDef *PID		PID句柄
* @param argument: float Kp		比例部分
* @param argument: float Ki   积分部分
* @param argument: float Kd   微分部分
* @retval None
*/
void PID_Init(PID_TypeDef *PID, float Kp, float Ki, float Kd){
	PID->Kp = Kp;
	PID->Ki = Ki;
	PID->Kd = Kd;
	PID->SP = 0.0f;
	
	PID->t_k_1 = 0;
	PID->err_k_1 = 0.0f;
	PID->err_int_k_1 = 0.0f;
}

/**
* @brief 改变设定值
* @param argument: PID_TypeDef *PID		PID句柄
* @param argument: float SP		设定速度 m/s
* @retval None
*/
void PID_ChangeSP(PID_TypeDef *PID, float SP){
	PID->SP = SP;
}

/**
* @brief 查看设定值
* @param argument: PID_TypeDef *PID		PID句柄
* @retval None
*/
float PID_GetSP(PID_TypeDef *PID){
	return PID->SP;
}

/**
* @brief 执行一次PID运算
* @param argument: PID_TypeDef *PID		PID句柄
* @param argument: float FB		反馈值
* @param argument: TickType_t cur_tick   当前所处时刻
* @retval None
*/
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
