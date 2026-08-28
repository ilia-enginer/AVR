
#include "init.h"


uint8_t initDevice(void)
{
	pAVR->touch.flag_hold = 0;
	pAVR->touch.flag_press = 0;
	pAVR->touch.flag_release = 0;
	pAVR->touch.x = 0;
	pAVR->touch.y = 0;
	
	initTFT();		// дисплей
		
	outputInit();	// выхода (светодиоды, реле и.т.д.)
	
	pAVR->avr_states.power_grid_mode = POWER_IS_OFF;	// флаг о питании дома
	pAVR->avr_states.flagCharge = RESET;							// флаг что заряжка откл
	pAVR->err.counter = 0;
	pAVR->warn.counter = 0;
	
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&AVR.adc, ADC_CHANELS);	// запуск ацп
	
	// настройка даты, времени(если необходимо)
	HAL_RTC_GetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN);
	if((DateToUpdate.Date == 1) && (DateToUpdate.Month == 1) && (DateToUpdate.Year == 0))
	{
		DateToUpdate.WeekDay = RTC_WEEKDAY_TUESDAY;
		DateToUpdate.Month = RTC_MONTH_SEPTEMBER;
		DateToUpdate.Date = 1;
		DateToUpdate.Year = 26;
		
		sTime.Hours = 12;
		sTime.Minutes = 0;
		sTime.Seconds = 0;
		sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		sTime.StoreOperation = RTC_STOREOPERATION_RESET;
		
		if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)	Error_Handler();
		if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)	Error_Handler();
	}
	
	
	menuChangeState(MAIN_MENU);
	
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
	
	//???
//	uint16_t size_img = sizeof(img_logo); // размер картинки в байтах (картинка лежит в файле img.h)
//  ILI9341_Draw_Image(img_logo, (ILI9341_SCREEN_HEIGHT / 3), (ILI9341_SCREEN_WIDTH / 3), (ILI9341_SCREEN_WIDTH / 3), (ILI9341_SCREEN_HEIGHT / 3), size_img); // вывести в центре
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