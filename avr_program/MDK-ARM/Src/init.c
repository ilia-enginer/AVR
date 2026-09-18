
#include <string.h>
#include <stdio.h>

#include "init.h"

#include "warn_err.h"
#include "menu_main.h"
#include "ILI9341_GFX.h"
#include "touch.h"

#include "fatfs_sd.h"

static char historyParamBuf[BUF_LEN_SD_PARAM] = {0, };
static uint32_t numbers[NUM_VARIABLES_HISTORI]; // Массив для чисел
uint8_t initDevice(void)
{
	// прочитать дату, время
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); 				
	HAL_RTC_GetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN);
	
	// ------------- структура прибора ------------
	pAVR->touch.flag_hold = 0;
	pAVR->touch.flag_press = 0;
	pAVR->touch.flag_release = 0;
	pAVR->touch.x = 0;
	pAVR->touch.y = 0;
	pAVR->avr_states.power_grid_mode 	= POWER_IS_OFF;		// флаг о питании дома
	pAVR->avr_states.flagCharge 			= RESET;					// флаг что заряжка откл
	pAVR->avr_states.powerAutoManual 	= AVR_AUTO;				// автоматический режим
	pAVR->avr_states.statusEngine 		= RESET;					// остановлен
	pAVR->avr_states.extPowerSupply 	= EXT_POWER_OFF;	// нет
	resetErrors();
	resetWarning();
	
	// ------------- дисплей ------------
	initTFT();
	menuChangeState(MAIN_MENU);
	
	// ------------- переферия ------------
	outputInit();	// выхода (светодиоды, реле и.т.д.)
	
	// ------------- sd card ------------
	SD_Init();
	
	// ------------- инициализация данных с sd card ------------
	if(getFillStructureStoryParameters()) {
		// если файл еще не проинициализирован - проинициализировать
		if(pAVR->sdParams.checkNum != INIT_HISTORY_FILE_SIGNATURE)
		{
			// инициализация структуры
			
			// проверка
			pAVR->sdParams.checkNum									= INIT_HISTORY_FILE_SIGNATURE;	// проверочное число инициализации
			
			// моточасы				
			pAVR->sdParams.engineHoursTotal					= 0;	// моточасы всего
			pAVR->sdParams.engineHoursTO						= 0;	// моточасы после ТО
			pAVR->sdParams.hoursBeforeTO						= 0;	// моточасы до ТО

			// ТО		
			pAVR->sdParams.hoursLastTO							= sTime.Hours;				// час последнего ТО
			pAVR->sdParams.minutesLastTO						= sTime.Minutes;			// минуты последнего ТО
			pAVR->sdParams.secondsLastTO						= sTime.Seconds;			// секунды последнего ТО
			pAVR->sdParams.dateLastTO								= DateToUpdate.Date;	// дата последнего ТО
			pAVR->sdParams.monthLastTO							= DateToUpdate.Month;	// месяц последнего ТО
			pAVR->sdParams.yearLastTO								= DateToUpdate.Year;	// год последнего ТО

			//??? узнать, записать
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

			pAVR->sdParams.hoursALLWithoutElectric	= 0;	// общее кол-во часов без эл-ва

			// запуск ДВС инфо
			pAVR->sdParams.numSuccessLaunch					= 0;	// кол-во удачных запусков
			pAVR->sdParams.numLaunchAttempt					= 0;	// кол-во попыток запуска
			
			// записать в файл на флеш
			setFillStructureStoryParameters();
		}
	}
	
	// ------------- ацп ------------
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&AVR.adc, ADC_CHANELS);	// запуск ацп
		
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
	}
	
	// перенести параметры в структуру
	pAVR->sdParams.checkNum									= numbers[0];
	pAVR->sdParams.engineHoursTotal					= numbers[1];
	pAVR->sdParams.engineHoursTO						= numbers[2];
	pAVR->sdParams.hoursBeforeTO						= numbers[3];
	pAVR->sdParams.hoursLastTO							= numbers[4];
	pAVR->sdParams.minutesLastTO						= numbers[5];
	pAVR->sdParams.secondsLastTO						= numbers[6];
	pAVR->sdParams.dateLastTO								= numbers[7];
	pAVR->sdParams.monthLastTO							= numbers[8];
	pAVR->sdParams.yearLastTO								= numbers[9];
	pAVR->sdParams.hoursNextTO							= numbers[10];
	pAVR->sdParams.minutesNextTO						= numbers[11];
	pAVR->sdParams.secondsNextTO						= numbers[12];
	pAVR->sdParams.dateNextTO								= numbers[13];
	pAVR->sdParams.monthNextTO							= numbers[14];
	pAVR->sdParams.yearNextTO								= numbers[15];
	pAVR->sdParams.hoursWithoutElectric			= numbers[16];
	pAVR->sdParams.minutesWithoutElectric		= numbers[17];
	pAVR->sdParams.secondsWithoutElectric		= numbers[18];
	pAVR->sdParams.dateWithoutElectric			= numbers[19];
	pAVR->sdParams.monthWithoutElectric			= numbers[20];
	pAVR->sdParams.yearWithoutElectric			= numbers[21];
	pAVR->sdParams.hoursALLWithoutElectric	= numbers[22];
	pAVR->sdParams.numSuccessLaunch					= numbers[23];
	pAVR->sdParams.numLaunchAttempt					= numbers[24];

	return 1;
}

