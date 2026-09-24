
#include <stdint.h>
#include <stm32f4xx_ll_adc.h>
#include "work.h"
#include "warn_err.h"
#include "menu_main.h"
#include "popUpWindow.h"


void work (void)
{
	dataCalcADC();		// пересчет значений ацп
	checkWarn();			// поиск предупреждений
	checkErr();				// поиск ошибок
	menuSwich();			// экранное меню
	
	
	
	// если необходимо сохранить всю инфу на флеш
	if(pAVR->avr_states.flagSaveInfoSD == SET)
	{
		checkInfoTO();
		notification("SAVE SD", "Сохранение данных", 5, pAVR->avr_states.menu_state);
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
	pAVR->v_t.v_motor = exponentialRunningAverage(pAVR->v_t.v_motor, voltage, KOFF_FILTR);

	// напряжение питания реле стартера
	voltage = (pAVR->adc.ravADC[3] / FULL_RANGE_f * OPORA_ADC) / K_REL_STARTER;
	pAVR->v_t.v_rele_starter = exponentialRunningAverage(pAVR->v_t.v_rele_starter, voltage, KOFF_FILTR);

	// напряжение питания проца
	voltage = OPORA_ADC * Vopora / pAVR->adc.ravADC[5];
	pAVR->v_t.v_cpu = exponentialRunningAverage(pAVR->v_t.v_cpu, voltage, KOFF_FILTR);

	//напряжение опоры
	voltage = pAVR->v_t.v_cpu / FULL_RANGE_f * pAVR->adc.ravADC[5];
	pAVR->v_t.v_opora = exponentialRunningAverage(pAVR->v_t.v_opora, voltage, KOFF_FILTR);
	
	//температура проца
	uint16_t adc_cal1 = *(volatile uint16_t *)TEMPSENSOR_CAL1_ADDR; // Калибровочное значение t1
  uint16_t adc_cal2 = *(volatile uint16_t *)TEMPSENSOR_CAL2_ADDR; // Калибровочное значение t2
	voltage = TEMPSENSOR_CAL1_TEMP + (TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP) * (pAVR->adc.ravADC[4] - adc_cal1) / (adc_cal2 - adc_cal1);
	pAVR->v_t.t_cpu = exponentialRunningAverage(pAVR->v_t.t_cpu, voltage, KOFF_FILTR);
	
	// запуск ацп
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&AVR.adc, ADC_CHANELS);	
}


float exponentialRunningAverage(float value, float valueNew, float koff)
{
	return valueNew * koff + value * (1.0 - koff);
}


