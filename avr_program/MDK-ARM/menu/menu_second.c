

#include "menu_second.h"


void secondMain (void)
{
	char buf[BUF_LEN] = {0,};
	uint16_t y = 5;			// начальные координаты
	uint16_t x = 20;		// начальные координаты
	uint8_t yInc = 22;	// на сколько опускать каждую строку

	//------------ обработка тач -----------------------		
	// если отпущен после нажатия или после длительного нажатия
	if(getTouch() == NO_PRESS || getTouch() == NO_LONG_PRESS)
	{
		// если нажатие в области кнопки меню
		if(pAVR->touch.x >= 5 && pAVR->touch.x <= 35 && pAVR->touch.y >= 200 && pAVR->touch.y <= 225) 
			menuChangeState(MAIN_MENU);	// главное меню 
		// если нажатие на "моточасы"
		else if(pAVR->touch.x >= x && pAVR->touch.x <= 320 && pAVR->touch.y >= y + (yInc * 0) && pAVR->touch.y <= y + (yInc * 1)) 
			engineHoursGet();	// вывод информации о ТО
		// если нажатие на "ТО"
		else if(pAVR->touch.x >= x && pAVR->touch.x <= 320 && pAVR->touch.y >= y + (yInc * 1) && pAVR->touch.y <= y + (yInc * 2)) 
			serviseWorkGet();	// вывод моточасов
		// если нажатие на Сервис тачскрина
		else if(pAVR->touch.x >= x && pAVR->touch.x <= 320 && pAVR->touch.y >= y + (yInc * 6) && pAVR->touch.y <= y + (yInc * 7)) 
			menuChangeState(SERVISE_TOUCH);	// меню сервис тачскрина
	}
	
	// обновлять главное меню не чаще, чем раз в 1с
	static uint32_t time_update = 0;
	if(HAL_GetTick() - time_update < 1000)	return;
	time_update = HAL_GetTick();
	
	//------------ Моточасы -----------------------
	snprintf(buf, BUF_LEN, "Моточасы");
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ ТО -----------------------
	snprintf(buf, BUF_LEN, "ТО");
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ Проведение ТО-----------------------
	snprintf(buf, BUF_LEN, "Проведение ТО");
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ Отключение эл-ва -----------------------
	snprintf(buf, BUF_LEN, "Отключение эл-ва");
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ Запуск ДВС -----------------------
	snprintf(buf, BUF_LEN, "Запуск ДВС");
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ Ручное переключение реле -----------------------
	snprintf(buf, BUF_LEN, "Ручное переключение реле");
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ Сервис тачскрина -----------------------
	snprintf(buf, BUF_LEN, "Сервис тачскрина");
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	
	//------------ слева снизу иконка перехода на шаг назад -----------------------
	ILI9341_Draw_Rectangle(5, 200, 30, 5, WHITE);
	ILI9341_Draw_Rectangle(5, 210, 30, 5, WHITE);
	ILI9341_Draw_Rectangle(5, 220, 30, 5, WHITE);
}

// вывод моточасов
void engineHoursGet (void)
{
	char header[BUF_LEN] = {0,};
	char buf[BUF_LEN] = {0,};
	
	snprintf(header, BUF_LEN, "Моточасы"); 
	snprintf(buf, BUF_LEN, "Моточасы всего     %d\nМоточасы после ТО  %d", 5, 3);		//??? заменить на реальное время 
	notification(header, buf, 0, SECOND_MENU);
}

// вывод информации о ТО
void serviseWorkGet(void)
{
	char header[BUF_LEN] = {0,};
	char buf[BUF_LEN] = {0,};
	
	snprintf(header, BUF_LEN, "ТО"); 
	snprintf(buf, BUF_LEN, "Последнее ТО    %d.%d.%d\nСлед. ТО %d.%d.%d\nМот. час. до ТО %d", 5, 11, 26, 3, 12, 27, 5);		//??? заменить на реальное время 
	notification(header, buf, 0, SECOND_MENU);
}


