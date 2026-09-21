
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "menu_main.h"
#include "init.h"
#include "warn_err.h"
#include "menu_second.h"
#include "popUpWindow.h"
#include "touch.h"

static uint8_t flagTO = RESET;		// для обновления даты ТО
// обновлять главное меню не чаще, чем раз в 1с
static uint32_t time_update = 0;

void secondMain (void)
{	
	uint8_t status;
	uint16_t y = 5;			// начальные координаты
	uint16_t x = 20;		// начальные координаты
	uint8_t yInc = 22;	// на сколько опускать каждую строку

	
	// проверка перед обновлением даты ТО
	if(flagTO)
	{
		status = confirmClick("Обновить дату ТО?");
		if(status == YES)
		{
			flagTO = RESET;
			
			// обновить инфо о ТО
			updateInfoTO();
			// сохранить на sd
			setFillStructureStoryParameters();
			
			delWarn(WARN_NECESSITY_TECH_INSP);
			notification("ТО", "Дата ТО обновлена", 0, SECOND_MENU);
			recLog("Пользователь - дата ТО обновлена"); 
		}
		else if(status == NO)
		{
			flagTO = RESET;
			menuChangeState(SECOND_MENU);
		}
		return;
	}
	
	//------------ обработка тач -----------------------		
	// если отпущен после нажатия или после длительного нажатия
	if(getTouch() == NO_PRESS || getTouch() == NO_LONG_PRESS)
	{
		// если нажатие в области кнопки меню
		if(pAVR->touch.x >= 5 && pAVR->touch.x <= 35 && pAVR->touch.y >= 200 && pAVR->touch.y <= 225) {
			menuChangeState(MAIN_MENU);	// главное меню 
			return;
		}
		// если нажатие на "моточасы"
		else if(pAVR->touch.x >= x && pAVR->touch.x <= 320 && pAVR->touch.y >= y + (yInc * 0) && pAVR->touch.y <= y + (yInc * 1)) {
			engineHoursGet();	// вывод информации о ТО
			return;
		}	
		// если нажатие на "ТО"
		else if(pAVR->touch.x >= x && pAVR->touch.x <= 320 && pAVR->touch.y >= y + (yInc * 1) && pAVR->touch.y <= y + (yInc * 2)) {
			serviseWorkGet();	// вывод моточасов
			return;
		}	
		// если нажатие на "провести ТО"
		else if(pAVR->touch.x >= x && pAVR->touch.x <= 320 && pAVR->touch.y >= y + (yInc * 2) && pAVR->touch.y <= y + (yInc * 3)) {
			flagTO = SET;
			return;
		}	
		// если нажатие на "отключение эл-ва"
		else if(pAVR->touch.x >= x && pAVR->touch.x <= 320 && pAVR->touch.y >= y + (yInc * 3) && pAVR->touch.y <= y + (yInc * 4)) {
			powerOutageGet();
			return;
		}	
		// если нажатие на "Запуск ДВС инфо"
		else if(pAVR->touch.x >= x && pAVR->touch.x <= 320 && pAVR->touch.y >= y + (yInc * 4) && pAVR->touch.y <= y + (yInc * 5)) {
			startEngineGet();
			return;
		}	
		// если нажатие на "Ручное переключение реле"
		else if(pAVR->touch.x >= x && pAVR->touch.x <= 320 && pAVR->touch.y >= y + (yInc * 5) && pAVR->touch.y <= y + (yInc * 6)) {
			menuChangeState(MANUAL_RELE_SWITCH);	// меню "Ручное переключение реле"
			return;
		}	
		// если нажатие на Сервис тачскрина
		else if(pAVR->touch.x >= x && pAVR->touch.x <= 320 && pAVR->touch.y >= y + (yInc * 6) && pAVR->touch.y <= y + (yInc * 7)) {
			menuChangeState(SERVISE_TOUCH);	// меню сервис тачскрина
			return;
		}	
		// если нажатие на Тех инфо
		else if(pAVR->touch.x >= x && pAVR->touch.x <= 320 && pAVR->touch.y >= y + (yInc * 7) && pAVR->touch.y <= y + (yInc * 8)) {
			menuChangeState(GET_V_MENU);	// Напряжения инфо
			return;
		}	
	}

	if(HAL_GetTick() - time_update < 1000)	return;
	time_update = HAL_GetTick();
	
	//------------ Моточасы -----------------------
	snprintf(buf, BUF_LEN, "Моточасы инфо");
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ ТО -----------------------
	snprintf(buf, BUF_LEN, "ТО инфо");
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ Проведение ТО-----------------------
	snprintf(buf, BUF_LEN, "Провести ТО");
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ Отключение эл-ва -----------------------
	snprintf(buf, BUF_LEN, "Отключение эл-ва инфо");
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	//------------ Запуск ДВС -----------------------
	snprintf(buf, BUF_LEN, "Запуск ДВС инфо");
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
	
	//------------ Напряжения инфо -----------------------
	snprintf(buf, BUF_LEN, "Тех инфо");
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc;
	
	
	//------------ слева снизу иконка перехода на шаг назад -----------------------
	ILI9341_Draw_Rectangle(5, 200, 30, 5, WHITE);
	ILI9341_Draw_Rectangle(5, 210, 30, 5, WHITE);
	ILI9341_Draw_Rectangle(5, 220, 30, 5, WHITE);
}

