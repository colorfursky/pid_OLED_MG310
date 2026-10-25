#ifndef __PID_MG310_H__
#define __PID_MG310_H__

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

void PID_MG310_Init(void);		// 初始化电机调速
void PID_MG310_Proc(TickType_t cur_tick);		// 电机调速系统进程
void PID_MG310_SetSpeed(float speed_m);		// 设定电机调速值
void PID_MG310_ChangeSpeed(float speed_change);			// 改变电机调速值
float PID_MG310_GetSP(void);			// 查看设定值

#endif
