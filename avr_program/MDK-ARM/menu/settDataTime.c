

#include "settDataTime.h"


// натройка времени
void setTime (void)
{
	char buf[BUF_LEN] = {0,};
	uint8_t status;
	
	uint16_t y = 55;			// начальные координаты
	uint16_t x = 5;				// начальные координаты
	uint8_t yInc = 35;		// на сколько опускать каждую строку
	
	static uint8_t flagFirst = RESET;		// первый вход в функцию
	static uint8_t flagSave = RESET;		// флаг сохранения
	static uint8_t flagExit = RESET;		// флаг выходы
	
	static uint8_t Hours;            
  static uint8_t Minutes;          
  static uint8_t Seconds;  
	
	// ------------------------------------------------------------------------
	if(!flagFirst)		// если первый вход в функцию
	{
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); 				// RTC_FORMAT_BIN , RTC_FORMAT_BCD
		HAL_RTC_GetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN);
		
		Hours		= sTime.Hours;
		Minutes	= sTime.Minutes;
		Seconds	= sTime.Seconds;
		
		ILI9341_Fill_Screen(MYFON);	// залить экран
		snprintf(buf, BUF_LEN, "Настройка времени");	
		ILI9341_WriteString(20, 5, buf, Font_16x26, WHITE, MYFON);
		
		snprintf(buf, BUF_LEN, "Часы       -  %.2d  +", Hours);	
		ILI9341_WriteString(x, y, buf, Font_16x26, WHITE, MYFON);
		ILI9341_Draw_Hollow_Rectangle_Coord(172, y - 2, 205, y + 26, WHITE);
		ILI9341_Draw_Hollow_Rectangle_Coord(282, y - 2, 318, y + 26, WHITE);
		y = y + yInc;
		
		snprintf(buf, BUF_LEN, "Минуты     -  %.2d  +", Minutes);	
		ILI9341_WriteString(x, y, buf, Font_16x26, WHITE, MYFON);
		ILI9341_Draw_Hollow_Rectangle_Coord(172, y - 2, 205, y + 26, WHITE);
		ILI9341_Draw_Hollow_Rectangle_Coord(282, y - 2, 318, y + 26, WHITE);
		y = y + yInc;
		
		snprintf(buf, BUF_LEN, "Секунды    -  %.2d  +", Seconds);	
		ILI9341_WriteString(x, y, buf, Font_16x26, WHITE, MYFON);
		ILI9341_Draw_Hollow_Rectangle_Coord(172, y - 2, 205, y + 26, WHITE);
		ILI9341_Draw_Hollow_Rectangle_Coord(282, y - 2, 318, y + 26, WHITE);
		y = y + yInc;
		
		
		// кнопка "Сохранить"
		ILI9341_Draw_Filled_Rectangle_Coord(40, 200, 160, 235, NAVY);
		snprintf(buf, BUF_LEN, "Сохранить");	
		ILI9341_WriteString(50, 210, buf, Font_11x18, WHITE, NAVY);
		
		// кнопка "Выйти"
		ILI9341_Draw_Filled_Rectangle_Coord(200, 200, 280, 235, NAVY);
		snprintf(buf, BUF_LEN, "Выйти");	
		ILI9341_WriteString(210, 210, buf, Font_11x18, WHITE, NAVY);
		
		flagFirst = SET;
	}
	
	// ------------------------------------------------------------------------
	// проверка перед сохранением
	if(flagSave)
	{
		status = confirmClick("Сохранить?");
		if(status == YES)
		{
			sTime.Hours 	= Hours;
			sTime.Minutes = Minutes;
			sTime.Seconds = Seconds;
			HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
			
			menuChangeState(MAIN_MENU);	
			flagSave = RESET;	
			flagFirst = RESET;
		}
		else if(status == NO)
		{
			flagSave = RESET;
			ILI9341_Fill_Screen(MYFON);
			flagFirst = RESET;
		}
		return;
	}
	// проверка перед выходом
	if(flagExit)
	{
		status = confirmClick("Выйти?");
		if(status == YES)
		{
			menuChangeState(MAIN_MENU);	
			flagExit = RESET;	
			flagFirst = RESET;
		}
		else if(status == NO)
		{
			flagExit = RESET;
			ILI9341_Fill_Screen(MYFON);
			flagFirst = RESET;
		}
		return;
	}

	// ------------------------------------------------------------------------
	//------------ обработка тач -----------------------		
	// если отпущен после нажатия или после длительного нажатия
	if(getTouch() == NO_PRESS || getTouch() == NO_LONG_PRESS)
	{
		// если нажатие в области кнопки "Сохранить?"
		if(pAVR->touch.x >= 40 && pAVR->touch.x <= 160 && pAVR->touch.y >= 200 && pAVR->touch.y <= 235) 
			flagSave = SET;
		// если нажатие на "Выйти"
		else if(pAVR->touch.x >= 200 && pAVR->touch.x <= 280 && pAVR->touch.y >= 200 && pAVR->touch.y <= 235) 
			flagExit = SET;
			
		// -------------------------кнопки "-", "+" -----------------------------------------------
		// *********** часы *************
		// кнопка - , настройка часов
		else if(pAVR->touch.x >= 172 && pAVR->touch.x <= 205 && pAVR->touch.y >= y - 2 && pAVR->touch.y <= y + 26) 
		{
			Hours--;
			if(Hours > 23) Hours = 23;
			snprintf(buf, BUF_LEN, "Часы       -  %.2d  +", Hours);	
			ILI9341_WriteString(x, y, buf, Font_16x26, WHITE, MYFON);
			ILI9341_Draw_Hollow_Rectangle_Coord(172, y - 2, 205, y + 26, WHITE);
			ILI9341_Draw_Hollow_Rectangle_Coord(282, y - 2, 318, y + 26, WHITE);
		}
		// кнопка + , настройка часов
		else if(pAVR->touch.x >= 282 && pAVR->touch.x <= 318 && pAVR->touch.y >= y - 2 && pAVR->touch.y <= y + 26) 
		{
			Hours++;
			if(Hours > 23) Hours = 0;
			snprintf(buf, BUF_LEN, "Часы       -  %.2d  +", Hours);	
			ILI9341_WriteString(x, y, buf, Font_16x26, WHITE, MYFON);
			ILI9341_Draw_Hollow_Rectangle_Coord(172, y - 2, 205, y + 26, WHITE);
			ILI9341_Draw_Hollow_Rectangle_Coord(282, y - 2, 318, y + 26, WHITE);
		}
		// *********** минуты *************
		else if(pAVR->touch.x >= 172 && pAVR->touch.x <= 205 && pAVR->touch.y >= (y + yInc) - 2 && pAVR->touch.y <= (y + yInc) + 26) 
		{
			Minutes--;
			if(Minutes > 59) Minutes = 59;
			snprintf(buf, BUF_LEN, "Минуты     -  %.2d  +", Minutes);
			ILI9341_WriteString(x, y + yInc, buf, Font_16x26, WHITE, MYFON);
			ILI9341_Draw_Hollow_Rectangle_Coord(172, (y + yInc) - 2, 205, (y + yInc) + 26, WHITE);
			ILI9341_Draw_Hollow_Rectangle_Coord(282, (y + yInc) - 2, 318, (y + yInc) + 26, WHITE);
		}
		// кнопка + , настройка часов
		else if(pAVR->touch.x >= 282 && pAVR->touch.x <= 318 && pAVR->touch.y >= (y + yInc) - 2 && pAVR->touch.y <= (y + yInc) + 26) 
		{
			Minutes++;
			if(Minutes > 59) Minutes = 0;
			snprintf(buf, BUF_LEN, "Минуты     -  %.2d  +", Minutes);	
			ILI9341_WriteString(x, y + yInc, buf, Font_16x26, WHITE, MYFON);
			ILI9341_Draw_Hollow_Rectangle_Coord(172, (y + yInc) - 2, 205, (y + yInc) + 26, WHITE);
			ILI9341_Draw_Hollow_Rectangle_Coord(282, (y + yInc) - 2, 318, (y + yInc) + 26, WHITE);
		}
		// *********** секунды *************
		else if(pAVR->touch.x >= 172 && pAVR->touch.x <= 205 && pAVR->touch.y >= (y + (yInc * 2)) - 2 && pAVR->touch.y <= (y + (yInc * 2)) + 26) 
		{
			Seconds--;
			if(Seconds > 59) Seconds = 59;
			snprintf(buf, BUF_LEN, "Секунды    -  %.2d  +", Seconds);
			ILI9341_WriteString(x, y + yInc*2, buf, Font_16x26, WHITE, MYFON);
			ILI9341_Draw_Hollow_Rectangle_Coord(172, (y + (yInc * 2)) - 2, 205, (y + (yInc * 2)) + 26, WHITE);
			ILI9341_Draw_Hollow_Rectangle_Coord(282, (y + (yInc * 2)) - 2, 318, (y + (yInc * 2)) + 26, WHITE);
		}
		// кнопка + , настройка часов
		else if(pAVR->touch.x >= 282 && pAVR->touch.x <= 318 && pAVR->touch.y >= (y + (yInc * 2)) - 2 && pAVR->touch.y <= (y + (yInc * 2)) + 26) 
		{
			Seconds++;
			if(Seconds > 59) Seconds = 0;
			snprintf(buf, BUF_LEN, "Секунды    -  %.2d  +", Seconds);
			ILI9341_WriteString(x, y + yInc*2, buf, Font_16x26, WHITE, MYFON);
			ILI9341_Draw_Hollow_Rectangle_Coord(172, (y + (yInc * 2)) - 2, 205, (y + (yInc * 2)) + 26, WHITE);
			ILI9341_Draw_Hollow_Rectangle_Coord(282, (y + (yInc * 2)) - 2, 318, (y + (yInc * 2)) + 26, WHITE);
		}
	}
}

