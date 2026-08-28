

#include "menu_main.h"


void menuChangeState (uint32_t state)
{
	char buf[64] = {0,};
	// главное меню
	if(state == MAIN_MENU)
	{
		ILI9341_Fill_Screen(MYFON);	// залить экран
		pAVR->avr_states.menu_state = MAIN_MENU;
	}
	// сервисное меню тача
	else if(state == SERVISE_TOUCH)
	{
		ILI9341_Fill_Screen(MYFON);	// залить экран
		snprintf(buf, 64, "X = %d, Y = %d", pAVR->touch.x, pAVR->touch.y); 
		ILI9341_WriteString(75, 10, buf, Font_11x18, WHITE, MYFON);
		ILI9341_Draw_Filled_Circle(pAVR->touch.x, pAVR->touch.y, 10, WHITE);
		
		pAVR->avr_states.menu_state = SERVISE_TOUCH;
	}
	// если такого состояния нет - перейти в главное меню
	else
	{
		ILI9341_Fill_Screen(MYFON);	// залить экран
		pAVR->avr_states.menu_state = MAIN_MENU;
	}
}

void menuSwich (void)
{
	switch(pAVR->avr_states.menu_state)
	{
		case MAIN_MENU:	menuMain();						// главное меню
			break;
		case SERVISE_TOUCH:	serviseTouch();		// сервисное меню тача			
			break;
		default:	menuMain();
			break;
	}
}


// число, время
// режим работы (питание от сети / питание от генератора / питание сети OFF)
// напряжение акб
// зарядка акб ON / OFF
// температура воздуха
// ошибки " "
// предупреждения " "
// справа снизу иконка входа в меню
void menuMain (void)
{
	uint16_t y = 10;
	uint16_t x = 20;
	uint8_t yInc = 25;
	
	#define buf_len  (64)
	char buf[buf_len] = {0,};
	char day_the_week[5] = {0,};
	

	//------------ число, время -----------------------
	HAL_RTC_GetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN);
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); 				// RTC_FORMAT_BIN , RTC_FORMAT_BCD
	switch(DateToUpdate.WeekDay)
	{
		case RTC_WEEKDAY_MONDAY: strncpy(day_the_week, "ПН", sizeof(day_the_week) - 1);
			break;
		case RTC_WEEKDAY_TUESDAY: strncpy(day_the_week, "ВТ", sizeof(day_the_week) - 1);
			break;
		case RTC_WEEKDAY_WEDNESDAY: strncpy(day_the_week, "СР", sizeof(day_the_week) - 1);
			break;
		case RTC_WEEKDAY_THURSDAY: strncpy(day_the_week, "ЧТ", sizeof(day_the_week) - 1);
			break;
		case RTC_WEEKDAY_FRIDAY: strncpy(day_the_week, "ПТ", sizeof(day_the_week) - 1);
			break;
		case RTC_WEEKDAY_SATURDAY: strncpy(day_the_week, "СБ", sizeof(day_the_week) - 1);
			break;
		case RTC_WEEKDAY_SUNDAY: strncpy(day_the_week, "ВС", sizeof(day_the_week) - 1);
			break;
		default:	strncpy(day_the_week, "ХЗ", sizeof(day_the_week) - 1);
			break;
	}

	// в 2 строки