// записывает структуру истории на sd
uint8_t setFillStructureStoryParameters(void)
{	
	// очистить массив чисел
	for(uint8_t i = 0; i < NUM_VARIABLES_HISTORI; i++)
		numbers[i] = 0;
		
	// проверка
	numbers[0] = pAVR->sdParams.checkNum;										// проверочное число инициализации
	
	// моточасы				
	numbers[1] = pAVR->sdParams.engineHoursTotal;						// моточасы всего
	numbers[2] = pAVR->sdParams.engineHoursTO;							// моточасы после ТО
	numbers[3] = pAVR->sdParams.hoursBeforeTO;							// моточасы до ТО

	// ТО		
	numbers[4] = pAVR->sdParams.hoursLastTO;								// час последнего ТО
	numbers[5] = pAVR->sdParams.minutesLastTO;							// минуты последнего ТО
	numbers[6] = pAVR->sdParams.secondsLastTO;							// секунды последнего ТО
	numbers[7] = pAVR->sdParams.dateLastTO;									// дата последнего ТО
	numbers[8] = pAVR->sdParams.monthLastTO;								// месяц последнего ТО
	numbers[9] = pAVR->sdParams.yearLastTO;									// год последнего ТО

	numbers[10] = pAVR->sdParams.hoursNextTO;								// час следующего ТО
	numbers[11] = pAVR->sdParams.minutesNextTO;							// минуты следующего ТО
	numbers[12] = pAVR->sdParams.secondsNextTO;							// секунды следующего ТО
	numbers[13] = pAVR->sdParams.dateNextTO;								// дата следующего ТО
	numbers[14] = pAVR->sdParams.monthNextTO;								// месяц следующего ТО
	numbers[15] = pAVR->sdParams.yearNextTO;								// год следующего ТО

	// отключение эл-ва
	numbers[16] = pAVR->sdParams.hoursWithoutElectric;			// час последнего отключения
	numbers[17] = pAVR->sdParams.minutesWithoutElectric;		// минуты последнего отключения
	numbers[18] = pAVR->sdParams.secondsWithoutElectric;		// секунды последнего отключения
	numbers[19] = pAVR->sdParams.dateWithoutElectric;				// дата последнего отключения
	numbers[20] = pAVR->sdParams.monthWithoutElectric;			// месяц последнего отключения
	numbers[21] = pAVR->sdParams.yearWithoutElectric;				// год последнего отключения

	numbers[22] = pAVR->sdParams.hoursALLWithoutElectric;		// общее кол-во часов без эл-ва

	// запуск ДВС инфо
	numbers[23] = pAVR->sdParams.numSuccessLaunch;					// кол-во удачных запусков
	numbers[24] = pAVR->sdParams.numLaunchAttempt;					// кол-во попыток запуска

	// очистить буфер
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 

	// переместить массив чисел в строку
	for (uint8_t i = 0; i < NUM_VARIABLES_HISTORI; i++) {
			// Вычисляем позицию: добавляем к текущей позиции в строке длину уже записанной части
			sprintf(historyParamBuf + strlen(historyParamBuf), "%d,", numbers[i]);
	}
	
	// записать в файл на флеш
	if(!recFileSdCard ("historyStorageFile.txt", historyParamBuf, 1)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	
	// ---------- запись той же структуры, но для пользовательского чтения-------
	// заголовок
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "( %02d:%02d:%02d  %02d-%02d-20%d )\n", sTime.Hours, sTime.Minutes, sTime.Seconds, DateToUpdate.Date, DateToUpdate.Month, DateToUpdate.Year);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 1)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	// моточасы
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "\n---Моточасы---\n");
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%d - моточасы всего\n", pAVR->sdParams.engineHoursTotal);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%d - моточасы после ТО\n", pAVR->sdParams.engineHoursTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%d - моточасы до ТО\n", pAVR->sdParams.hoursBeforeTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	// ТО
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "\n---ТО---\n");
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - час последнего ТО\n", pAVR->sdParams.hoursLastTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - минуты последнего ТО\n", pAVR->sdParams.minutesLastTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - секунды последнего ТО\n\n", pAVR->sdParams.secondsLastTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - дата последнего ТО\n", pAVR->sdParams.dateLastTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - месяц последнего ТО\n", pAVR->sdParams.monthLastTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - год последнего ТО\n\n", pAVR->sdParams.yearLastTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - час следующего ТО\n", pAVR->sdParams.hoursNextTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - минуты следующего ТО\n", pAVR->sdParams.minutesNextTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - секунды следующего ТО\n\n", pAVR->sdParams.secondsNextTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - дата следующего ТО\n", pAVR->sdParams.dateNextTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - месяц следующего ТО\n", pAVR->sdParams.monthNextTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - год следующего ТО\n", pAVR->sdParams.yearNextTO);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	// отключение эл-ва
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "\n---отключение эл-ва---\n");
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - час последнего отключения\n", pAVR->sdParams.hoursWithoutElectric);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - минуты последнего отключения\n", pAVR->sdParams.minutesWithoutElectric);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - секунды последнего отключения\n\n", pAVR->sdParams.secondsWithoutElectric);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - дата последнего отключения\n", pAVR->sdParams.dateWithoutElectric);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - месяц последнего отключения\n", pAVR->sdParams.monthWithoutElectric);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%02d - год последнего отключения\n\n", pAVR->sdParams.yearWithoutElectric);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%d - общее кол-во часов без эл-ва\n", pAVR->sdParams.hoursALLWithoutElectric);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	// запуск ДВС инфо
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "\n---запуск ДВС инфо---\n");
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%d - кол-во удачных запусков\n", pAVR->sdParams.numSuccessLaunch);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	memset(historyParamBuf,'\0',sizeof(historyParamBuf)); 
	sprintf(historyParamBuf + strlen(historyParamBuf), "%d - кол-во попыток запуска\n", pAVR->sdParams.numLaunchAttempt);
	if(!recFileSdCard ("historyUserFile.txt", historyParamBuf, 0)){
		setErr(ERR_SD_CARD);
		return 0;
	}
	
	recLog("Структура истории на sd карте обновлена");
	return 1;
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
	