char header[BUF_LEN] = {0,};
// вывод моточасов
void engineHoursGet (void)
{
	snprintf(header, BUF_LEN, "Моточасы инфо"); 
	snprintf(buf, BUF_LEN, "Моточасы всего     %d\nМоточасы после ТО  %d", 
																																				pAVR->sdParams.engineHoursTotal, 
																																				pAVR->sdParams.engineHoursTO);		
	notification(header, buf, 0, SECOND_MENU);
}

// вывод информации о ТО
void serviseWorkGet(void)
{
	snprintf(header, BUF_LEN, "ТО инфо"); 
	snprintf(buf, BUF_LEN, "Последнее ТО    %d.%d.%d\nСлед. ТО        %d.%d.%d\nМот. час. до ТО %d", pAVR->sdParams.dateLastTO, 
																																																	pAVR->sdParams.monthLastTO, 
																																																	pAVR->sdParams.yearLastTO, 
																																																	pAVR->sdParams.dateNextTO, 
																																																	pAVR->sdParams.monthNextTO, 
																																																	pAVR->sdParams.yearNextTO, 
																																																	pAVR->sdParams.hoursBeforeTO);		
	notification(header, buf, 0, SECOND_MENU);
}

// вывод информации о отключении эл-ва
void powerOutageGet(void)
{
	char buf[160] = {0,};
	snprintf(header, BUF_LEN, "Откл эл-ва инфо"); 
	snprintf(buf, 160, "Последнее отключение\nВремя          %d.%d.%d\nДата           %d.%d.%d\nБез эл-ва        %d\nВсего без эл-ва  %d", 
																																																																				pAVR->sdParams.hoursWithoutElectric, 
																																																																				pAVR->sdParams.minutesWithoutElectric, 
																																																																				pAVR->sdParams.secondsWithoutElectric, 
																																																																				pAVR->sdParams.dateWithoutElectric, 
																																																																				pAVR->sdParams.monthWithoutElectric, 
																																																																				pAVR->sdParams.yearWithoutElectric, 
																																																																				pAVR->sdParams.hoursLastWithoutElectric, 
																																																																				pAVR->sdParams.hoursALLWithoutElectric);		
	notification(header, buf, 0, SECOND_MENU);
}

// вывод информации о запуске двигателя
void startEngineGet(void)
{
	snprintf(header, BUF_LEN, "Запуск ДВС инфо"); 
	snprintf(buf, BUF_LEN, "Ко-во запусков  %d\nПопытки запуска %d", pAVR->sdParams.numSuccessLaunch, 
																																		pAVR->sdParams.numLaunchAttempt);		
	notification(header, buf, 0, SECOND_MENU);
}

// меню ручного переключения реле (мотора)
static uint8_t flag_exit = RESET;
static uint8_t flag_change = RESET;

