#ifndef __SOFTWARE_I2C_H__
#define __SOFTWARE_I2C_H__

#include <stdint.h>

#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB
/* ports' define	*/
#define SCL_PORT	SCL_GPIO_Port
#define SCL_PIN		SCL_Pin
#define SDA_PORT	SDA_GPIO_Port
#define SDA_PIN		SDA_Pin


#define SCL_HIGH	HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_SET)
#define SCL_LOW		HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_RESET)
#define SDA_HIGH	HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_SET)
#define SDA_LOW		HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_RESET)

#define SDA_READ	HAL_GPIO_ReadPin(SDA_PORT, SDA_Pin)

void dwt_init(void);

void Software_I2C_Start(void);
void Software_I2C_End(void);
void Software_I2C_SendByte(uint8_t Byte);
uint8_t Software_I2C_ReceiveByte(void);
void Software_I2C_Ask(uint8_t ask);
uint8_t Software_I2C_ReceiveAsk(void);

#endif
