
#include <stdio.h>
#include <string.h>

#include "popUpWindow.h"
#include "ILI9341_GFX.h"
#include "menu_main.h"
#include "touch.h"

#define POP_UP_BUF_LEN  (200)
static char popUpWinBuf[POP_UP_BUF_LEN] = {0,};
static uint8_t flag_block = RESET;		// чтоб каждый раз не обновлял окно
// окно подтверждения действия
uint8_t confirmClick (const char* text)
{
	uint16_t x = 60;
	uint16_t y = 60;		// начальные координаты
	uint8_t yInc = 25;	// на сколько опускать каждую строку
	uint16_t stringLen = 18;
	
	if(flag_block == RESET)
	{
		ILI9341_Draw_Filled_Rectangle_Coord(40, 30, 280, 210, NAVY);

		// текст уведомления
		// не более хх символов в строке
		uint16_t len_text = 0;
		len_text = lineAlignment(text, popUpWinBuf, stringLen);
		for(uint16_t i = 0; i < len_text; )	{
			i += ILI9341_WriteStringLen(x, y, popUpWinBuf+i, stringLen, Font_11x18, WHITE, NAVY);
//			memset(buf, '\00', i);
			y += yInc;
		}

		flag_block = SET;
		
		// ------------ кнопка "да" ------------
		ILI9341_Draw_Filled_Rectangle_Coord(50, 170, 100, 200, MYFON);
		snprintf(popUpWinBuf, BUF_LEN, "Да"); 
		ILI9341_WriteString(65, 175, popUpWinBuf, Font_11x18, WHITE, MYFON);
		
		// ------------ кнопка "нет" ------------
		ILI9341_Draw_Filled_Rectangle_Coord(220, 170, 270, 200, MYFON);
		snprintf(popUpWinBuf, BUF_LEN, "Нет"); 
		ILI9341_WriteString(230, 175, popUpWinBuf, Font_11x18, WHITE, MYFON);
		
		memset(popUpWinBuf, '\00', sizeof(popUpWinBuf));
	}
	
	//------------ обработка тач -----------------------		
	// если отпущен после короткого нажатия 
	if(getTouch() == NO_PRESS)
	{
		// если нажатие в области кнопки "да"
		if(pAVR->touch.x >= 50 && pAVR->touch.x <= 100 && pAVR->touch.y >= 170 && pAVR->touch.y <= 200) // если нажатие происходит в области этих координат
		{
			flag_block = RESET;
			return YES;
		}
		// если нажатие в области кнопки "нет"
		else if(pAVR->touch.x >= 220 && pAVR->touch.x <= 270 && pAVR->touch.y >= 170 && pAVR->touch.y <= 200) // если нажатие происходит в области этих координат
		{
			flag_block = RESET;
			return NO;
		}
	}

	return NOTHING;
}


// уведомление
// - заголовок
// - текст
// - время на которое надо открыть в секундах. 0 - на постоянную
// - меню в которое надо потом вернуться
static uint32_t menuReturn = MAIN_MENU;
static uint32_t timeDelay = 0;
void notification(const char* header, const char* text, uint32_t time, uint32_t menu)
{
	uint16_t y = 55;				// начальные координаты
	uint16_t x = 30;
	uint8_t yInc = 20;			// на сколько опускать каждую строку
	uint16_t stringLen = 25;
	
	if(flag_block == RESET)
	{		
		if(time)
			timeDelay = (time * 1000) + HAL_GetTick();
		else
			timeDelay = 0;
		
		ILI9341_Draw_Filled_Rectangle_Coord(15, 20, 305, 210, DARKGREEN);	// квадрат
		
		snprintf(popUpWinBuf, BUF_LEN, "%s", header); 														// заголовок
		ILI9341_WriteString(40, 25, popUpWinBuf, Font_16x26, WHITE, DARKGREEN);	
		memset(popUpWinBuf, '\00', sizeof(popUpWinBuf));
		
		// текст уведомления
		// не более хх символов в строке
		uint16_t len_text = 0;
		len_text = lineAlignment(text, popUpWinBuf, stringLen);
		for(uint16_t i = 0; i < len_text; )	{
			i += ILI9341_WriteStringLen(x, y, popUpWinBuf+i, stringLen, Font_11x18, WHITE, DARKGREEN);
			memset(popUpWinBuf, '\00', i);
			y += yInc;
			if(y > 210-16)
				break;
		}
	
		// ------------ кнопка "ок" ------------
		memset(popUpWinBuf, '\00', sizeof(popUpWinBuf));
		ILI9341_Draw_Filled_Rectangle_Coord(130, 170, 190, 200, MYFON);
		snprintf(popUpWinBuf, BUF_LEN, "Ок"); 
		ILI9341_WriteString(150, 175, popUpWinBuf, Font_11x18, WHITE, MYFON);
		
		flag_block = SET;
		menuReturn = menu;
		menuChangeState(NOTIFICATION);
		memset(popUpWinBuf, '\00', sizeof(popUpWinBuf));
		return;
	}

	
	//------------ обработка тач -----------------------		
	// если отпущен после короткого нажатия 
	if(getTouch() == NO_PRESS || getTouch() == NO_LONG_PRESS)
	{
		// если нажатие в области кнопки "ok"
		if(pAVR->touch.x >= 130 && pAVR->touch.x <= 190 && pAVR->touch.y >= 170 && pAVR->touch.y <= 200)
		{
			flag_block = RESET;
			timeDelay = 0;
			menuChangeState(menuReturn);
		}
	}
	
	//------------ обработка времени -----------------------		
	if(timeDelay)
		if(HAL_GetTick() > timeDelay)
		{
			flag_block = RESET;
			timeDelay = 0;
			menuChangeState(menuReturn);
		}
}

// выравнивание строки
// разбивает строку на заданное кол-во символов в строке
// учитавает знак новой строки
// ставит четное кол-во знаков латинской раскладки (необходимо для русских букв)
// возвращает кол-во символов буфера
uint16_t lineAlignment(const char* text, char *buf, uint16_t lenLine)
{	
	volatile uint16_t lenBuf = 0;
	uint16_t lenStr = 0;
	char spase[] = {' '};
	
	while(*text)
	{
		if(*text == '\n')
		{
			for( ; lenStr < lenLine; ) {
				strncpy(buf++, spase, 1);
				lenStr++;
				lenBuf++;
			}
			text++;
			lenStr = 0;
		}
		
		if ( (uint8_t)*text >= 0xC0 ){	// код 0xC0 соответствует символу кириллица 'A' по ASCII Win-1251
			strncpy(buf++, text++, 2);
			buf++;
			text++;
			lenBuf += 2;
		}
		else {
			strncpy(buf++, text++, 1);
			lenBuf++;
		}	
		lenStr ++;
		if(lenStr >= lenLine) lenStr = 0;
	}
	return lenBuf;
}
