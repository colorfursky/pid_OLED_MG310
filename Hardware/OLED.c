#include "stm32f1xx_hal.h"

#include "Software_I2C.h"
#include "OLED.h"
#include "OLED_Fonts.h"

#include <string.h>

void OLED_Init(void){
	dwt_init();
	
	OLED_SendCommand(0xAE);
	
	OLED_SendCommand(0xD5);
	OLED_SendCommand(0x90);
	
	OLED_SendCommand(0xA8);
	OLED_SendCommand(0x3F);
	
	OLED_SendCommand(0xD3);
	OLED_SendCommand(0x00);
	
	OLED_SendCommand(0x40);
	
	OLED_SendCommand(0xA1);
	
	OLED_SendCommand(0xC8);
	
	OLED_SendCommand(0xDA);
	OLED_SendCommand(0x12);
	
	OLED_SendCommand(0x81);
	OLED_SendCommand(0xB0);
	
	OLED_SendCommand(0xD9);
	OLED_SendCommand(0x22);
	
	OLED_SendCommand(0xDB);
	OLED_SendCommand(0x30);
	
	OLED_SendCommand(0xA4);
	
	OLED_SendCommand(0xA6);
	
	OLED_SendCommand(0x8D);
	OLED_SendCommand(0x14);
	
	OLED_SendCommand(0xAF);
}

void OLED_SetCursor(uint8_t X, uint8_t Page){
	OLED_SendCommand(0x00 | (X & 0x0F));
	OLED_SendCommand(0x10 | ((X & 0xF0) >> 4));
	
	OLED_SendCommand(0xB0 | Page);
}

void OLED_SendCommand(uint8_t Command){
	Software_I2C_Start();
	Software_I2C_SendByte(0x78);
	Software_I2C_ReceiveAsk();
	Software_I2C_SendByte(0x00);		// Control Byte
	Software_I2C_ReceiveAsk();
	Software_I2C_SendByte(Command);
	Software_I2C_ReceiveAsk();
	Software_I2C_End();
}

void OLED_SendData(uint8_t Data){
	Software_I2C_Start();
	Software_I2C_SendByte(0x78);
	Software_I2C_ReceiveAsk();
	Software_I2C_SendByte(0x40);		// Control Byte
	Software_I2C_ReceiveAsk();
	Software_I2C_SendByte(Data);
	Software_I2C_ReceiveAsk();
	Software_I2C_End();
}

void OLED_Clear(void){
	for(uint8_t j = 0; j<8; j++){
		OLED_SetCursor(0, j);
		for(uint8_t i = 0; i < 128; i++){
			OLED_SendData(0x00);
		}
	}
}

void OLED_ShowChar(uint8_t X, uint8_t Page, char Char, uint8_t FontSize){
	if(FontSize == 6){
		OLED_SetCursor(X, Page);
		for(uint8_t i=0; i<6; i++)
			OLED_SendData(OLED_F6x8[Char - ' '][i]);
	}
	else if(FontSize == 8){
		OLED_SetCursor(X, Page);
		for(uint8_t i=0; i<8; i++)
			OLED_SendData(OLED_F8x16[Char - ' '][i]);
		
		OLED_SetCursor(X, Page+1);
		for(uint8_t i=0; i<8; i++)
			OLED_SendData(OLED_F8x16[Char - ' '][i + 8]);
	}
}

void OLED_ShowString(uint8_t X, uint8_t Page, char *String, uint8_t FontSize){
	for(uint8_t i = 0; String[i] != '\0'; i++){
		OLED_ShowChar(X + i * FontSize, Page, String[i], FontSize);
	}
}

void OLED_ShowImage(uint8_t X, uint8_t Page, uint8_t Width, uint8_t Height, const uint8_t *Image){
	for(uint8_t j=0; j < Height; j++){
		OLED_SetCursor(X, Page + j);
		for(uint8_t i=0; i<Width; i++){
			OLED_SendData(Image[Width * j + i]);
		}
	}
}

void OLED_ShowChineseChars(uint8_t X, uint8_t Page, char *Chinese_String){
	char chinese_sigle[OLED_CHN_CHAR_WIDTH + 1] = {0};
	uint8_t p_chinese = 0;
	uint8_t pIndex;
	for(uint8_t i=0; Chinese_String[i] != '\0'; i++){
		chinese_sigle[p_chinese] = Chinese_String[i];
		p_chinese++;
		if(p_chinese >= OLED_CHN_CHAR_WIDTH){
			p_chinese = 0;
			for(pIndex=0; strcmp(ChineseFonts_16X16[pIndex].Index, "") != 0; pIndex++){
				if(strcmp(ChineseFonts_16X16[pIndex].Index, chinese_sigle) == 0){
					break;
				}
			}
			OLED_ShowImage(X + ((i + 1) / OLED_CHN_CHAR_WIDTH - 1) * 16, Page, 16, 2, ChineseFonts_16X16[pIndex].data);
		}
	}
}

void OLED_LOGO(void){
	OLED_ShowImage(16 + 24 * 0, 2, 24, 3, Img[0]);
	OLED_ShowImage(16 + 24 * 1, 2, 24, 3, Img[1]);
	OLED_ShowImage(16 + 24 * 2, 2, 24, 3, Img[2]);
	OLED_ShowImage(16 + 24 * 3, 2, 24, 3, Img[3]);
	
	OLED_ShowImage(16 + 24 * 0, 5, 24, 3, Img[4]);
	OLED_ShowImage(16 + 24 * 1, 5, 24, 3, Img[5]);
	OLED_ShowImage(16 + 24 * 2, 5, 24, 3, Img[6]);
	OLED_ShowImage(16 + 24 * 3, 5, 24, 3, Img[7]);
}

void OLED_Ciallo(void){
	uint8_t i = 0;
	while(ciallo_fonts[i][0] != 0xFF){
		if(i < 8)
			OLED_ShowImage(i*8, 4, 8, 2, ciallo_fonts[i]);
		else
			OLED_ShowImage((i-8)*8, 6, 8, 2, ciallo_fonts[i]);
		i++;
	}
}
