
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "init.h"

#include "warn_err.h"
#include "menu_main.h"
#include "ILI9341_GFX.h"
#include "touch.h"
#include "fatfs_sd.h"
#include "work.h"
#include "img.h"


static char historyParamBuf[BUF_LEN_SD_PARAM] = {0, };
static uint32_t numbers[NUM_VARIABLES_HISTORI]; // Массив для чисел
uint8_t initDevice(void)
{
	// прочитать дату, время
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); 				
	HAL_RTC_GetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN);
	
	// ------------- структура прибора ------------
	pAVR->touch.flag_press = 0;
	pAVR->touch.flag_release = 0;
	pAVR->touch.x = 0;
	pAVR->touch.y = 0;
	pAVR->avr_states.power_grid_mode 	= POWER_IS_OFF;		// флаг о питании дома
	pAVR->avr_states.flagCharge 			= RESET;					// флаг что заряжка откл
	pAVR->avr_states.powerAutoManual 	= AVR_AUTO;				// автоматический режим
	pAVR->avr_states.extPowerSupply 	= EXT_POWER_OFF;	// нет
	resetErrors();
	resetWarning();
	pAVR->engine.status = ENGINE_STOPPED;
	pAVR->engine.launchAttempts = 0;
	pAVR->engine.flagTimeout = RESET;

	HAL_IWDG_Refresh(&hiwdg);
	
	// ------------- переферия ------------
	outputInit();	// выхода (светодиоды, реле и.т.д.)
	HAL_IWDG_Refresh(&hiwdg);
	
	// ------------- дисплей ------------
	initTFT();
	menuChangeState(MAIN_MENU);
	HAL_IWDG_Refresh(&hiwdg);
	
	// ------------- sd card ------------
	SD_Init();
	
	// проверка ошибок перезагрузки
	if(RTC->BKP0R == U_CONFIG_WACH_DOG_SIGNATURE)
		setErr(ERR_WATCH_DOG);
	if(RTC->BKP0R == U_CONFIG_HARD_FAULT_SIGNATURE)
		setErr(ERR_HARD_RESET);	
		
	set_BKP0R(U_CONFIG_WACH_DOG_SIGNATURE);
	HAL_IWDG_Refresh(&hiwdg);
	
	// ------------- инициализация данных с sd card ------------
	if(getFillStructureStoryParameters()) 
	{
		// если файл еще не проинициализирован - проинициализировать
		if(pAVR->sdParams.checkNum != INIT_HISTORY_FILE_SIGNATURE)
		{
			// инициализация структуры
			
			// проверка
			pAVR->sdParams.checkNum									= INIT_HISTORY_FILE_SIGNATURE;	// проверочное число инициализации

			// моточасы				
			pAVR->sdParams.engineHoursTotal					= 0;	// моточасы всего
			pAVR->sdParams.engineMinutesTotal				= 0;	// мотоминуты всего
			pAVR->sdParams.engineHoursTO						= 0;	// моточасы после ТО
			pAVR->sdParams.engineMinutesTO					= 0;	// мотоминуты после ТО
			pAVR->sdParams.hoursBeforeTO						= 0;	// моточасы до ТО
			pAVR->sdParams.minutesBeforeTO					= 0;	// мотоминуты до ТО

			// ТО		
			pAVR->sdParams.hoursLastTO							= sTime.Hours;				// час последнего ТО
			pAVR->sdParams.minutesLastTO						= sTime.Minutes;			// минуты последнего ТО
			pAVR->sdParams.secondsLastTO						= sTime.Seconds;			// секунды последнего ТО
			pAVR->sdParams.dateLastTO								= DateToUpdate.Date;	// дата последнего ТО
			pAVR->sdParams.monthLastTO							= DateToUpdate.Month;	// месяц последнего ТО
			pAVR->sdParams.yearLastTO								= DateToUpdate.Year;	// год последнего ТО

			//
			pAVR->sdParams.hoursNextTO							= 0;	// час следующего ТО
			pAVR->sdParams.minutesNextTO						= 0;	// минуты следующего ТО
			pAVR->sdParams.secondsNextTO						= 0;	// секунды следующего ТО
			pAVR->sdParams.dateNextTO								= 0;	// дата следующего ТО
			pAVR->sdParams.monthNextTO							= 0;	// месяц следующего ТО
			pAVR->sdParams.yearNextTO								= 0;	// год следующего ТО

			// отключение эл-ва
			pAVR->sdParams.hoursWithoutElectric			= 0;	// час последнего отключения
			pAVR->sdParams.minutesWithoutElectric		= 0;	// минуты последнего отключения
			pAVR->sdParams.secondsWithoutElectric		= 0;	// секунды последнего отключения
			pAVR->sdParams.dateWithoutElectric			= 0;	// дата последнего отключения
			pAVR->sdParams.monthWithoutElectric			= 0;	// месяц последнего отключения
			pAVR->sdParams.yearWithoutElectric			= 0;	// год последнего отключения

			pAVR->sdParams.minutesLastWithoutElectric = 0;// минуты без эл-ва за последний раз	
			pAVR->sdParams.hoursALLWithoutElectric		= 0;	// общее кол-во часов без эл-ва
			pAVR->sdParams.minutesALLWithoutElectric	= 0;	// общее кол-во минут без эл-ва
			
			// запуск ДВС инфо
			pAVR->sdParams.numSuccessLaunch					= 0;	// кол-во удачных запусков
			pAVR->sdParams.numLaunchAttempt					= 0;	// кол-во попыток запуска
			
			// обновить инфо о ТО - в данном случае проинициализировать
			updateInfoTO();
		}
		else{
			checkInfoTO();
		}
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	// ------------- ацп ------------
	//HAL_ADCEx_Calibration_Start(&hadc1);	// почемуто нет такой функции
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&AVR.adc, ADC_CHANELS);	// запуск ацп
	for(uint8_t i = 0; i < 50; i++){
		HAL_Delay(10);
		dataCalcADC();
	}
		
	// определить какое питание и выставить флаг
	if(pAVR->v_t.v_out > U_V_OUT_MIN)
		pAVR->avr_states.extPowerSupply = EXT_POWER_ON;
	else
		pAVR->avr_states.extPowerSupply = EXT_POWER_OFF;
		
	if(pAVR->v_t.v_bat < U_AKB_MIN_0)
		setErr(ERR_LOW_VOLTAGE_AKB);
	