// настройка даты
void setData (void)
{
	char day_the_week[5] = {0,};
	char buf[BUF_LEN] = {0,};
	uint8_t status;
	
	uint16_t y = 40;			// начальные координаты
	uint16_t x = 5;				// начальные координаты
	uint8_t yInc = 35;		// на сколько опускать каждую строку
	
	static uint8_t flagFirst = RESET;		// первый вход в функцию
	static uint8_t flagSave = RESET;		// флаг сохранения
	static uint8_t flagExit = RESET;		// флаг выходы
	
	static uint8_t WeekDay;   
	static uint8_t Month;            
  static uint8_t Date;          
  static uint8_t Year;  
	
	
	// ------------------------------------------------------------------------
	if(!flagFirst)		// если первый вход в функцию
	{
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); 				// RTC_FORMAT_BIN , RTC_FORMAT_BCD
		HAL_RTC_GetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN);
		
		WeekDay	= DateToUpdate.WeekDay;
		Month		= DateToUpdate.Month;
		Date		= DateToUpdate.Date;
		Year		= DateToUpdate.Year;
		
		switch(WeekDay)
		{
			case RTC_WEEKDAY_MONDAY: strncpy(day_the_week, "ПН", sizeof(day_the_week));
				break;
			case RTC_WEEKDAY_TUESDAY: strncpy(day_the_week, "ВТ", sizeof(day_the_week));
				break;
			case RTC_WEEKDAY_WEDNESDAY: strncpy(day_the_week, "СР", sizeof(day_the_week));
				break;
			case RTC_WEEKDAY_THURSDAY: strncpy(day_the_week, "ЧТ", sizeof(day_the_week));
				break;
			case RTC_WEEKDAY_FRIDAY: strncpy(day_the_week, "ПТ", sizeof(day_the_week));
				break;
			case RTC_WEEKDAY_SATURDAY: strncpy(day_the_week, "СБ", sizeof(day_the_week));
				break;
			case RTC_WEEKDAY_SUNDAY: strncpy(day_the_week, "ВС", sizeof(day_the_week));
				break;
			default:	strncpy(day_the_week, "ХЗ", sizeof(day_the_week) - 1);
				break;
		}
	
		ILI9341_Fill_Screen(MYFON);	// залить экран
		snprintf(buf, BUF_LEN, "  Настройка даты");	
		ILI9341_WriteString(20, 5, buf, Font_16x26, WHITE, MYFON);
		
		snprintf(buf, BUF_LEN, "Число      -  %.2d  +", Date);	
		ILI9341_WriteString(x, y, buf, Font_16x26, WHITE, MYFON);
		ILI9341_Draw_Hollow_Rectangle_Coord(172, y - 2, 205, y + 26, WHITE);
		ILI9341_Draw_Hollow_Rectangle_Coord(282, y - 2, 318, y + 26, WHITE);
		y = y + yInc;
		
		snprintf(buf, BUF_LEN, "Месяц      -  %.2d  +", Month);	
		ILI9341_WriteString(x, y, buf, Font_16x26, WHITE, MYFON);
		ILI9341_Draw_Hollow_Rectangle_Coord(172, y - 2, 205, y + 26, WHITE);
		ILI9341_Draw_Hollow_Rectangle_Coord(282, y - 2, 318, y + 26, WHITE);
		y = y + yInc;
		
		snprintf(buf, BUF_LEN, "Год        -  %.2d  +", Year);	
		ILI9341_WriteString(x, y, buf, Font_16x26, WHITE, MYFON);
		ILI9341_Draw_Hollow_Rectangle_Coord(172, y - 2, 205, y + 26, WHITE);
		ILI9341_Draw_Hollow_Rectangle_Coord(282, y - 2, 318, y + 26, WHITE);
		y = y + yInc;
		
		snprintf(buf, BUF_LEN, "День       -  %s  +", day_the_week);
		ILI9341_WriteString(x, y, buf, Font_16x26, WHITE, MYFON);
		ILI9341_Draw_Hollow_Rectangle_Coord(172, y - 2, 205, y + 26, WHITE);
		ILI9341_Draw_Hollow_Rectangle_Coord(282, y - 2, 318, y + 26, WHITE);
		
		// кнопка "Сохранить"
		ILI9341_Draw_Filled_Rectangle_Coord(40, 200, 160, 235, NAVY);
		snprintf(buf, BUF_LEN, "Сохранить");	
		ILI9341_WriteString(50, 210, buf, Font_11x18, WHITE, NAVY);
		
		// кнопка "Выйти"
		ILI9341_Draw_Filled_Rectangle_Coord(200, 200, 280, 235, NAVY);
		snprintf(buf, BUF_LEN, "Выйти");	
		ILI9341_WriteString(210, 210, buf, Font_11x18, WHITE, NAVY);
		
		flagFirst = SET;
	}
	
	// ------------------------------------------------------------------------
	// проверка перед сохранением
	if(flagSave)
	{
		status = confirmClick("Сохранить?");
		if(status == YES)
		{
			DateToUpdate.WeekDay 	= WeekDay;
			DateToUpdate.Month 		= Month;
			DateToUpdate.Date 		= Date;
			DateToUpdate.Year 		= Year;
			HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN);
			
			menuChangeState(MAIN_MENU);	
			flagSave = RESET;	
			flagFirst = RESET;
		}
		else if(status == NO)
		{
			flagSave = RESET;
			ILI9341_Fill_Screen(MYFON);
			flagFirst = RESET;
		}
		return;
	}
	// проверка перед выходом
	if(flagExit)
	{
		status = confirmClick("Выйти?");
		if(status == YES)
		{
			menuChangeState(MAIN_MENU);	
			flagExit = RESET;	
			flagFirst = RESET;
		}
		else if(status == NO)
		{
			flagExit = RESET;
			ILI9341_Fill_Screen(MYFON);
			flagFirst = RESET;
		}
		return;
	}

	// ------------------------------------------------------------------------
	//------------ обработка тач -----------------------		
	// если отпущен после нажатия или после длительного нажатия
	if(getTouch() == NO_PRESS || getTouch() == NO_LONG_PRESS)
	{
		// если нажатие в области кнопки "Сохранить?"
		if(pAVR->touch.x >= 40 && pAVR->touch.x <= 160 && pAVR->touch.y >= 200 && pAVR->touch.y <= 235) 
			flagSave = SET;
		// если нажатие на "Выйти"
		else if(pAVR->touch.x >= 200 && pAVR->touch.x <= 280 && pAVR->touch.y >= 200 && pAVR->touch.y <= 235) 
			flagExit = SET;
			
			
		// -------------------------кнопки "-", "+" -----------------------------------------------
		// *********** число *************
		// кнопка - , настройка числа
		else if(pAVR->touch.x >= 172 && pAVR->touch.x <= 205 && pAVR->touch.y >= y - 2 && pAVR->touch.y <= y + 26) 
		{
			Date--;
			if(Date == 0) Date = 31;
			snprintf(buf, BUF_LEN, "Число      -  %.2d  +", Date);	
			ILI9341_WriteString(x, y, buf, Font_16x26, WHITE, MYFON);
			ILI9341_Draw_Hollow_Rectangle_Coord(172, y - 2, 205, y + 26, WHITE);
			ILI9341_Draw_Hollow_Rectangle_Coord(282, y - 2, 318, y + 26, WHITE);
		}
		// кнопка + , настройка числа
		else if(pAVR->touch.x >= 282 && pAVR->touch.x <= 318 && pAVR->touch.y >= y - 2 && pAVR->touch.y <= y + 26) 
		{
			Date++;
			if(Date > 31) Date = 1;
			snprintf(buf, BUF_LEN, "Число      -  %.2d  +", Date);	
			ILI9341_WriteString(x, y, buf, Font_16x26, WHITE, MYFON);
			ILI9341_Draw_Hollow_Rectangle_Coord(172, y - 2, 205, y + 26, WHITE);
			ILI9341_Draw_Hollow_Rectangle_Coord(282, y - 2, 318, y + 26, WHITE);
		}
		// *********** месяц *************
		else if(pAVR->touch.x >= 172 && pAVR->touch.x <= 205 && pAVR->touch.y >= (y + yInc) - 2 && pAVR->touch.y <= (y + yInc) + 26) 
		{
			Month--;
			if(Month == 0) Month = 12;
			snprintf(buf, BUF_LEN, "Месяц      -  %.2d  +", Month);
			ILI9341_WriteString(x, y + yInc, buf, Font_16x26, WHITE, MYFON);
			ILI9341_Draw_Hollow_Rectangle_Coord(172, (y + yInc) - 2, 205, (y + yInc) + 26, WHITE);
			ILI9341_Draw_Hollow_Rectangle_Coord(282, (y + yInc) - 2, 318, (y + yInc) + 26, WHITE);
		}
		// кнопка + , настройка месяц
		else if(pAVR->touch.x >= 282 && pAVR->touch.x <= 318 && pAVR->touch.y >= (y + yInc) - 2 && pAVR->touch.y <= (y + yInc) + 26) 
		{
			Month++;
			if(Month > 12) Month = 1;
			snprintf(buf, BUF_LEN, "Месяц      -  %.2d  +", Month);	
			ILI9341_WriteString(x, y + yInc, buf, Font_16x26, WHITE, MYFON);
			ILI9341_Draw_Hollow_Rectangle_Coord(172, (y + yInc) - 2, 205, (y + yInc) + 26, WHITE);
			ILI9341_Draw_Hollow_Rectangle_Coord(282, (y + yInc) - 2, 318, (y + yInc) + 26, WHITE);
		}
		// *********** год *************
		else if(pAVR->touch.x >= 172 && pAVR->touch.x <= 205 && pAVR->touch.y >= (y + (yInc * 2)) - 2 && pAVR->touch.y <= (y + (yInc * 2)) + 26) 
		{
			Year--;
			if(Year == 25) Year = 99;
			snprintf(buf, BUF_LEN, "Год        -  %.2d  +", Year);
			ILI9341_WriteString(x, y + (yInc*2), buf, Font_16x26, WHITE, MYFON);
			ILI9341_Draw_Hollow_Rectangle_Coord(172, (y + (yInc * 2)) - 2, 205, (y + (yInc * 2)) + 26, WHITE);
			ILI9341_Draw_Hollow_Rectangle_Coord(282, (y + (yInc * 2)) - 2, 318, (y + (yInc * 2)) + 26, WHITE);
		}
		// кнопка + , настройка года
		else if(pAVR->touch.x >= 282 && pAVR->touch.x <= 318 && pAVR->touch.y >= (y + (yInc * 2)) - 2 && pAVR->touch.y <= (y + (yInc * 2)) + 26) 
		{
			Year++;
			if(Year > 99) Year = 26;
			snprintf(buf, BUF_LEN, "Год        -  %.2d  +", Year);
			ILI9341_WriteString(x, y + (yInc*2), buf, Font_16x26, WHITE, MYFON);
			ILI9341_Draw_Hollow_Rectangle_Coord(172, (y + (yInc * 2)) - 2, 205, (y + (yInc * 2)) + 26, WHITE);
			ILI9341_Draw_Hollow_Rectangle_Coord(282, (y + (yInc * 2)) - 2, 318, (y + (yInc * 2)) + 26, WHITE);
		}
		// *********** день недели *************
		else if(pAVR->touch.x >= 172 && pAVR->touch.x <= 205 && pAVR->touch.y >= (y + (yInc * 3)) - 2 && pAVR->touch.y <= (y + (yInc * 3)) + 26) 
		{
			WeekDay--;
			if(WeekDay == 0) WeekDay = 7;
			switch(WeekDay)
			{
				case RTC_WEEKDAY_MONDAY: strncpy(day_the_week, "ПН", sizeof(day_the_week));
					break;
				case RTC_WEEKDAY_TUESDAY: strncpy(day_the_week, "ВТ", sizeof(day_the_week));
					break;
				case RTC_WEEKDAY_WEDNESDAY: strncpy(day_the_week, "СР", sizeof(day_the_week));
					break;
				case RTC_WEEKDAY_THURSDAY: strncpy(day_the_week, "ЧТ", sizeof(day_the_week));
					break;
				case RTC_WEEKDAY_FRIDAY: strncpy(day_the_week, "ПТ", sizeof(day_the_week));
					break;
				case RTC_WEEKDAY_SATURDAY: strncpy(day_the_week, "СБ", sizeof(day_the_week));
					break;
				case RTC_WEEKDAY_SUNDAY: strncpy(day_the_week, "ВС", sizeof(day_the_week));
					break;
				default:	strncpy(day_the_week, "ХЗ", sizeof(day_the_week) - 1);
					break;
			}
			snprintf(buf, BUF_LEN, "День       -  %s  +", day_the_week);
			ILI9341_WriteString(x, y + (yInc*3), buf, Font_16x26, WHITE, MYFON);
			ILI9341_Draw_Hollow_Rectangle_Coord(172, (y + (yInc * 3)) - 2, 205, (y + (yInc * 3)) + 26, WHITE);
			ILI9341_Draw_Hollow_Rectangle_Coord(282, (y + (yInc * 3)) - 2, 318, (y + (yInc * 3)) + 26, WHITE);
		}
		// кнопка + , настройка дня недели
		else if(pAVR->touch.x >= 282 && pAVR->touch.x <= 318 && pAVR->touch.y >= (y + (yInc * 3)) - 2 && pAVR->touch.y <= (y + (yInc * 3)) + 26) 
		{
			WeekDay++;
			if(WeekDay > 7) WeekDay = 1;
			switch(WeekDay)
			{
				case RTC_WEEKDAY_MONDAY: strncpy(day_the_week, "ПН", sizeof(day_the_week));
					break;
				case RTC_WEEKDAY_TUESDAY: strncpy(day_the_week, "ВТ", sizeof(day_the_week));
					break;
				case RTC_WEEKDAY_WEDNESDAY: strncpy(day_the_week, "СР", sizeof(day_the_week));
					break;
				case RTC_WEEKDAY_THURSDAY: strncpy(day_the_week, "ЧТ", sizeof(day_the_week));
					break;
				case RTC_WEEKDAY_FRIDAY: strncpy(day_the_week, "ПТ", sizeof(day_the_week));
					break;
				case RTC_WEEKDAY_SATURDAY: strncpy(day_the_week, "СБ", sizeof(day_the_week));
					break;
				case RTC_WEEKDAY_SUNDAY: strncpy(day_the_week, "ВС", sizeof(day_the_week));
					break;
				default:	strncpy(day_the_week, "ХЗ", sizeof(day_the_week) - 1);
					break;
			}
			snprintf(buf, BUF_LEN, "День       -  %s  +", day_the_week);
			ILI9341_WriteString(x, y + (yInc*3), buf, Font_16x26, WHITE, MYFON);
			ILI9341_Draw_Hollow_Rectangle_Coord(172, (y + (yInc * 3)) - 2, 205, (y + (yInc * 3)) + 26, WHITE);
			ILI9341_Draw_Hollow_Rectangle_Coord(282, (y + (yInc * 3)) - 2, 318, (y + (yInc * 3)) + 26, WHITE);
		}
	}
}