static uint8_t flag_main_rele = RESET;
static uint8_t flag_zazhig_rele = RESET;
static uint8_t flag_starter_rele = RESET;
static uint8_t flag_podsos_rele = RESET;

void manualRelaySwitchMenu(void)
{
	uint16_t y = 5;				// начальные координаты
	uint16_t x = 5;				// начальные координаты
	uint8_t yInc = 25;		// на сколько опускать каждую строку
	uint8_t status;
	
	// проверка подтверждения выхода
	if(flag_exit)
	{
		status = confirmClick("Выйти?");
		if(status == YES)	{
			flag_main_rele = RESET;
			flag_zazhig_rele = RESET;
			flag_starter_rele = RESET;
			flag_podsos_rele = RESET;
			flag_exit = RESET;
			flag_change = RESET;
			RELE_STARTER_OFF();
			RELE_PODSOS_OFF();
			menuChangeState(SECOND_MENU);
		}
		else if(status == NO)	{
			flag_exit = RESET;
			ILI9341_Fill_Screen(MYFON);	// залить экран
		}
		return;
	}
	else if(flag_main_rele)
	{
		status = confirmClick("Переключить главное реле?");
		if(status == YES)	{
			flag_main_rele = RESET;
			if(!flag_change){
				flag_change = SET;
				pAVR->avr_states.powerAutoManual = AVR_MANUAL;
				setWarn(WARN_MANUAL_CONTROL_EN);
			}
			else
				ILI9341_Fill_Screen(MYFON);	// залить экран
				
			if(HAL_GPIO_ReadPin(RELE_OBSH_GPIO_Port, RELE_OBSH_Pin)){
				RELE_OBSH_ON();
				recLog("Пользователь - включено главное реле");
			}
			else
				RELE_OBSH_OFF();
				recLog("Пользователь - отключено главное реле");
		}
		else if(status == NO)	{
			flag_main_rele = RESET;
			ILI9341_Fill_Screen(MYFON);	// залить экран
		}
		return;
	}
	else if(flag_zazhig_rele)
	{
		status = confirmClick("Переключить реле зажигания?");
		if(status == YES)	{
			flag_zazhig_rele = RESET;
			if(!flag_change){
				flag_change = SET;
				pAVR->avr_states.powerAutoManual = AVR_MANUAL;
				setWarn(WARN_MANUAL_CONTROL_EN);
			}
			else
				ILI9341_Fill_Screen(MYFON);	// залить экран
				
			if(HAL_GPIO_ReadPin(RELE_OBSH_GPIO_Port, RELE_ZAJIG_Pin)){
				RELE_ZAJIG_OFF();
				recLog("Пользователь - выключено реле зажигания");
			}
			else
				RELE_ZAJIG_ON();
				recLog("Пользователь - включено реле зажигания");
		}
		else if(status == NO)	{
			flag_zazhig_rele = RESET;
			ILI9341_Fill_Screen(MYFON);	// залить экран
		}
		return;
	}
	else if(flag_starter_rele)
	{
		status = confirmClick("Переключить реле стартера?");
		if(status == YES)	{
			flag_starter_rele = RESET;
			if(!flag_change){
				flag_change = SET;
				pAVR->avr_states.powerAutoManual = AVR_MANUAL;
				setWarn(WARN_MANUAL_CONTROL_EN);
			}
			else
				ILI9341_Fill_Screen(MYFON);	// залить экран
				
			if(HAL_GPIO_ReadPin(RELE_STARTER_GPIO_Port, RELE_STARTER_Pin)){
				RELE_STARTER_OFF();
				recLog("Пользователь - выключено реле стартера");
			}
			else
				RELE_STARTER_ON();
				recLog("Пользователь - включено реле стартера");
		}
		else if(status == NO)	{
			flag_starter_rele = RESET;
			ILI9341_Fill_Screen(MYFON);	// залить экран
		}
		return;
	}
	else if(flag_podsos_rele)
	{
		status = confirmClick("Переключить реле подсоса?");
		if(status == YES)	{
			flag_podsos_rele = RESET;
			if(!flag_change){
				flag_change = SET;
				pAVR->avr_states.powerAutoManual = AVR_MANUAL;
				setWarn(WARN_MANUAL_CONTROL_EN);
			}
			else
				ILI9341_Fill_Screen(MYFON);	// залить экран
				
			if(HAL_GPIO_ReadPin(RELE_PODSOS_GPIO_Port, RELE_PODSOS_Pin)){
				RELE_PODSOS_OFF();
				recLog("Пользователь - выключено реле подсоса");
			}
			else
				RELE_PODSOS_ON();
				recLog("Пользователь - включено реле подсоса");
		}
		else if(status == NO)	{
			flag_podsos_rele = RESET;
			ILI9341_Fill_Screen(MYFON);	// залить экран
		}
		return;
	}
	
	//------------ обработка тач -----------------------		
	// если отпущен после нажатия или после длительного нажатия
	if(getTouch() == NO_PRESS || getTouch() == NO_LONG_PRESS)
	{
		// если нажатие в области кнопки назад
		if(pAVR->touch.x >= 250 && pAVR->touch.x <= 315 && pAVR->touch.y >= 200 && pAVR->touch.y <= 235) {
			flag_exit = SET;
			return;
		}	
		// если нажатие в области главное реле
		else if(pAVR->touch.x >= x && pAVR->touch.x <= 315 && pAVR->touch.y >= y + (yInc * 1) + 10 && pAVR->touch.y <= y + (yInc * 2) + 10) {
			flag_main_rele = SET;
			return;
		}	
		// если нажатие в области реле зажигания
		else if(pAVR->touch.x >= x && pAVR->touch.x <= 315 && pAVR->touch.y >= y + (yInc * 2) + 10 && pAVR->touch.y <= y + (yInc * 3) + 10) {
			flag_zazhig_rele = SET;
			return;
		}	
		// если нажатие в области реле стартера
		else if(pAVR->touch.x >= x && pAVR->touch.x <= 315 && pAVR->touch.y >= y + (yInc * 3) + 10 && pAVR->touch.y <= y + (yInc * 4) + 10) {
			flag_starter_rele = SET;
			return;
		}	
		// если нажатие в области реле подсоса
		else if(pAVR->touch.x >= x && pAVR->touch.x <= 315 && pAVR->touch.y >= y + (yInc * 4) + 10 && pAVR->touch.y <= y + (yInc * 5) + 10) {
			flag_podsos_rele = SET;
			return;
		}	
	}


	if(HAL_GetTick() - time_update < 1000)	return;
	time_update = HAL_GetTick();
	
	
	// ------------ кнопка "назад" ------------
	ILI9341_Draw_Filled_Rectangle_Coord(250, 200, 315, 235, NAVY);
	snprintf(buf, BUF_LEN, "Назад"); 
	ILI9341_WriteString(255, 210, buf, Font_11x18, WHITE, NAVY);
	
	
	// ------------ заголовок ------------
	snprintf(buf, BUF_LEN, "Управление реле мотора");	
	ILI9341_WriteString(30, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc + 10;
	
	snprintf(buf, BUF_LEN, "Реле главное"); 
	ILI9341_WriteString(x+35, y, buf, Font_11x18, WHITE, MYFON);
	ILI9341_Draw_Hollow_Circle(x+10, y+10, 9, WHITE);
	ILI9341_Draw_Hollow_Circle(x+10, y+10, 10, WHITE);	
	if(!HAL_GPIO_ReadPin(RELE_OBSH_GPIO_Port, RELE_OBSH_Pin))
		ILI9341_Draw_Filled_Circle(x+10, y+10, 5, WHITE);
	y = y + yInc;
		
	snprintf(buf, BUF_LEN, "Реле зажигания"); 
	ILI9341_WriteString(x+35, y, buf, Font_11x18, WHITE, MYFON);
	ILI9341_Draw_Hollow_Circle(x+10, y+10, 9, WHITE);
	ILI9341_Draw_Hollow_Circle(x+10, y+10, 10, WHITE);	
	if(HAL_GPIO_ReadPin(RELE_OBSH_GPIO_Port, RELE_ZAJIG_Pin))
		ILI9341_Draw_Filled_Circle(x+10, y+10, 5, WHITE);
	y = y + yInc;
	
	snprintf(buf, BUF_LEN, "Реле стартера"); 
	ILI9341_WriteString(x+35, y, buf, Font_11x18, WHITE, MYFON);
	ILI9341_Draw_Hollow_Circle(x+10, y+10, 9, WHITE);
	ILI9341_Draw_Hollow_Circle(x+10, y+10, 10, WHITE);	
	if(HAL_GPIO_ReadPin(RELE_STARTER_GPIO_Port, RELE_STARTER_Pin))
		ILI9341_Draw_Filled_Circle(x+10, y+10, 5, WHITE);
	y = y + yInc;
	
	snprintf(buf, BUF_LEN, "Реле подсоса"); 
	ILI9341_WriteString(x+35, y, buf, Font_11x18, WHITE, MYFON);
	ILI9341_Draw_Hollow_Circle(x+10, y+10, 9, WHITE);
	ILI9341_Draw_Hollow_Circle(x+10, y+10, 10, WHITE);	
	if(HAL_GPIO_ReadPin(RELE_PODSOS_GPIO_Port, RELE_PODSOS_Pin))
		ILI9341_Draw_Filled_Circle(x+10, y+10, 5, WHITE);
	y = y + yInc;
}
		
// меню показа напряжений, температуры места на sd карте
void get_v_menu(void)
{
	uint16_t y = 5;				// начальные координаты
	uint16_t x = 10;				// начальные координаты
	uint8_t yInc = 23;		// на сколько опускать каждую строку

	//------------ обработка тач -----------------------		
	// если отпущен после нажатия или после длительного нажатия
	if(getTouch() == NO_PRESS || getTouch() == NO_LONG_PRESS)
	{
		// если нажатие в области кнопки назад
		if(pAVR->touch.x >= 250 && pAVR->touch.x <= 315 && pAVR->touch.y >= 200 && pAVR->touch.y <= 235) {
			menuChangeState(SECOND_MENU);
			return;
		}	
	}

	if(HAL_GetTick() - time_update < 500)	return;
	time_update = HAL_GetTick();
	
	
	// ------------ кнопка "назад" ------------
	ILI9341_Draw_Filled_Rectangle_Coord(250, 200, 315, 235, NAVY);
	snprintf(buf, BUF_LEN, "Назад"); 
	ILI9341_WriteString(255, 210, buf, Font_11x18, WHITE, NAVY);
	
	
	// ------------ заголовок ------------
	snprintf(buf, BUF_LEN, "Напряжения инфо");	
	ILI9341_WriteString(60, y, buf, Font_11x18, WHITE, MYFON);
	y += yInc+5;
	
	// ------------ напряжения ------------
	snprintf(buf, BUF_LEN, "Внешнее            %.2f", pAVR->v_t.v_out); 
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y = y + yInc;
	
	snprintf(buf, BUF_LEN, "Акум               %.2f", pAVR->v_t.v_bat); 
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y = y + yInc;
	
	snprintf(buf, BUF_LEN, "На обмотке мотора  %.2f", pAVR->v_t.v_motor); 
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y = y + yInc;
	
	snprintf(buf, BUF_LEN, "На реле стартера   %.2f", pAVR->v_t.v_rele_starter); 
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y = y + yInc;
	
	snprintf(buf, BUF_LEN, "Опора проца        %.2f", pAVR->v_t.v_opora); 
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y = y + yInc;
	
	snprintf(buf, BUF_LEN, "Питание проца      %.2f", pAVR->v_t.v_cpu); 
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y = y + yInc;
	
	snprintf(buf, BUF_LEN, "Температура проца  %.2f", pAVR->v_t.t_cpu); 
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y = y + yInc;
	
	snprintf(buf, BUF_LEN, "Место на sd %dкб", FreeSpace); 
	ILI9341_WriteString(x, y, buf, Font_11x18, WHITE, MYFON);
	y = y + yInc;

}


