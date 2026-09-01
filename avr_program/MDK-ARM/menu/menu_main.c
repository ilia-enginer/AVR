

#include "menu_main.h"

// смена менюшки
void menuChangeState (uint32_t state)
{
	// главное меню
	if(state == MAIN_MENU)
	{
		ILI9341_Fill_Screen(MYFON);	// залить экран
		pAVR->avr_states.menu_state = MAIN_MENU;
	}
	// сервисное меню тача
	else if(state == SERVISE_TOUCH)
		pAVR->avr_states.menu_state = SERVISE_TOUCH;
	// настройка времени
	else if(state == SET_TIME)
		pAVR->avr_states.menu_state = SET_TIME;
	// настройка даты
	else if(state == SET_DATA)
		pAVR->avr_states.menu_state = SET_DATA;
	// меню просмотра ошибок
	else if(state == GET_ERROR)
	{
		ILI9341_Fill_Screen(MYFON);	// залить экран
		pAVR->avr_states.menu_state = GET_ERROR;
	}
	// меню просмотра предупреждений
	else if(state == GET_WARNING)
	{
		ILI9341_Fill_Screen(MYFON);	// залить экран
		pAVR->avr_states.menu_state = GET_WARNING;
	}
	// если такого состояния нет - перейти в главное меню
	else
	{
		ILI9341_Fill_Screen(MYFON);	// залить экран
		pAVR->avr_states.menu_state = MAIN_MENU;
	}
}

// автомат переключения менюшки
void menuSwich (void)
{
	switch(pAVR->avr_states.menu_state)
	{
		case MAIN_MENU:	menuMain();						// главное меню
			break;
		case SERVISE_TOUCH:	serviseTouch();		// сервисное меню тача			
			break;
		case SET_TIME:	setTime();						// натройка времени		
			break;
		case SET_DATA:	setData();						// натройка даты		
			break;
		case GET_ERROR:	menuGetErrors();			// меню просмотра ошибок
			break;
		case GET_WARNING:	menuGetWarnings();	// меню просмотра предупреждений
			break;
		default:	menuMain();
			break;
	}
}


