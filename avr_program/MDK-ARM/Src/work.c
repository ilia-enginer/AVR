
#include <stdint.h>
#include <stm32f4xx_ll_adc.h>

#include "work.h"

#include "warn_err.h"
#include "popUpWindow.h"
#include "ILI9341_GFX.h"



void work (void)
{
	HAL_IWDG_Refresh(&hiwdg);	// отмена ресета
	
	dataCalcADC();						// пересчет значений ацп
	checkWarn();							// поиск предупреждений
	checkErr();								// поиск ошибок
	
	if(BRIGHTNESS_GET_TFT != NULL_BRIGHTNESS){
		if(!pAVR->avr_states.oledWork){
			ili9341_SleepOff();
		}
		
		menuSwich();							// экранное меню
	}
	
	engineWork();							// управление двигателем
	powerAutomationControl();	// управление силовым автоматом
	chargeAkb();							// зарядка акб
	ledChange();							// моргать светодиодом
	
	// если необходимо сохранить всю инфу на флеш
	if(pAVR->avr_states.flagSaveInfoSD == SET)
	{
		HAL_IWDG_Refresh(&hiwdg);
		checkInfoTO();
		notification("SAVE SD", "Сохранение данных", 15, pAVR->avr_states.menu_state);
		// записать на sd
		setFillStructureStoryParameters();
		pAVR->avr_states.flagSaveInfoSD = RESET;
	}
}


void dataCalcADC(void)
{
	float voltage;

	uint16_t Vopora = *(volatile uint16_t *)VREFINT_CAL_ADDR; 
	
	// внешнее напряжение
	voltage = (pAVR->adc.ravADC[0] / FULL_RANGE_f * OPORA_ADC) / K_EXT_V;
	pAVR->v_t.v_out = exponentialRunningAverage(pAVR->v_t.v_out, voltage, KOFF_FILTR);

	// напряжение акума
	voltage = (pAVR->adc.ravADC[1] / FULL_RANGE_f * OPORA_ADC) / K_AKB;
	pAVR->v_t.v_bat = exponentialRunningAverage(pAVR->v_t.v_bat, voltage, KOFF_FILTR);

	// напряжение c обмотки возбуждения
	voltage = ((pAVR->adc.ravADC[2] / FULL_RANGE_f * OPORA_ADC) / K_ENGINE) - V_FALL_DIODE;
	if(voltage < 0.0f) voltage = 0.0f;
	pAVR->v_t.v_motor = exponentialRunningAverage(pAVR->v_t.v_motor, voltage, KOFF_FILTR);

	// напряжение питания реле стартера
	voltage = (pAVR->adc.ravADC[3] / FULL_RANGE_f * OPORA_ADC) / K_REL_STARTER;
	pAVR->v_t.v_rele_starter = exponentialRunningAverage(pAVR->v_t.v_rele_starter, voltage, KOFF_FILTR);

	// напряжение питания проца
	voltage = __LL_ADC_CALC_VREFANALOG_VOLTAGE(pAVR->adc.ravADC[5], LL_ADC_RESOLUTION_12B);
	pAVR->v_t.v_cpu = exponentialRunningAverage(pAVR->v_t.v_cpu, voltage, KOFF_FILTR);

	//напряжение опоры
	voltage = pAVR->v_t.v_cpu / FULL_RANGE_f * pAVR->adc.ravADC[5];
	pAVR->v_t.v_opora = exponentialRunningAverage(pAVR->v_t.v_opora, voltage, KOFF_FILTR);
	
	//температура проца	
//	uint16_t adc_cal1 = *(volatile uint16_t *)TEMPSENSOR_CAL1_ADDR; // Калибровочное значение t1
//  uint16_t adc_cal2 = *(volatile uint16_t *)TEMPSENSOR_CAL2_ADDR; // Калибровочное значение t2
//	voltage = TEMPSENSOR_CAL1_TEMP + (TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP) * (pAVR->adc.ravADC[4] - adc_cal1) / (adc_cal2 - adc_cal1);
	voltage = __LL_ADC_CALC_TEMPERATURE(pAVR->v_t.v_cpu, pAVR->adc.ravADC[4], LL_ADC_RESOLUTION_12B);
	pAVR->v_t.t_cpu = exponentialRunningAverage(pAVR->v_t.t_cpu, voltage, KOFF_FILTR);
	
	// запуск ацп
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&AVR.adc, ADC_CHANELS);	
	
	HAL_Delay(2);
}


void charge_ON_OFF(uint8_t status)
{
	if(status == RESET)
	{
		if(HAL_GPIO_ReadPin(CHARGE_ON_OFF_GPIO_Port, CHARGE_ON_OFF_Pin))
		{
			CHARGE_OFF();
			pAVR->avr_states.flagCharge = RESET;
			recLog("Зарядка отключена");
		}		
	}
	else if(status == SET)
	{
		if(!HAL_GPIO_ReadPin(CHARGE_ON_OFF_GPIO_Port, CHARGE_ON_OFF_Pin))
		{
			CHARGE_ON();
			pAVR->avr_states.flagCharge = SET;
			pAVR->avr_states.timeChargeStart = realToUnix();
			delWarn(WARN_CHARGE_AKB);
			recLog("Зарядка включена");
		}		
	}
}

time_t ledChangeTime = 0;
void ledChange(void)
{
	// моргать раз в сек
	if((realToUnix() - ledChangeTime) >= 1)
	{
		// красным - если ошибка
		if(pAVR->err.counter){
			LED_OFF();
			LED_ERROR_TOGGLE();
		}
		// в остальных случаях зеленым
		else {
			LED_ERROR_OFF();
			LED_TOGGLE();
		}
		ledChangeTime = realToUnix();
	}
}

void chargeAkb(void)
{
	// заряжать только если разряжен и двигатель остановлен
	// и не более Х часов
	if((pAVR->v_t.v_bat < U_AKB_MIN_1) &&
		((pAVR->engine.status == ENGINE_STOPPED) || (pAVR->engine.status == ENGINE_TIMEOUT)) &&
		(pAVR->avr_states.extPowerSupply == EXT_POWER_ON) &&
		(!pAVR->avr_states.flagCharge))
	{
		charge_ON_OFF(SET);
		// засечь время
		pAVR->avr_states.timeChargeStart = realToUnix();
	}
	
	// если прошло Х часов - отключить зарядку
	if((pAVR->avr_states.flagCharge) &&
		((realToUnix() - pAVR->avr_states.timeChargeStart) > TIME_CHARGE))
	{
		charge_ON_OFF(RESET);
	}
	
	// если напряжение высокое - отключить зарядку
	if((pAVR->avr_states.flagCharge) && 
			(pAVR->v_t.v_bat > U_AKB_MAX))
	{
		charge_ON_OFF(RESET);
	}
}


