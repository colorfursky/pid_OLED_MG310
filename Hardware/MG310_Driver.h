#ifndef __MG310_DRIVER_H__
#define __MG310_DRIVER_H__

#include <stdint.h>

#define AIN1_Pin GPIO_PIN_1
#define AIN1_GPIO_Port GPIOA
#define AIN2_Pin GPIO_PIN_2
#define AIN2_GPIO_Port GPIOA
#define STBY_Pin GPIO_PIN_3
#define STBY_GPIO_Port GPIOA

#define AIN1_PIN		AIN1_Pin
#define AIN1_PORT		AIN1_GPIO_Port
#define AIN2_PIN		AIN2_Pin
#define AIN2_PORT		AIN2_GPIO_Port
#define STBY_PIN		STBY_Pin
#define STBY_PORT		STBY_GPIO_Port

#define AIN1_HIGH		HAL_GPIO_WritePin(AIN1_PORT, AIN1_PIN, GPIO_PIN_SET);
#define AIN1_LOW		HAL_GPIO_WritePin(AIN1_PORT, AIN1_PIN, GPIO_PIN_RESET);
#define AIN2_HIGH		HAL_GPIO_WritePin(AIN2_PORT, AIN2_PIN, GPIO_PIN_SET);
#define AIN2_LOW		HAL_GPIO_WritePin(AIN2_PORT, AIN2_PIN, GPIO_PIN_RESET);
#define STBY_HGIH		HAL_GPIO_WritePin(STBY_PORT, STBY_PIN, GPIO_PIN_SET);
#define STBY_LOW		HAL_GPIO_WritePin(STBY_PORT, STBY_PIN, GPIO_PIN_RESET);

#define ENCODER_PPR          13.0f      // 每相每圈脉冲数
#define GEAR_RATIO           20.0f      // 减速比
#define ENCODER_MULTIPLIER   1.0f       // 1 倍频
#define WHEEL_DIAMETER_M     0.048f     // 轮子直径(米)
#define SAMPLE_PERIOD_S      0.02f      // 采样周期 20ms

#define PULSES_PER_REV  (ENCODER_PPR * GEAR_RATIO * ENCODER_MULTIPLIER)  // = 240
#define WHEEL_CIRCUM    (3.14159f * WHEEL_DIAMETER_M)                     // ≈ 0.1508m
#define DIST_PER_COUNT  (WHEEL_CIRCUM / PULSES_PER_REV)                   // ≈ 0.000145m

#define MEASURE_DELAY		20

typedef enum{
	ROTATE_FORWARD = 0,
	ROTATE_REVERSE,
	ROTATE_BRAKE
} Rotate_State;

typedef enum{
	MG310_ON = 0,
	MG310_OFF
}	MG310_Switch;

void MG310_Rotation(Rotate_State state);
void MG310_Cmd(MG310_Switch state);
void MG310_SetSpeed_Percent(uint16_t speed);

void MG310_MesureCurSpeed(void);
float MG310_GetCurSpeed(void);

#endif
