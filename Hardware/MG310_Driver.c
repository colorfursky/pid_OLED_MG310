#include "stm32f1xx_hal.h"

#include "MG310_Driver.h"

#include "tim.h"
//#include "task.h"

float MG310_CurSpeed = 0.0f;

void MG310_Rotation(Rotate_State state){
	if(state == ROTATE_FORWARD){
		AIN1_HIGH;
		AIN2_LOW;
	}else if(state == ROTATE_REVERSE){
		AIN1_LOW;
		AIN2_HIGH;
	}else if(state == ROTATE_BRAKE){
		AIN1_HIGH;
		AIN2_HIGH;
	}
}

void MG310_Cmd(MG310_Switch state){
	if(state == MG310_ON){
		STBY_HGIH;
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	}else if(state == MG310_OFF){
		STBY_LOW;
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	}
}

void MG310_SetSpeed_Percent(uint16_t speed){
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, speed);
}

void MG310_MesureCurSpeed(void){
	uint16_t count;

    // 临界区保护读-清零的原子性
//    taskENTER_CRITICAL();
	
	count = __HAL_TIM_GET_COUNTER(&htim3);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	
//    taskEXIT_CRITICAL();
	
	// 速度 = 计数 × 每计数距离 / 采样周期
	MG310_CurSpeed = (float)count * DIST_PER_COUNT / SAMPLE_PERIOD_S;
	
	return;
}

float MG310_GetCurSpeed(void)
{   
	// 直接返回全局
    return MG310_CurSpeed;
}
