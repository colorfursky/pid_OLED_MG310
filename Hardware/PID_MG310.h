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

void PID_MG310_Init(void);
void PID_MG310_Proc(TickType_t cur_tick);
void PID_MG310_SetSpeed(float speed_m);
void PID_MG310_ChangeSpeed(float speed_change);
float PID_MG310_GetSP(void);

#endif
