
#include <stdint.h>
#include <math.h>

#include "powerCircuitBreaker.h"
#include "main.h"


// переключает главный рубильник
void switchPowerCircuitBreaker(uint32_t status)
{
	// отключить питание дома
	if((status == POWER_IS_OFF) &&
			(pAVR->avr_states.power_grid_mode != POWER_IS_OFF))
	{
		RELE_SOST_1_OFF();
		RELE_SOST_2_OFF();
		HAL_Delay(1);
		RELE_SOST_0_ON();
		IWDG->KR = 0xAAAA;
		HAL_Delay(1000);
		IWDG->KR = 0xAAAA;
		HAL_Delay(1000);
		IWDG->KR = 0xAAAA;
		HAL_Delay(1000);
		RELE_SOST_0_OFF();
		pAVR->avr_states.power_grid_mode = POWER_IS_OFF;
		recLog("Переключение силового автомата авр, POWER_IS_OFF");
	}
	// питание от внешней сети
	else if((status == EXTERNAL_POWER) &&
					(pAVR->avr_states.power_grid_mode != EXTERNAL_POWER))
	{
		RELE_SOST_0_OFF();
		RELE_SOST_2_OFF();
		HAL_Delay(1);
		RELE_SOST_1_ON();
		pAVR->avr_states.power_grid_mode = EXTERNAL_POWER;
		recLog("Переключение силового автомата авр, EXTERNAL_POWER");
	}
	// питание от генератора
	else if((status == POWERED_BY_GENERATOR) && 
					(pAVR->avr_states.power_grid_mode != POWERED_BY_GENERATOR))
	{
		RELE_SOST_0_OFF();
		RELE_SOST_1_OFF();
		HAL_Delay(1);
		RELE_SOST_2_ON();
		pAVR->avr_states.power_grid_mode = POWERED_BY_GENERATOR;
		recLog("Переключение силового автомата авр, POWERED_BY_GENERATOR");
	}
}