//	// для проверки ошибок и предупреждений	
//	for(uint8_t i = 0; i < MAX_ERR_AND_WARN; i++)	{
//		pAVR->err.counter++;
//		pAVR->err.array_flags[i] = SET;
//		pAVR->warn.counter++;
//		pAVR->warn.array_flags[i] = SET;
//	}

	recLog("Start CPU");
	return 1;
}


// читает параметры  с sd карты
// переводит строку, прочитанную с sd карты в массив чисел параметров
uint8_t getFillStructureStoryParameters(void)
{
	char *end = historyParamBuf;  // Указатель для отслеживания конца числа 
	uint8_t i = 0;
	
	// очистить буфер
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	pAVR->sdParams.checkNum									= numbers[0];		// для дальнейшей проверки в случае преждевременного  выхода из функции
	
	if(!readFileSdCard("historyStorageFile.txt", historyParamBuf)) {
		setErr(ERR_SD_CARD);
		return 0;
	}
	
	// очистить массив чисел
	for(i = 0; i < NUM_VARIABLES_HISTORI; i++)
		numbers[i] = 0;
		
	i = 0;
	while (*end) {
			// Преобразуем подстроку от `str` до `end` в число
			numbers[i++] = strtol(end, &end, 10);
			// Пропускаем разделитель (запятую)
			if (*end == ',') {
					end++;
			}
			if(*end == '\0')
						break;
			if(i == NUM_VARIABLES_HISTORI)
				break;
	}
			
	i = 0;
	// перенести параметры в структуру
	pAVR->sdParams.checkNum									= numbers[i++];
	pAVR->sdParams.engineHoursTotal					= numbers[i++];
	pAVR->sdParams.engineMinutesTotal				= numbers[i++];
	pAVR->sdParams.engineMinutesTO					= numbers[i++];
	pAVR->sdParams.engineHoursTO						= numbers[i++];
	pAVR->sdParams.minutesBeforeTO					= numbers[i++];
	pAVR->sdParams.hoursBeforeTO						= numbers[i++];
	pAVR->sdParams.hoursLastTO							= numbers[i++];
	pAVR->sdParams.minutesLastTO						= numbers[i++];
	pAVR->sdParams.secondsLastTO						= numbers[i++];
	pAVR->sdParams.dateLastTO								= numbers[i++];
	pAVR->sdParams.monthLastTO							= numbers[i++];
	pAVR->sdParams.yearLastTO								= numbers[i++];
	pAVR->sdParams.hoursNextTO							= numbers[i++];
	pAVR->sdParams.minutesNextTO						= numbers[i++];
	pAVR->sdParams.secondsNextTO						= numbers[i++];
	pAVR->sdParams.dateNextTO								= numbers[i++];
	pAVR->sdParams.monthNextTO							= numbers[i++];
	pAVR->sdParams.yearNextTO								= numbers[i++];
	pAVR->sdParams.hoursWithoutElectric			= numbers[i++];
	pAVR->sdParams.minutesWithoutElectric		= numbers[i++];
	pAVR->sdParams.secondsWithoutElectric		= numbers[i++];
	pAVR->sdParams.dateWithoutElectric			= numbers[i++];
	pAVR->sdParams.monthWithoutElectric			= numbers[i++];
	pAVR->sdParams.yearWithoutElectric			= numbers[i++];
	pAVR->sdParams.minutesLastWithoutElectric = numbers[i++];
	pAVR->sdParams.hoursALLWithoutElectric	= numbers[i++];
	pAVR->sdParams.minutesALLWithoutElectric = numbers[i++];
	pAVR->sdParams.numSuccessLaunch					= numbers[i++];
	pAVR->sdParams.numLaunchAttempt					= numbers[i++];
	
	return 1;
}

