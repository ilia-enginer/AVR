

#include "menu_warn_err.h"


// меню просмотра ошибок
void menuGetErrors (void)
{
	uint16_t y = 5;			// начальные координаты
	uint16_t x = 5;		// начальные координаты
	uint8_t yInc = 16;	// на сколько опускать каждую строку
	char buf[BUF_LEN] = {0,};
	uint8_t status;
	
	static uint8_t flag_clear = RESET;
	
	// проверка подтверждения очистки ошибок
	if(flag_clear)
	{
		status = confirmClick("Очистить ошибки?");
		if(status == YES)
		{
			flag_clear = RESET;
			resetErrors();
			ILI9341_Fill_Screen(MYFON);	// залить экран
		}
		else if(status == NO)
		{
			flag_clear = RESET;
			ILI9341_Fill_Screen(MYFON);	// залить экран
		}
		return;
	}
	
	//------------ обработка тач -----------------------		
	// если отпущен после нажатия или после длительного нажатия
	if(getTouch() == NO_PRESS || getTouch() == NO_LONG_PRESS)
	{
		// если нажатие в области кнопки назад
		if(pAVR->touch.x >= 250 && pAVR->touch.x <= 315 && pAVR->touch.y >= 200 && pAVR->touch.y <= 235) 
			menuChangeState(MAIN_MENU);	// главное меню 
		// если нажатие в области кнопки сброса ошибок
		if(pAVR->touch.x >= 295 && pAVR->touch.x <= 318 && pAVR->touch.y >= 135 && pAVR->touch.y <= 195) 
			flag_clear = SET;
	}

	// обновлять главное меню не чаще, чем раз в 1с
	static uint32_t time_update = 0;
	if(HAL_GetTick() - time_update < 1000)	return;
	time_update = HAL_GetTick();
	
	
	// ------------ кнопка "назад" ------------
	ILI9341_Draw_Filled_Rectangle_Coord(250, 200, 315, 235, NAVY);
	snprintf(buf, BUF_LEN, "Назад"); 
	ILI9341_WriteString(255, 210, buf, Font_11x18, WHITE, NAVY);
	
	// ------------ кнопка "сброс" ------------
	ILI9341_Draw_Filled_Rectangle_Coord(295, 135, 318, 195, NAVY);
	snprintf(buf, BUF_LEN, "C"); 
	ILI9341_WriteString(303, 140, buf, Font_7x9, WHITE, NAVY);
	snprintf(buf, BUF_LEN, "l"); 
	ILI9341_WriteString(303, 150, buf, Font_7x9, WHITE, NAVY);
	snprintf(buf, BUF_LEN, "e"); 
	ILI9341_WriteString(303, 160, buf, Font_7x9, WHITE, NAVY);
	snprintf(buf, BUF_LEN, "a"); 
	ILI9341_WriteString(303, 170, buf, Font_7x9, WHITE, NAVY);
	snprintf(buf, BUF_LEN, "r"); 
	ILI9341_WriteString(303, 180, buf, Font_7x9, WHITE, NAVY);
	
	// ------------ заголовок ------------
	snprintf(buf, BUF_LEN, "Ошибки ( %d )", pAVR->err.counter);	
	ILI9341_WriteString(100, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc + 5;
	
	// если нет ошибок
	if(pAVR->err.counter == 0)
	{
		snprintf(buf, BUF_LEN, "Ошибок нет");	
		ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
		return;
	}
	
	// ------------ вывод списка ошибок ------------
	if(pAVR->err.array_flags[ERR_MAX_LAUNCH_ATTEMP])
	{
		snprintf(buf, BUF_LEN, "- макс.кол-во попыток запуска");	
		ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
		y += yInc;
	}
	if(pAVR->err.array_flags[ERR_STARTER_RELE_SHUTDOWN])
	{
		snprintf(buf, BUF_LEN, "- ошибка откл. реле стартера");	
		ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
		y += yInc;
	}
	if(pAVR->err.array_flags[ERR_STARTER_RELE_ACTIVATION])
	{
		snprintf(buf, BUF_LEN, "- ошибка вкл. реле стартера");	
		ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
		y += yInc;
	}
	if(pAVR->err.array_flags[ERR_LOW_VOLTAGE_AKB])
	{
		snprintf(buf, BUF_LEN, "- низкое напряжение акб");	
		ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
		y += yInc;
	}
	if(pAVR->err.array_flags[ERR_HIGHT_VOLTAGE_AKB])
	{
		snprintf(buf, BUF_LEN, "- высокое напряжение акб");	
		ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
		y += yInc;
	}
	if(pAVR->err.array_flags[ERR_CHARG_CIRCUIT])
	{
		snprintf(buf, BUF_LEN, "- неисправность цепи зарядки");	
		ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
		y += yInc;
	}
	if(pAVR->err.array_flags[ERR_HARD_RESET])
	{
		snprintf(buf, BUF_LEN, "- был хард ресет");	
		ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
		y += yInc;
	}
	if(pAVR->err.array_flags[ERR_WATCH_DOG])
	{
		snprintf(buf, BUF_LEN, "- был сброс по вачдогу");	
		ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
		y += yInc;
	}
	if(pAVR->err.array_flags[ERR_ENGINE_STALLED])
	{
		snprintf(buf, BUF_LEN, "- двигатель заглох");	
		ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
		y += yInc;
	}
	if(pAVR->err.array_flags[ERR_SD_CARD])
	{
		snprintf(buf, BUF_LEN, "- ошибка sd карты");	
		ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
		y += yInc;
	}
}

		
// меню просмотра предупреждений
void menuGetWarnings (void)
{
	uint16_t y = 5;			// начальные координаты
	uint16_t x = 5;		// начальные координаты
	uint8_t yInc = 16;	// на сколько опускать каждую строку
	char buf[BUF_LEN] = {0,};
	uint8_t status;
	
	static uint8_t flag_clear = RESET;
	
	// проверка подтверждения очистки ошибок
	if(flag_clear)
	{
		status = confirmClick("Очистить предупреждения?");
		if(status == YES)
		{
			flag_clear = RESET;
			resetWarning();
			ILI9341_Fill_Screen(MYFON);	// залить экран
		}
		else if(status == NO)
		{
			flag_clear = RESET;
			ILI9341_Fill_Screen(MYFON);	// залить экран
		}
		return;
	}
	
	//------------ обработка тач -----------------------		
	// если отпущен после нажатия или после длительного нажатия
	if(getTouch() == NO_PRESS || getTouch() == NO_LONG_PRESS)
	{
		// если нажатие в области кнопки назад
		if(pAVR->touch.x >= 250 && pAVR->touch.x <= 315 && pAVR->touch.y >= 200 && pAVR->touch.y <= 235) 
			menuChangeState(MAIN_MENU);	// главное меню 
		// если нажатие в области кнопки сброса ошибок
		if(pAVR->touch.x >= 295 && pAVR->touch.x <= 318 && pAVR->touch.y >= 135 && pAVR->touch.y <= 195) 
			flag_clear = SET;
	}

	// обновлять главное меню не чаще, чем раз в 1с
	static uint32_t time_update = 0;
	if(HAL_GetTick() - time_update < 1000)	return;
	time_update = HAL_GetTick();
	
	
	// ------------ кнопка "назад" ------------
	ILI9341_Draw_Filled_Rectangle_Coord(250, 200, 315, 235, NAVY);
	snprintf(buf, BUF_LEN, "Назад"); 
	ILI9341_WriteString(255, 210, buf, Font_11x18, WHITE, NAVY);
	
	// ------------ кнопка "сброс" ------------
	ILI9341_Draw_Filled_Rectangle_Coord(295, 135, 318, 195, NAVY);
	snprintf(buf, BUF_LEN, "C"); 
	ILI9341_WriteString(303, 140, buf, Font_7x9, WHITE, NAVY);
	snprintf(buf, BUF_LEN, "l"); 
	ILI9341_WriteString(303, 150, buf, Font_7x9, WHITE, NAVY);
	snprintf(buf, BUF_LEN, "e"); 
	ILI9341_WriteString(303, 160, buf, Font_7x9, WHITE, NAVY);
	snprintf(buf, BUF_LEN, "a"); 
	ILI9341_WriteString(303, 170, buf, Font_7x9, WHITE, NAVY);
	snprintf(buf, BUF_LEN, "r"); 
	ILI9341_WriteString(303, 180, buf, Font_7x9, WHITE, NAVY);
	
	// ------------ заголовок ------------
	snprintf(buf, BUF_LEN, "Предупреждения ( %d )", pAVR->warn.counter);	
	ILI9341_WriteString(50, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc + 5;
	
	// если нет предупреждений
	if(pAVR->warn.counter == 0)
	{
		snprintf(buf, BUF_LEN, "Предупреждений нет");	
		ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
		return;
	}
	
	// ------------ вывод списка ошибок ------------
	if(pAVR->warn.array_flags[WARN_MANUAL_CONTROL_EN])
	{
		snprintf(buf, BUF_LEN, "- ручной режим работы");	
		ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
		y += yInc;
	}
	if(pAVR->warn.array_flags[WARN_NECESSITY_TECH_INSP])
	{
		snprintf(buf, BUF_LEN, "- необходимо провести ТО");	
		ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
		y += yInc;
	}
	if(pAVR->warn.array_flags[WARN_CHARGE_AKB])
	{
		snprintf(buf, BUF_LEN, "- необходимо зарядить АКБ");	
		ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
		y += yInc;
	}


}