/*
----------- АЛГОРИТМ УПРАВЛЕНИЯ СИЛОВЫМ АВТОМАТОМ ------------
определить какое питание и выставить флаг
если не автоматический режим - выйти
	
при отсутствии питания
	если нет напряжения, но поднят флаг что есть - значит только что отключили
		- записать время и дату отключения в структуру(+ лог), 
		- записать в юниксе для подсчета
		- поднять флаг отсутствия напряжения
		если автоматический режим
			- если двигатель еще работает(например после последнего отключения)
				- перевести двигатель в рабочий режим
	если напряжения нет в течении 30 сек и двигатель не в рабочем режиме
		- перевести двигатель в рабочий режим(отдельная функция)
	если напряжения нет в течении 30 сек и двигатель в рабочем режиме
		- переключить силовой рубильник на питание от генератора
		
при наличии питания
	если есть напряжение, но поднят флаг что нет - значит только что вкллючили
		- засечь время т.е. сбросить счетчик
		- прибавить к общему счетчику время без эл-ва
		- убрать флаг что нет эл-ва
	если есть напряжение и есть флаг что оно есть и прошла 1 мин
		- перейти на питание от сети
		- начать глушить двигатель
*/
time_t powerOutageTime = 0;	// время отключения эл-ва
time_t powersupply = 0;			// время включения эл-ва
void powerAutomationControl(void)
{
	// определить какое питание и выставить флаг
	if(pAVR->v_t.v_out <= U_V_OUT_MIN)
	{
		// если нет напряжения, но поднят флаг что есть - значит только что отключили
		if((pAVR->v_t.v_out <= U_V_OUT_MIN) && (pAVR->avr_states.extPowerSupply != EXT_POWER_OFF))
		{
			// прочитать дату, время
			HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); 				
			HAL_RTC_GetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN);
			
			// записать время в структуру
			pAVR->sdParams.hoursWithoutElectric 	= sTime.Hours;				// час последнего отключения
			pAVR->sdParams.minutesWithoutElectric = sTime.Minutes;			// минуты последнего отключения
			pAVR->sdParams.secondsWithoutElectric = sTime.Seconds;			// секунды последнего отключения
			pAVR->sdParams.dateWithoutElectric 		= DateToUpdate.Date;	// дата последнего отключения
			pAVR->sdParams.monthWithoutElectric 	= DateToUpdate.Month;	// месяц последнего отключения
			pAVR->sdParams.yearWithoutElectric 		= DateToUpdate.Year;	// год последнего отключения
			
			recLog("Отключение эл-ва");	// запись в лог
			
			powersupply = 0;
			
			// записать время отключения в юникс
			powerOutageTime = realToUnix();
			
			// поднять флаг отсутствия напряжения
			pAVR->avr_states.extPowerSupply = EXT_POWER_OFF;
			
			// если автоматический режим
			if(pAVR->avr_states.powerAutoManual == AVR_AUTO) {
				// если двигатель еще работает(например после последнего отключения)
				if(pAVR->engine.status == ENGINE_COOLING){
					startStopEngine(SET);	// перевести двигатель в рабочий режим
				}
				
				// записать время отключения
				powerOutageTime = realToUnix();
			}
		}
		// если напряжения нет в течении 30 сек и двигатель не в рабочем режиме 
		else if((pAVR->avr_states.extPowerSupply == EXT_POWER_OFF) &&
			(powerOutageTime != 0) &&
			((realToUnix() - powerOutageTime) > 30) &&
			(pAVR->engine.status != ENGINE_WORK))
		{
			// перевести двигатель в рабочий режим
			if(pAVR->avr_states.powerAutoManual == AVR_AUTO)			// если конечно автоматический режим работы
				startStopEngine(SET);
		}
		// если напряжения нет в течении 30 сек и двигатель в рабочем режиме
		else if((pAVR->avr_states.extPowerSupply == EXT_POWER_OFF) &&
			(powerOutageTime != 0) &&
			((realToUnix() - powerOutageTime) > 30) &&
			(pAVR->engine.status == ENGINE_WORK))
		{
			// переключить силовой рубильник на питание от генератора
			if(pAVR->avr_states.powerAutoManual == AVR_AUTO)			// если конечно автоматический режим работы
				switchPowerCircuitBreaker(POWERED_BY_GENERATOR);
		}
		// если так вышло что время все еще не засечено - засечь
		else if((pAVR->avr_states.extPowerSupply == EXT_POWER_OFF) &&
						(powerOutageTime == 0))
		{
			// записать время отключения
			powerOutageTime = realToUnix();
		}
	}
	// при наличии питания
	else
	{
		// если есть напряжение, но поднят флаг что нет - значит только что вкллючили
		if(pAVR->avr_states.extPowerSupply != EXT_POWER_ON)
		{
			// засечь время
			powersupply = realToUnix();
			
			// прибавить к общему счетчику время без эл-ва	
			volatile uint32_t temp = powerOutageTime / 3600;	// вычисляю часы
			pAVR->sdParams.hoursALLWithoutElectric += temp;
			pAVR->sdParams.minutesALLWithoutElectric += (uint32_t)powerOutageTime - ((uint32_t)temp * 3600);	// минуты
			
			powerOutageTime = 0;
			
			// убрать флаг что нет эл-ва
			pAVR->avr_states.extPowerSupply = EXT_POWER_ON;
		}
		// если есть напряжение и есть флаг что оно есть и прошла 1 мин
		else if((pAVR->avr_states.extPowerSupply == EXT_POWER_ON) &&
						(powersupply != 0) &&
						((realToUnix() - powersupply) > 60))
		{
			// если автоматический режим работы
			if(pAVR->avr_states.powerAutoManual == AVR_AUTO)
			{
				// перейти на питание от сети
				switchPowerCircuitBreaker(EXTERNAL_POWER);
				// начать глушить двигатель
				startStopEngine(RESET);
			}
		}
	}
}