// записывает структуру истории на sd
uint8_t setFillStructureStoryParameters(void)
{	
	uint8_t i = 0;
	// очистить массив чисел
	for(i = 0; i < NUM_VARIABLES_HISTORI; i++)
		numbers[i] = 0;
		
	i = 0;	
	// проверка
	numbers[i++] = pAVR->sdParams.checkNum;										// проверочное число инициализации
			
	// моточасы				
	numbers[i++] = pAVR->sdParams.engineHoursTotal;						// моточасы всего
	numbers[i++] = pAVR->sdParams.engineMinutesTotal;					// мотоминуты всего
	numbers[i++] = pAVR->sdParams.engineHoursTO;							// моточасы после ТО
	numbers[i++] = pAVR->sdParams.engineMinutesTO;						// мотоминуты после ТО
	numbers[i++] = pAVR->sdParams.hoursBeforeTO;							// моточасы до ТО
	numbers[i++] = pAVR->sdParams.minutesBeforeTO;						// мотоминуты до ТО

	// ТО		
	numbers[i++] = pAVR->sdParams.hoursLastTO;								// час последнего ТО
	numbers[i++] = pAVR->sdParams.minutesLastTO;							// минуты последнего ТО
	numbers[i++] = pAVR->sdParams.secondsLastTO;							// секунды последнего ТО
	numbers[i++] = pAVR->sdParams.dateLastTO;									// дата последнего ТО
	numbers[i++] = pAVR->sdParams.monthLastTO;								// месяц последнего ТО
	numbers[i++] = pAVR->sdParams.yearLastTO;									// год последнего ТО

	numbers[i++] = pAVR->sdParams.hoursNextTO;								// час следующего ТО
	numbers[i++] = pAVR->sdParams.minutesNextTO;							// минуты следующего ТО
	numbers[i++] = pAVR->sdParams.secondsNextTO;							// секунды следующего ТО
	numbers[i++] = pAVR->sdParams.dateNextTO;								// дата следующего ТО
	numbers[i++] = pAVR->sdParams.monthNextTO;								// месяц следующего ТО
	numbers[i++] = pAVR->sdParams.yearNextTO;								// год следующего ТО

	// отключение эл-ва
	numbers[i++] = pAVR->sdParams.hoursWithoutElectric;			// час последнего отключения
	numbers[i++] = pAVR->sdParams.minutesWithoutElectric;		// минуты последнего отключения
	numbers[i++] = pAVR->sdParams.secondsWithoutElectric;		// секунды последнего отключения
	numbers[i++] = pAVR->sdParams.dateWithoutElectric;				// дата последнего отключения
	numbers[i++] = pAVR->sdParams.monthWithoutElectric;			// месяц последнего отключения
	numbers[i++] = pAVR->sdParams.yearWithoutElectric;				// год последнего отключения

	numbers[i++] = pAVR->sdParams.minutesLastWithoutElectric;	// минуты без эл-ва за последний раз	
	numbers[i++] = pAVR->sdParams.hoursALLWithoutElectric;		// общее кол-во часов без эл-ва
	numbers[i++] = pAVR->sdParams.minutesALLWithoutElectric;	// общее кол-во минут без эл-ва

	// запуск ДВС инфо
	numbers[i++] = pAVR->sdParams.numSuccessLaunch;					// кол-во удачных запусков
	numbers[i++] = pAVR->sdParams.numLaunchAttempt;					// кол-во попыток запуска
	
	// очистить буфер
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 

	// переместить массив чисел в строку
	for (i = 0; i < NUM_VARIABLES_HISTORI; i++) {
			// Вычисляем позицию: добавляем к текущей позиции в строке длину уже записанной части
			sprintf(historyParamBuf + strlen(historyParamBuf), "%d,", numbers[i]);
	}
	
	// записать в файл на флеш
	if(!recFileSdCard ("historyStorageFile.txt", historyParamBuf, 1)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	// ---------- запись той же структуры, но для пользовательского чтения-------
	// заголовок
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "( %02d:%02d:%02d  %02d-%02d-20%d )\n", sTime.Hours, sTime.Minutes, sTime.Seconds, DateToUpdate.Date, DateToUpdate.Month, DateToUpdate.Year);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 1)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	// моточасы
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "\n---Моточасы---\n");
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%d - моточасы всего\n", pAVR->sdParams.engineHoursTotal);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%d - моточасы после ТО\n", pAVR->sdParams.engineHoursTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%d - моточасы до ТО\n", pAVR->sdParams.hoursBeforeTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	// ТО
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "\n---ТО---\n");
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - час последнего ТО\n", pAVR->sdParams.hoursLastTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - минуты последнего ТО\n", pAVR->sdParams.minutesLastTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - секунды последнего ТО\n\n", pAVR->sdParams.secondsLastTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - дата последнего ТО\n", pAVR->sdParams.dateLastTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - месяц последнего ТО\n", pAVR->sdParams.monthLastTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - год последнего ТО\n\n", pAVR->sdParams.yearLastTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - час следующего ТО\n", pAVR->sdParams.hoursNextTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - минуты следующего ТО\n", pAVR->sdParams.minutesNextTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - секунды следующего ТО\n\n", pAVR->sdParams.secondsNextTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - дата следующего ТО\n", pAVR->sdParams.dateNextTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - месяц следующего ТО\n", pAVR->sdParams.monthNextTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - год следующего ТО\n", pAVR->sdParams.yearNextTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	// отключение эл-ва
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "\n---отключение эл-ва---\n");
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - час последнего отключения\n", pAVR->sdParams.hoursWithoutElectric);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - минуты последнего отключения\n", pAVR->sdParams.minutesWithoutElectric);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - секунды последнего отключения\n\n", pAVR->sdParams.secondsWithoutElectric);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - дата последнего отключения\n", pAVR->sdParams.dateWithoutElectric);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - месяц последнего отключения\n", pAVR->sdParams.monthWithoutElectric);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - год последнего отключения\n\n", pAVR->sdParams.yearWithoutElectric);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%d - минуты без эл-ва за последний раз\n", pAVR->sdParams.minutesLastWithoutElectric);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%d - общее кол-во часов без эл-ва\n", pAVR->sdParams.hoursALLWithoutElectric);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%d - общее кол-во минут без эл-ва\n", pAVR->sdParams.minutesALLWithoutElectric);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	// запуск ДВС инфо
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "\n---запуск ДВС инфо---\n");
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%d - кол-во удачных запусков\n", pAVR->sdParams.numSuccessLaunch);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%d - кол-во попыток запуска\n", pAVR->sdParams.numLaunchAttempt);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	
	recLog("Структура истории на sd карте обновлена");
	return 1;
}