// главное меню
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
	char buf[BUF_LEN] = {0,};
	char day_the_week[5] = {0,};
	
	uint8_t status;
	static uint8_t flagSetTime = RESET;						// для перехода в режим настройки времени
	static uint8_t flagSetData = RESET;						// для перехода в режим настройки даты
	static uint8_t flagPowerAutoManual = RESET;		// для смены режимы управления ручной / авто
	static uint8_t flagStatusEngine = RESET;			// для принудительного включения / выключения двс
	static uint8_t flagPowGridMod = RESET;				// для вызова меню переключения силового рубильника авр
	static uint8_t flagCharge = RESET;						// для принудительного включения / выключения зарядки
	
	
	uint16_t y = 5;			// начальные координаты
	uint16_t x = 20;		// начальные координаты
	uint8_t yInc = 22;	// на сколько опускать каждую строку
	
	
	// проверка перед переходом в настройки времени
	if(flagSetTime)
	{
		status = confirmClick("Настроить время?");
		if(status == YES)
		{
			menuChangeState(SET_TIME);	
			flagSetTime = RESET;	
		}
		else if(status == NO)
		{
			flagSetTime = RESET;
			ILI9341_Fill_Screen(MYFON);
		}
		return;
	}
	// проверка перед переходом в настройки даты
	else if(flagSetData)
	{
		status = confirmClick("Настроить дату?");
		if(status == YES)
		{
			menuChangeState(SET_DATA);	
			flagSetData = RESET;	
		}
		else if(status == NO)
		{
			flagSetData = RESET;
			ILI9341_Fill_Screen(MYFON);
		}
		return;
	}
	// проверка нужно ли менять режим работы
	else if(flagPowerAutoManual)
	{
		if(pAVR->avr_states.powerAutoManual == AVR_AUTO)
			status = confirmClick("Перейти на ручное управление?");
		else if(pAVR->avr_states.powerAutoManual == AVR_MANUAL)
			status = confirmClick("Перейти на автоматическое управление?");
			
		if(status == YES)
		{
			if(pAVR->avr_states.powerAutoManual == AVR_AUTO)				pAVR->avr_states.powerAutoManual = AVR_MANUAL;
			else if(pAVR->avr_states.powerAutoManual == AVR_MANUAL)	pAVR->avr_states.powerAutoManual = AVR_AUTO;
			
			flagPowerAutoManual = RESET;	
			menuChangeState(MAIN_MENU);	
		}
		else if(status == NO)
		{
			flagPowerAutoManual = RESET;
			menuChangeState(MAIN_MENU);	
		}
		return;
	}
	// проверка нужно ли заводить / глушить генератор
	else if(flagStatusEngine)
	{
		if(pAVR->avr_states.statusEngine == RESET)
			status = confirmClick("Запустить двигатель?");
		else if(pAVR->avr_states.statusEngine == SET)
			status = confirmClick("Заглушить двигатель?");
			
		if(status == YES)
		{
			if(pAVR->avr_states.statusEngine == RESET)			pAVR->avr_states.statusEngine = SET;
			else if(pAVR->avr_states.statusEngine == SET)		pAVR->avr_states.statusEngine = RESET;
			
			flagStatusEngine = RESET;	
			pAVR->avr_states.powerAutoManual = AVR_MANUAL;
			menuChangeState(MAIN_MENU);	
		}
		else if(status == NO)
		{
			flagStatusEngine = RESET;
			menuChangeState(MAIN_MENU);	
		}
		return;
	}
	// вызов меню переключения силового автомата
	else if(flagPowGridMod)
	{	
		status = switchAvrAutomatic();	
		if(status)
		{
			flagPowGridMod = RESET;	
			menuChangeState(MAIN_MENU);	
		}
		return;
	}
	// проверка нужно ли включать / выключать зарядку
	else if(flagCharge)
	{
		if(pAVR->avr_states.flagCharge)
			status = confirmClick("Выключить зарядку АКБ?");
		else
			status = confirmClick("Включить зарядку АКБ?");
			
		if(status == YES)
		{
			if(pAVR->avr_states.flagCharge)	pAVR->avr_states.flagCharge = RESET;
			else														pAVR->avr_states.flagCharge = SET;
			pAVR->avr_states.powerAutoManual = AVR_MANUAL;
			
			flagCharge = RESET;	
			menuChangeState(MAIN_MENU);	
		}
		else if(status == NO)
		{
			flagCharge = RESET;
			menuChangeState(MAIN_MENU);	
		}
		return;
	}
	
	
	//------------ обработка тач -----------------------		
	// если отпущен после нажатия или после длительного нажатия
	if(getTouch() == NO_PRESS || getTouch() == NO_LONG_PRESS)
	{
		// если нажатие в области кнопки меню
		if(pAVR->touch.x >= 285 && pAVR->touch.x <= 315 && pAVR->touch.y >= 200 && pAVR->touch.y <= 225) 
			menuChangeState(SERVISE_TOUCH);	// меню отладки тача
		// если нажатие на время
		else if(pAVR->touch.x >= 30 && pAVR->touch.x <= 130 && pAVR->touch.y >= 0 && pAVR->touch.y <= yInc) 
			flagSetTime = SET;
		// если нажатие на дату
		else if(pAVR->touch.x >= 155 && pAVR->touch.x <= 290 && pAVR->touch.y >= 0 && pAVR->touch.y <= yInc) 
			flagSetData = SET;
		// если нажатие на "режим управления"
		else if(pAVR->touch.x >= 0 && pAVR->touch.x <= 320 && pAVR->touch.y >= y + 5 + (yInc * 1) && pAVR->touch.y <= y + 5 + (yInc * 2)) 	
			flagPowerAutoManual = SET;
		// если нажатие на "двс"
		else if(pAVR->touch.x >= 0 && pAVR->touch.x <= 320 && pAVR->touch.y >= y + 5 + (yInc * 3) && pAVR->touch.y <= y + 5 + (yInc * 4)) 	
			flagStatusEngine = SET;
		// если нажатие на "питание дома"
		else if(pAVR->touch.x >= 0 && pAVR->touch.x <= 320 && pAVR->touch.y >= y + 5 + (yInc * 4) && pAVR->touch.y <= y + 5 + (yInc * 5)) 	
			flagPowGridMod = SET;
		// если нажатие на "зарядка акб"
		else if(pAVR->touch.x >= 0 && pAVR->touch.x <= 320 && pAVR->touch.y >= y + 5 + (yInc * 6) && pAVR->touch.y <= y + 5 + (yInc * 7)) 	
			flagCharge = SET;
		// если на "ошибки"
		else if(pAVR->touch.x >= 0 && pAVR->touch.x <= 260 && pAVR->touch.y >= y + 5 + (yInc * 8) && pAVR->touch.y <= y + 5 + (yInc * 9)) 	
			menuChangeState(GET_ERROR);	// меню просмотра ошибок
		// если на "предупреждения"
		else if(pAVR->touch.x >= 0 && pAVR->touch.x <= 260 && pAVR->touch.y >= y + 5 + (yInc * 9) && pAVR->touch.y <= y + 5 + (yInc * 10)) 	
			menuChangeState(GET_WARNING);	// меню просмотра предупреждений
	}
	
	// обновлять главное меню не чаще, чем раз в 1с
	static uint32_t time_update = 0;
	if(HAL_GetTick() - time_update < 1000)	return;
	time_update = HAL_GetTick();

	//------------ число, время -----------------------
	// сначала считывать время, затем дату, иначе не будут разблокированы теневые регистры и считывание времени будет не каждый раз будет успешно
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); 				// RTC_FORMAT_BIN , RTC_FORMAT_BCD
	HAL_RTC_GetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN);
	switch(DateToUpdate.WeekDay)
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
	snprintf(buf, BUF_LEN, "  %d:%d:%d  %d-%d-20%d-%s", sTime.Hours, sTime.Minutes, sTime.Seconds, DateToUpdate.Date, DateToUpdate.Month, DateToUpdate.Year, day_the_week);	
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc + 5;
	
	//------------ режим работы авто/ручной -----------------------
	if(pAVR->avr_states.powerAutoManual == AVR_AUTO)
		snprintf(buf, BUF_LEN, "Режим управления  Авто");
	else if(pAVR->avr_states.powerAutoManual == AVR_MANUAL)
		snprintf(buf, BUF_LEN, "Режим управления  Ручной");
	else
		snprintf(buf, BUF_LEN, "Режим управления  Ошибка");
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ внешнее питание да / нет -----------------------
	if(pAVR->avr_states.extPowerSupply == EXT_POWER_OFF)
		snprintf(buf, BUF_LEN, "Внешнее питание   Нет");
	else if(pAVR->avr_states.extPowerSupply == EXT_POWER_ON)
		snprintf(buf, BUF_LEN, "Внешнее питание   Да");
	else
		snprintf(buf, BUF_LEN, "Внешнее питание   Ошибка");
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ статус ДВС генератора (запущен/остановлен) -----------------------
	if(pAVR->avr_states.statusEngine == RESET)
		snprintf(buf, BUF_LEN, "Двс               OFF");
	else if(pAVR->avr_states.statusEngine == SET)
		snprintf(buf, BUF_LEN, "Двс               ON");
	else
		snprintf(buf, BUF_LEN, "Двс               Ошибка");
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ режим работы -----------------------
	switch(pAVR->avr_states.power_grid_mode)
	{
		case POWER_IS_OFF: 					snprintf(buf, BUF_LEN, "Силовое реле      Отключено");	
			break;
		case EXTERNAL_POWER: 				snprintf(buf, BUF_LEN, "Силовое реле      Внеш.сеть");	
			break;
		case POWERED_BY_GENERATOR: 	snprintf(buf, BUF_LEN, "Силовое реле      Генератор");	
			break;
		default:										snprintf(buf, BUF_LEN, "Силовое реле      ош. строк");	
			break;
	}
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ напряжение акб -----------------------
	snprintf(buf, BUF_LEN, "Напряжение акб    %.2f В", pAVR->v_t.v_bat);
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;

	//------------ зарядка акб ON / OFF -----------------------
	switch(pAVR->avr_states.flagCharge)
	{
		case RESET: snprintf(buf, BUF_LEN, "Зарядка акб       OFF");	
			break;
		case SET: snprintf(buf, BUF_LEN, "Зарядка акб       ON");	
			break;
		default:	snprintf(buf, BUF_LEN, "Зарядка акб ошибка строки");	
			break;
	}
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ температура воздуха -----------------------
	snprintf(buf, BUF_LEN, "Температура возд. %.1f С", pAVR->v_t.t_cpu);
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ ошибки -----------------------
	snprintf(buf, BUF_LEN, "Ошибки           ( %d )", pAVR->err.counter);
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ предупреждения -----------------------
	snprintf(buf, BUF_LEN, "Предупреждения   ( %d )", pAVR->warn.counter);
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ справа снизу иконка входа в меню -----------------------
	ILI9341_Draw_Rectangle(285, 200, 30, 5, WHITE);
	ILI9341_Draw_Rectangle(285, 210, 30, 5, WHITE);
	ILI9341_Draw_Rectangle(285, 220, 30, 5, WHITE);
}

