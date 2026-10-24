#ifndef __PID_H__
#define __PID_H__

#include <stdint.h>

#define configUSE_16_BIT_TICKS 0

#if( configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffff
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL

	/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
	not need to be guarded with a critical section. */
	#define portTICK_TYPE_IS_ATOMIC 1
#endif

typedef struct{
	float Kp;		// 比例系数
	float Ki;		// 积分项的系数
	float Kd;		// 微分项的系数
	float SP;		// 用户的设定值
	
	uint32_t t_k_1;		// 上次运行PID的时间
	float err_k_1;		// 上次运行PID的误差
	float err_int_k_1;		// 上次运行的积分值
} PID_TypeDef;

// 初始化PID
void PID_Init(PID_TypeDef *PID, float Kp, float Ki, float Kd);
// 改变设定值
void PID_ChangeSP(PID_TypeDef *PID, float SP);
// 查看设定值
float PID_GetSP(PID_TypeDef *PID);
// 执行一次PID运算
float PID_Compute(volatile PID_TypeDef *PID, volatile float FB, TickType_t cur_tick);

#endif