// обновляет инфо о
// след ТО
void updateInfoTO (void)
{
	// прочитать дату, время
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); 				
	HAL_RTC_GetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN);
	
	// запись даты ТО
	pAVR->sdParams.hoursLastTO		= sTime.Hours;									// час последнего ТО
	pAVR->sdParams.minutesLastTO	= sTime.Minutes;								// минуты последнего ТО
	pAVR->sdParams.secondsLastTO	= sTime.Seconds;								// секунды последнего ТО
	pAVR->sdParams.dateLastTO			= DateToUpdate.Date;						// дата последнего ТО
	pAVR->sdParams.monthLastTO		= DateToUpdate.Month;						// месяц последнего ТО
	pAVR->sdParams.yearLastTO			= DateToUpdate.Year;						// год последнего ТО
	
	// Перевожу в Unix-время (в UTC)
	time_t unix_realTime = realToUnix();
	
	unix_realTime += INTERVAL_DATA_TO_UNIX;	// прибавляю интервал ТО
	
	// перевод обратно в дату и время
	struct tm *time_struct = localtime(&unix_realTime); // Получаем структуру в UTC
	if (time_struct != NULL) 
	{
			// дата и время след ТО
			pAVR->sdParams.hoursNextTO		= time_struct->tm_hour;
			pAVR->sdParams.minutesNextTO	= time_struct->tm_min;
			pAVR->sdParams.secondsNextTO	= time_struct->tm_sec;
			pAVR->sdParams.dateNextTO			= time_struct->tm_mday;
			pAVR->sdParams.monthNextTO		= time_struct->tm_mon+1;
			pAVR->sdParams.yearNextTO			= time_struct->tm_year + 1900 - 2000;	// т.к. хранятся только 2 последние цифры года
			
			// обнуление моточасов
			pAVR->sdParams.engineHoursTO 		= 0;
			pAVR->sdParams.engineMinutesTO 	= 0;
			pAVR->sdParams.hoursBeforeTO 		= INTERVAL_TO_HOURS;
			pAVR->sdParams.minutesBeforeTO 	= 0;
			
			// записать в файл на флеш
			setFillStructureStoryParameters();
   }
}