// переключение силового автомата авр
uint8_t switchAvrAutomatic (void)
{
	static uint8_t flag_block = RESET;		// чтоб каждый раз не обновлял окно
	static uint8_t flag_status_block = RESET;
	static POWER_GRID_MODE power_grid_mode;
	char buf[BUF_LEN] = {0,};
	uint16_t y = 65;				// начальные координаты
	uint8_t yInc = 35;			// на сколько опускать каждую строку
	uint8_t status;
	static uint8_t yPoint = 75;	// координатынавигационной точки
	
	if(flag_block == RESET)
	{
		flag_block = SET;
		power_grid_mode = pAVR->avr_states.power_grid_mode;
		
		switch(power_grid_mode)
		{
			case POWER_IS_OFF:					yPoint = y+10;
				break;
			case EXTERNAL_POWER:				yPoint = y + yInc+10;
				break;
			case POWERED_BY_GENERATOR:	yPoint = y + (yInc * 2)+10;
				break;
			default:										yPoint = 85;
				break;
		}
		
		ILI9341_Draw_Filled_Rectangle_Coord(15, 20, 305, 210, DARKGREEN);
		snprintf(buf, BUF_LEN, "Силовое реле"); 
		ILI9341_WriteString(65, 25, buf, Font_16x26, WHITE, DARKGREEN);
		
		snprintf(buf, BUF_LEN, "положение 0"); 
		ILI9341_WriteString(60, y, buf, Font_11x18, WHITE, DARKGREEN);
		ILI9341_Draw_Hollow_Circle(40, y+10, 9, WHITE);
		ILI9341_Draw_Hollow_Circle(40, y+10, 10, WHITE);	
		y = y + yInc;
		
		snprintf(buf, BUF_LEN, "пит. от внешней сети"); 
		ILI9341_WriteString(60, y, buf, Font_11x18, WHITE, DARKGREEN);
		ILI9341_Draw_Hollow_Circle(40, y+10, 9, WHITE);
		ILI9341_Draw_Hollow_Circle(40, y+10, 10, WHITE);	
		y = y + yInc;
		
		snprintf(buf, BUF_LEN, "пит. от генератора"); 
		ILI9341_WriteString(60, y, buf, Font_11x18, WHITE, DARKGREEN);
		ILI9341_Draw_Hollow_Circle(40, y+10, 9, WHITE);
		ILI9341_Draw_Hollow_Circle(40, y+10, 10, WHITE);	
		y = y + yInc;
		
		ILI9341_Draw_Filled_Circle(40, yPoint, 5, WHITE);	// рисует закрашеный круг (первые два аргумента это координаты центра, а третий радиус)
		
		// ------------ кнопка "ок" ------------
		ILI9341_Draw_Filled_Rectangle_Coord(130, 170, 190, 200, MYFON);
		snprintf(buf, BUF_LEN, "Ок"); 
		ILI9341_WriteString(150, 175, buf, Font_11x18, WHITE, MYFON);
	}
	
	if(flag_status_block == SET)
	{
		switch(power_grid_mode)
		{
			case POWER_IS_OFF:					status = confirmClick("Переключить в положение 0?");
				break;
			case EXTERNAL_POWER:				status = confirmClick("Переключить на питание от внешней сети?");
				break;
			case POWERED_BY_GENERATOR:	status = confirmClick("Переключить на питание от генератора?");
				break;
			default:										status = confirmClick("ошибка переключения");
				break;
		}
		if(status == YES)
		{
			pAVR->avr_states.power_grid_mode = power_grid_mode;
			pAVR->avr_states.powerAutoManual = AVR_MANUAL;
			flag_status_block = RESET;
			flag_block = RESET;
			return 1;
		}
		else if(status == NO)
		{
			flag_status_block = RESET;
			flag_block = RESET;
			return 1;
		}
	}
	
	//------------ обработка тач -----------------------		
	// если отпущен после короткого нажатия 
	if(getTouch() == NO_PRESS || getTouch() == NO_LONG_PRESS)
	{
		// если нажатие в области кнопки "положение 0"
		if(pAVR->touch.x >= 15 && pAVR->touch.x <= 305 && pAVR->touch.y >= y+10 && pAVR->touch.y <= y+10+18) // если нажатие происходит в области этих координат
		{
			power_grid_mode = POWER_IS_OFF;
			ILI9341_Draw_Filled_Circle(40, y+10, 6, DARKGREEN);	
			ILI9341_Draw_Filled_Circle(40, y+10+yInc, 6, DARKGREEN);	
			ILI9341_Draw_Filled_Circle(40, y+10+(yInc*2), 6, DARKGREEN);	
			yPoint = y+10;
			ILI9341_Draw_Filled_Circle(40, yPoint, 5, WHITE);
		}
		// если нажатие в области кнопки "пит. от внешней сети"
		else if(pAVR->touch.x >= 15 && pAVR->touch.x <= 305 && pAVR->touch.y >= y+10 + yInc && pAVR->touch.y <= y+10+18 + yInc) // если нажатие происходит в области этих координат
		{
			power_grid_mode = EXTERNAL_POWER;
			ILI9341_Draw_Filled_Circle(40, y+10, 6, DARKGREEN);	
			ILI9341_Draw_Filled_Circle(40, y+10+yInc, 6, DARKGREEN);	
			ILI9341_Draw_Filled_Circle(40, y+10+(yInc*2), 6, DARKGREEN);	
			yPoint = y + yInc+10;
			ILI9341_Draw_Filled_Circle(40, yPoint, 5, WHITE);
		}
		// если нажатие в области кнопки "пит. от генератора"
		else if(pAVR->touch.x >= 15 && pAVR->touch.x <= 305 && pAVR->touch.y >= y+10 + (yInc * 2) && pAVR->touch.y <= y+10+18 + (yInc * 2)) // если нажатие происходит в области этих координат
		{
			power_grid_mode = POWERED_BY_GENERATOR;
			ILI9341_Draw_Filled_Circle(40, y+10, 6, DARKGREEN);	
			ILI9341_Draw_Filled_Circle(40, y+10+yInc, 6, DARKGREEN);	
			ILI9341_Draw_Filled_Circle(40, y+10+(yInc*2), 6, DARKGREEN);	
			yPoint = y + (yInc * 2)+10;
			ILI9341_Draw_Filled_Circle(40, yPoint, 5, WHITE);
		}
		// если нажатие в области кнопки "ok"
		else if(pAVR->touch.x >= 130 && pAVR->touch.x <= 190 && pAVR->touch.y >= 170 && pAVR->touch.y <= 200) // если нажатие происходит в области этих координат
		{
			// если выбранное положение не равно исходному
			if(power_grid_mode != pAVR->avr_states.power_grid_mode)
			{
				flag_status_block = SET;
				switch(power_grid_mode)
				{
					case POWER_IS_OFF:					status = confirmClick("Переключить в положение 0?");
						break;
					case EXTERNAL_POWER:				status = confirmClick("Переключить на питание от внешней сети?");
						break;
					case POWERED_BY_GENERATOR:	status = confirmClick("Переключить на питание от генератора?");
						break;
					default:										status = confirmClick("ошибка переключения");
						break;
				}
				if(status == YES)
				{
					pAVR->avr_states.power_grid_mode = power_grid_mode;
					pAVR->avr_states.powerAutoManual = AVR_MANUAL;
					flag_status_block = RESET;
					flag_block = RESET;
					return 1;
				}
				else if(status == NO)
				{
					flag_status_block = RESET;
					flag_block = RESET;
					return 1;
				}
			}
			else
			{
				flag_block = RESET;
				flag_status_block = RESET;
				return 1;
			}
		}
	}
	return 0;
}



// окно подтверждения действия
uint8_t confirmClick (const char* text)
{
	static uint8_t flag_block = RESET;		// чтоб каждый раз не обновлял окно
	char buf[BUF_LEN] = {0,};
	uint16_t y = 60;		// начальные координаты
	uint8_t yInc = 25;	// на сколько опускать каждую строку
	uint8_t maxCharLine = 32;	// макс кол-во символов в строке
	
	if(flag_block == RESET)
	{
		ILI9341_Draw_Filled_Rectangle_Coord(40, 30, 280, 210, NAVY);
		
		// не более 19 символов в строке
		uint8_t len_text = strlen(text);
		for(uint8_t i = 0; i < len_text; )
		{
			snprintf(buf, maxCharLine+1, "%s", text + i);	
			//strncpy(buf, text + i, maxCharLine);
			
			ILI9341_WriteString(60, y, buf, Font_11x18, WHITE, NAVY);
			i = i + maxCharLine;
			y = y + yInc;
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