//	snprintf(buf, buf_len, "Дата  %d-%d-20%d-%s", DateToUpdate.Date, DateToUpdate.Month, DateToUpdate.Year, day_the_week);	
//	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
//	y += yInc;
//	snprintf(buf, buf_len, "Время %d:%d:%d", sTime.Hours, sTime.Minutes, sTime.Seconds);
//	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
//	y += yInc;
	
	// в 1 строку
	snprintf(buf, buf_len, "   %d:%d:%d  %d-%d-20%d-%s", sTime.Hours, sTime.Minutes, sTime.Seconds, DateToUpdate.Date, DateToUpdate.Month, DateToUpdate.Year, day_the_week);	
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc + 5;
	
	//------------ режим работы -----------------------
	switch(pAVR->avr_states.power_grid_mode)
	{
		case POWER_IS_OFF: snprintf(buf, buf_len, "Питание дома      отключено");	
			break;
		case EXTERNAL_POWER: snprintf(buf, buf_len, "Питание     от внешней сети");	
			break;
		case POWERED_BY_GENERATOR: snprintf(buf, buf_len, "Питание       от генератора");	
			break;
		default:	snprintf(buf, buf_len, "Питание       ошибка строки");	
			break;
	}
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;

	//------------ напряжение акб -----------------------
	snprintf(buf, buf_len, "Напряжение акб    %.2f В", pAVR->v_t.v_bat);
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;

	//------------ зарядка акб ON / OFF -----------------------
	switch(pAVR->avr_states.flagCharge)
	{
		case RESET: snprintf(buf, buf_len, "Зарядка акб       OFF");	
			break;
		case SET: snprintf(buf, buf_len, "Зарядка акб       ON");	
			break;
		default:	snprintf(buf, buf_len, "Зарядка акб ошибка строки");	
			break;
	}
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ температура воздуха -----------------------
	snprintf(buf, buf_len, "Температура возд. %.1f С", pAVR->v_t.t_cpu);
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ ошибки -----------------------
	snprintf(buf, buf_len, "Ошибки           ( %d )", pAVR->err.counter);
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ предупреждения -----------------------
	snprintf(buf, buf_len, "Предупреждения   ( %d )", pAVR->warn.counter);
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ справа снизу иконка входа в меню -----------------------
	ILI9341_Draw_Rectangle(265, 195, 30, 5, WHITE);
	ILI9341_Draw_Rectangle(265, 205, 30, 5, WHITE);
	ILI9341_Draw_Rectangle(265, 215, 30, 5, WHITE);
	
	//------------ обработка тач -----------------------		
	// если отпущен после нажатия или после длительного нажатия
	if(getTouch() == NO_PRESS || getTouch() == NO_LONG_PRESS)
	{
		// если нажатие в области кнопки меню
		if(pAVR->touch.x >= 265 && pAVR->touch.x <= 295 && pAVR->touch.y >= 195 && pAVR->touch.y <= 220) // если нажатие происходит в области этих координат
			menuChangeState(SERVISE_TOUCH);	// меню отладки тача
	}
}


uint8_t getTouch (void)
{
//    NONE = 0,				// ничего
//		PRESS,					// нажат
//		NO_PRESS,				// отпущен
//		LONG_PRESS,			// длительное нажатие
//		NO_LONG_PRESS,	// отпущен после длительного нажатия

	// для задержки опроса
	static uint32_t time_press = 0;
	if(HAL_GetTick() - time_press < 30)	return NONE;
	time_press = HAL_GetTick();
	
	// если нажат и не отпущен
	if(pAVR->touch.flag_press == SET && pAVR->touch.flag_release == RESET)
	{
			// запросить координаты
			pAVR->touch.x = 0;
			pAVR->touch.y = 0;

			TOUCH_CS_UNSELECT;
			DISP_CS_UNSELECT;
			HAL_SPI_DeInit(DISP_SPI_PTR);
			hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
			HAL_SPI_Init(DISP_SPI_PTR);

			ILI9341_TouchGetCoordinates(&pAVR->touch.x, &pAVR->touch.y);			
			
			HAL_SPI_DeInit(DISP_SPI_PTR);
			hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
			HAL_SPI_Init(DISP_SPI_PTR);

			__HAL_SPI_ENABLE(DISP_SPI_PTR);
			DISP_CS_SELECT;	
			
			// если координаты получены
			if(pAVR->touch.x && pAVR->touch.y)
				return PRESS;
	}
	// если отпущен
	else if(pAVR->touch.flag_release == SET)
	{
		pAVR->touch.flag_press = RESET;
		pAVR->touch.flag_release = RESET;
		time_press = HAL_GetTick();
		return NO_PRESS;
	}
			
	return NONE;
}

void serviseTouch (void)
{
	char buf[64] = {0,};
	uint8_t status = getTouch();
	
	if(status == NO_PRESS || status == NO_LONG_PRESS)
	{	
		snprintf(buf, 64, "X = %d, Y = %d", pAVR->touch.x, pAVR->touch.y); 
		ILI9341_Fill_Screen(MYFON);
		ILI9341_WriteString(75, 10, buf, Font_11x18, WHITE, MYFON);
		ILI9341_Draw_Filled_Circle(pAVR->touch.x, pAVR->touch.y, 10, WHITE);
	}
	
	//------------ справа снизу иконка входа в меню -----------------------
	ILI9341_Draw_Rectangle(265, 195, 30, 5, WHITE);
	ILI9341_Draw_Rectangle(265, 205, 30, 5, WHITE);
	ILI9341_Draw_Rectangle(265, 215, 30, 5, WHITE);
	
	//------------ обработка тач -----------------------		
	// если отпущен после нажатия или после длительного нажатия
	if(status == NO_LONG_PRESS)
	{
		// если нажатие в области кнопки меню
		if(pAVR->touch.x >= 265 && pAVR->touch.x <= 295 && pAVR->touch.y >= 195 && pAVR->touch.y <= 220) // если нажатие происходит в области этих координат
			menuChangeState(MAIN_MENU);	// меню отладки тача
	}
}