void checkInfoTO (void)
{
	struct tm tm = {0}; // Инициализируем нулями
		
	// прочитать дату, время
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); 				
	HAL_RTC_GetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN);

	//------ реальное время ---------
	// Заполняем структуру временем 
	tm.tm_sec = sTime.Seconds;
	tm.tm_min = sTime.Minutes;
	tm.tm_hour = sTime.Hours;
	// дата
	tm.tm_mday = DateToUpdate.Date;
	tm.tm_mon = DateToUpdate.Month;
	tm.tm_year = DateToUpdate.Year;

	// Переводим в Unix-время (в UTC)
	time_t unix_realTime = mktime(&tm);
	
	//------ время последнего ТО---------
	// Заполняем структуру временем 
	tm.tm_sec = pAVR->sdParams.secondsLastTO;
	tm.tm_min = pAVR->sdParams.minutesLastTO;
	tm.tm_hour = pAVR->sdParams.hoursLastTO;
	// дата
	tm.tm_mday = pAVR->sdParams.dateLastTO;
	tm.tm_mon = pAVR->sdParams.monthLastTO;
	tm.tm_year = pAVR->sdParams.yearLastTO;

	// Переводим в Unix-время (в UTC)
	time_t unix_lastTO = mktime(&tm);
		
		
	// если пора делать ТО по истечению даты
	if((unix_realTime > (unix_lastTO + INTERVAL_DATA_TO_UNIX)) ||
		(pAVR->sdParams.engineHoursTO > INTERVAL_TO_HOURS))					// или по истечению моточасов
	{
		setWarn(WARN_NECESSITY_TECH_INSP);
	}
}

