#include "stm32f1xx_hal.h"

#include "Software_I2C.h"

#define DELAY 0

// 初始化 DWT
void dwt_init(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

// 微秒级忙等
void delay_us(uint32_t us) {
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000);
    while ((DWT->CYCCNT - start) < ticks);
}

void W_SCL(uint8_t BitValue){
	if(BitValue == 1){
		SCL_HIGH;
	}else{
		SCL_LOW;
	}
	delay_us(DELAY);
}

void W_SDA(uint8_t BitValue){
	if(BitValue == 1){
		SDA_HIGH;
	}else{
		SDA_LOW;
	}
	delay_us(DELAY);
}

void Software_I2C_SendByte(uint8_t Byte){
	uint8_t i;
	for(i = 0; i < 8; i++){
		if((Byte & 0x80) != 0)   // 取最高位
			W_SDA(1);
		else
			W_SDA(0);
			W_SCL(1);
			W_SCL(0);
			Byte <<= 1;              // 左移
	}
}

uint8_t Software_I2C_ReceiveByte(void){
	uint8_t data = 0x00, i;
	W_SDA(1);
	for(i=0; i<8; i++){
		W_SCL(1);
		data <<= 1;
		if(SDA_READ == GPIO_PIN_SET){
			data |= 0x01;
		}
		W_SCL(0);
	}
	return data;
}

void Software_I2C_Ask(uint8_t ask){
	if(ask == 0){
		W_SDA(0);
	}else{
		W_SDA(1);
	}
	W_SCL(1);
	W_SCL(0);
}

uint8_t Software_I2C_ReceiveAsk(void){
	uint8_t ask;
	W_SDA(1);
	W_SCL(1);
	ask = SDA_READ;
	W_SCL(0);
	return ask;
}

void Software_I2C_Start(void){
	
	W_SDA(1);
	W_SCL(1);
	
	W_SDA(0);
	W_SCL(0);
}

void Software_I2C_End(void){
	W_SDA(0);
	W_SCL(0);
	W_SCL(1);
	W_SDA(1);
}
