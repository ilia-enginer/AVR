

#include "popUpWindow.h"


// окно подтверждения действия
uint8_t confirmClick (const char* text)
{
	static uint8_t flag_block = RESET;		// чтоб каждый раз не обновлял окно
	char buf[BUF_LEN] = {0,};
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
		len_text = lineAlignment(text, buf, stringLen);
		for(uint16_t i = 0; i < len_text; )	{
			i += ILI9341_WriteStringLen(x, y, buf+i, stringLen, Font_11x18, WHITE, NAVY);
//			memset(buf, '\00', i);
			y += yInc;
		}

		flag_block = SET;
		
		// ------------ кнопка "да" ------------
		ILI9341_Draw_Filled_Rectangle_Coord(50, 170, 100, 200, MYFON);
		snprintf(buf, BUF_LEN, "Да"); 
		ILI9341_WriteString(65, 175, buf, Font_11x18, WHITE, MYFON);
		
		// ------------ кнопка "нет" ------------
		ILI9341_Draw_Filled_Rectangle_Coord(220, 170, 270, 200, MYFON);
		snprintf(buf, BUF_LEN, "Нет"); 
		ILI9341_WriteString(230, 175, buf, Font_11x18, WHITE, MYFON);
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
void notification(const char* header, const char* text, uint32_t time, uint32_t menu)
{
	static uint8_t flag_block = RESET;		// чтоб каждый раз не обновлял окно
	static uint32_t menuReturn = MAIN_MENU;
	static uint32_t timeDelay = 0;
	
	char buf[BUF_LEN] = {0,};
	uint16_t y = 65;				// начальные координаты
	uint16_t x = 30;
	uint8_t yInc = 35;			// на сколько опускать каждую строку
	uint16_t stringLen = 25;
	
	if(flag_block == RESET)
	{		
		if(time)
			timeDelay = (time * 1000) + HAL_GetTick();
		else
			timeDelay = 0;
		
		ILI9341_Draw_Filled_Rectangle_Coord(15, 20, 305, 210, DARKGREEN);	// квадрат
		
		snprintf(buf, BUF_LEN, "%s", header); 														// заголовок
		ILI9341_WriteString(40, 25, buf, Font_16x26, WHITE, DARKGREEN);	
//		memset(buf, '\00', sizeof(buf));
		
		// текст уведомления
		// не более хх символов в строке
		uint16_t len_text = 0;
		len_text = lineAlignment(text, buf, stringLen);
		for(uint16_t i = 0; i < len_text; )	{
			i += ILI9341_WriteStringLen(x, y, buf+i, stringLen, Font_11x18, WHITE, DARKGREEN);
//			memset(buf, '\00', i);
			y += yInc;
		}
	
		// ------------ кнопка "ок" ------------
//		memset(buf, '\00', sizeof(buf));
		ILI9341_Draw_Filled_Rectangle_Coord(130, 170, 190, 200, MYFON);
		snprintf(buf, BUF_LEN, "Ок"); 
		ILI9341_WriteString(150, 175, buf, Font_11x18, WHITE, MYFON);
		
		flag_block = SET;
		menuReturn = menu;
		menuChangeState(NOTIFICATION);
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

//	uint16_t bufSize = strlen(text);
//	uint16_t maxLenLine = lenLine * 2;
//	uint8_t spase = 0;
//	uint16_t numSpase;
//	
//	for(uint16_t b = 0, t = 0, c = 0; t < bufSize; t++, b++)
//	{
//		
//		buf[b] = text[t];
//		
//if ( ((uint8_t)buf[b] >= 0x20) && (uint8_t)buf[b] <= 0x7e )
//	c++;
//else
//	c = c+2;
//		
////		if((buf[b] == ' ') && (c < maxLenLine))	// если это символ пробела
////		{
////			spase++;
////			numSpase = b;
////		}
//				
//		// если символ новой строки
////		if((uint8_t)buf[b] == 0x7f)		
//		if(buf[b] == 0x0A)		
//		{
//			c = c+2;
//			// заполнить текущую строку пробелом
//			for( ; c <= maxLenLine; c++)
//			{
//				buf[b++] = ' ';
//				c = c+2;
//			}
//			c = 0;
//			spase = 0;
//		}
////		else
////		{
////			// если дошел до конца предполагаемой длины строки
////			if(c >= maxLenLine)
////			{
////				// если пробелов нечетное кол-во
////				// добавить еще один, т.к. пробел - 1 символ, русская буква - 2 символа
////				if (spase % 2 != 0) {
////						b = numSpase;
////						t = numSpase-1;
////				} 
////				c = 0;
////			}
////			
////		}
//		
//	}
}