uint8_t initTFT(void)
{
	ledTFTInit();		// подсветка дисплея
	
	__HAL_SPI_ENABLE(DISP_SPI_PTR); // включаем SPI

  DISP_CS_UNSELECT;
  TOUCH_CS_UNSELECT; // это нужно только если есть тач

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ILI9341_Init(); // инициализация дисплея

  ILI9341_Set_Rotation(SCREEN_VERTICAL_2); // установка ориентации экрана (варианты в файле ILI9341_GFX.h)

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ILI9341_Fill_Screen(MYFON); // заливка всего экрана цветом (цвета в файле ILI9341_GFX.h)
	
	// заставка загрузки
//	uint32_t size_img = sizeof(img_logo); // размер картинки в байтах (картинка лежит в файле img.h)
//	ILI9341_Draw_Image(img_logo, 60, 7, IMG_WIDTH, IMG_HEIGHT, size_img); // вывести в центре
	
//???	// полоса загрузки
//  for(uint16_t i = 0; i < ILI9341_SCREEN_HEIGHT * 0.8; i++)
//  {
//		HAL_IWDG_Refresh(&hiwdg);
//		ILI9341_Draw_Rectangle(32, 220, i, 8, OLIVE);
//		HAL_Delay(20);
//  }
	
	return 1;
}


uint8_t ledTFTInit(void)
{
	return ledTFT_ON_OFF(SET);
}

uint8_t ledTFT_ON_OFF(uint8_t status)
{
	if(status == SET)
	{
		BRIGHTNESS_TFT(MAX_BRIGHTNESS);		// яркость на полную
		HAL_TIM_PWM_Start(TIM_LED_TFT, TIM_CHANEL_LED_TFT);
	}
	else
	{
		ili9341_SleepOn();
		BRIGHTNESS_TFT(NULL_BRIGHTNESS);
		HAL_TIM_PWM_Stop(TIM_LED_TFT, TIM_CHANEL_LED_TFT);
	}
	return 1;
}

uint8_t outputInit(void)
{
	RELE_OBSH_ON();			// вкл питание всех релюх
	RELE_ZAJIG_OFF();		// реле зажигания выкл
	RELE_STARTER_OFF();	// реле стартер выкл
	RELE_PODSOS_OFF();	// реле подсоса выкл
	switchPowerCircuitBreaker(EXTERNAL_POWER);	// питание от внешней сети
	LED_ON();						// светодиод работы вкл
	LED_ERROR_OFF();		// светодиод аварии выкл
	CHARGE_OFF();				// зарядка акб выкл

	return 1;
}