//	//???
//	uint32_t size_img = sizeof(img_logo); // размер картинки в байтах (картинка лежит в файле img.h)
//	ILI9341_Draw_Image(img_logo, 60, 7, IMG_WIDTH, IMG_HEIGHT, size_img); // вывести в центре
//	
//	// полоса загрузки
//  for(uint16_t i = 0; i < ILI9341_SCREEN_HEIGHT * 0.8; i++)
//  {
//		ILI9341_Draw_Rectangle(32, 220, i, 8, OLIVE);
//		HAL_Delay(20);
//  }

	return 1;
}


uint8_t ledTFTInit(void)
{
	HAL_TIM_PWM_Start(TIM_LED_TFT, TIM_CHANEL_LED_TFT);
	BRIGHTNESS_TFT(1000);		// яркость на полную
	return 1;
}

uint8_t outputInit(void)
{
	RELE_OBSH_ON();			// вкл питание всех релюх
	RELE_ZAJIG_OFF();		// реле зажигания выкл
	RELE_STARTER_OFF();	// реле стартер выкл
	RELE_PODSOS_OFF();	// реле подсоса выкл
	RELE_SOST_0_OFF();	// реле 0 положения сети выкл
	RELE_SOST_1_OFF();	// реле 1 положения сети выкл
	RELE_SOST_2_OFF();	// реле 2 положения сети выкл
	LED_ON();						// светодиод работы вкл
	LED_ERROR_OFF();		// светодиод аварии выкл
	CHARGE_OFF();				// зарядка акб выкл

	return 1;
}
