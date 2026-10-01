
#include <stdint.h>
#include <stm32f4xx.h>

#include "engine.h"
#include "main.h"


/*
------------ функция запуска/остановки двигателя ---------
если надо запустить
	если статус двигателя	ОСТАНОВЛЕН 
		статус - ЗАПУСК
		выйти
	если статус двигателя	ОХЛАЖДЕНИЕ
		если время работы больше 4 часа и поднят флаг "работа с перерывами"
			- выйти
			
		- сброс переменной "время охлаждения"
		- статус - ЗАПУЩЕН
		- выйти

если надо заглушить
	если статус двигателя	ЗАПУСК
		- выкл. стартер
		- выкл. подсос
		- сбросить счетчик кручения стартером
		- сбросить счетчик попыток последнего запуска
		- записать время в переменную "начала остановки"
		- статус - ОСТАНОВКА
		- выйти
		
	если статус двигателя	ПРОГРЕВ
		- сбросить переменную "время прогрева"
		- записать время в переменную "начала остановки"
		- статус - ОСТАНОВКА
		- выйти
		
	если статус двигателя	ЗАПУЩЕН
		- статус - ОХЛАЖДЕНИЕ
		- выйти
*/		
uint8_t startStopEngine(uint32_t status)
{
	// если надо запустить
	if(status == SET)
	{
		// если статус двигателя	ОСТАНОВЛЕН 
		if(pAVR->engine.status == ENGINE_STOPPED){
			// статус - ЗАПУСК
			pAVR->engine.status = ENGINE_START;
			recLog("Новый статус двигателя - запуск");
			return 1;
		}
		// если статус двигателя	ОХЛАЖДЕНИЕ
		else if(pAVR->engine.status == ENGINE_COOLING){
			// если время работы больше времени непрерывной работы и поднят флаг "работа с перерывами"
			if((((uint32_t)realToUnix() - pAVR->engine.workingTime) > ENGINE_CONTINUOUS_TIME) &&
					(pAVR->engine.flagTimeout == SET))
					return 0;
					
			// сброс переменной "время охлаждения"
			pAVR->engine.collingTime = 0;
			// статус - ЗАПУЩЕН
			pAVR->engine.status = ENGINE_WORK;
			recLog("Новый статус двигателя - работа");
			return 1;
		}
	}
	
	// если надо заглушить
	else if(status == RESET)
	{
		// если статус двигателя	ЗАПУСК или отдых стартера
		if((pAVR->engine.status == ENGINE_START) ||
			(pAVR->engine.status == ENGINE_STARTER_REST)){
			RELE_STARTER_OFF();											// выкл. стартер
			RELE_PODSOS_OFF();											// выкл. подсос
			pAVR->engine.starterRotationTime = 0;		// сбросить счетчик кручения стартером
			pAVR->engine.launchAttempts = 0;				// сбросить счетчик попыток последнего запуска
			pAVR->engine.collingTime = (uint32_t)realToUnix();	// записать время в переменную "начала остановки"
			pAVR->engine.status = ENGINE_STOP;			// статус - ОСТАНОВКА
			recLog("Новый статус двигателя - остановка");
			return 1;
		}
		// если статус двигателя	ПРОГРЕВ
		else if(pAVR->engine.status == ENGINE_WARM_UP){
			pAVR->engine.warmUpTime = 0;												// сбросить переменную "время прогрева"
			pAVR->engine.collingTime = (uint32_t)realToUnix();	// записать время в переменную "начала остановки"
			pAVR->engine.status = ENGINE_STOP;									// статус - ОСТАНОВКА
			recLog("Новый статус двигателя - остановка");
			return 1;
		}
		// если статус двигателя	ЗАПУЩЕН
		else if(pAVR->engine.status == ENGINE_WORK){
			pAVR->engine.status = ENGINE_COOLING;		// статус - ОХЛАЖДЕНИЕ
			recLog("Новый статус двигателя - охлаждение");
			return 1;
		}
	}

	return 0;
}

	
/*	
------------- АЛГОРИТМ РАБОТЫ ДВИГАТЕЛЯ ------------

если статус двигателя	ОСТАНОВЛЕН 
	- выйти	
если статус двигателя	ЗАПУСК
	- если двигатель в ошибке
		- выйти
	- если не крутит стартером 
		- если выключено зажигание 
			- включить(сделать отдельную функцию) 
			- пауза 200мс
			- проверка включения реле
				если не включилось 
					- ошибка
					- выключить
					- статус - ОСТАНОВЛЕН
					- выйти
		- если необходимо вкл подсос и не включен
			- включить, пауза 3мс
		- засечь время начала работы стартера
		- включить стартер
		- выйти
	- если крутит стартером 
		- если завелся
			- выкл. стартер
			- выкл. подсос
			- сбросить счетчик кручения стартером
			- сбросить счетчик попыток последнего запуска
			- инкрементировать удачную попытку запуска
			- инкрементировать попытку запуска всего
			- засечь время прогрева
			- засечь время работы
			- статус - ПРОГРЕВ
			- выйти
		- если крутит больше чем надо
			- выкл. стартер
			- выкл. подсос
			- выкл. зажигание
			- сбросить счетчик кручения стартером
			- засечь время начала отдыха стартера
			- инкрементировать попытку последнего запуска
			- инкрементировать попытку запуска всего
			- если ичерпано кол-во попыток запуска
				- статус двигателя - остановлен
				- обнулить переменную попыток последнего запуска
				- двигатель в ошибку
				- статус - ОСТАНОВЛЕН
				- выйти
			- статус - ОТДЫХ СТАРТЕРА
			- выйти
			
если статус двигателя	ОТДЫХ СТАРТЕРА	
	если прошло 3с
		- сбросить переменную перерыва
		- статус ЗАПУСК
		- выйти

если статус двигателя	ПРОГРЕВ
	если двигатель заглох!!!!!!!!!!!!!!!!
		- сбросить переменную "время прогрева"
			- двигатель в ошибку
			- приплюсовать моточасы после ТО
			- минусовать моточасы до ТО
			- приплюсовать моточасы всего
			- обнулить переменную времени работы
			- статус - ОСТАНОВЛЕН
			- выйти
		
	если прогревается 3 мин
		- сбросить переменную "время прогрева"
		- статус - ЗАПУЩЕН
		- выйти

если статус двигателя	ЗАПУЩЕН
	если двигатель заглох!!!!!!!!!!!!!!!!
			- двигатель в ошибку
			- приплюсовать моточасы после ТО
			- минусовать моточасы до ТО
			- приплюсовать моточасы всего
			- обнулить переменную времени работы
			- статус - ОСТАНОВЛЕН
			- выйти
			
	если работает больше 4 часов
		если можно делать перерывы
			- сброс переменной "время охлаждения"
			- статус - ОХЛАЖДЕНИЕ
			- если нет внешнего питания - силовой рубильник в 0
			- выйти

		если нельзя делать перерывы
			- ворнинг - необходим отдых ДВС
			- выйти

если статус двигателя	ОХЛАЖДЕНИЕ
	если двигатель заглох!!!!!!!!!!!!!!!!
		- сбросить переменную "время прогрева"
			- двигатель в ошибку
			- приплюсовать моточасы после ТО
			- минусовать моточасы до ТО
			- приплюсовать моточасы всего
			- обнулить переменную времени работы
			- статус - ОСТАНОВЛЕН
			- выйти
			
	если охлаждается больше 3мин
		- сброс переменной "время охлаждения"
		- выкл. зажигание
		- статус ОСТАНОВКА
		- записать время в переменную "начала остановки"
		- выйти

если статус двигателя	ОСТАНОВКА	
	если останавливается больше 10 секунд
		- выкл. зажигание
		- статус - ОСТАНОВЛЕН
		- двигатель в ошибку
		- сбросить переменную "начала остановки"
		- приплюсовать моточасы после ТО
		- минусовать моточасы до ТО
		- приплюсовать моточасы всего
		- обнулить переменную времени работы
		- выйти
	если остановился
		- сбросить переменную "начала остановки"
		- статус - ОСТАНОВЛЕН
		- приплюсовать моточасы после ТО
		- минусовать моточасы до ТО
		- приплюсовать моточасы всего
		- обнулить переменную времени работы
			если время работы больше 4 часа и поднят флаг "работа с перерывами"
				- засечь переменную "начала перерыва"
				- статус - ПЕРЕРЫВ
		- выйти
		
если статус двигателя	ПЕРЕРЫВ		
		если перерыв больше 30 мин
			- сброс переменной "начала перерыва"
			- статус - ОСТАНОВЛЕН
			- выйти
*/			
void engineWork(void)
{
	// если статус двигателя	ОСТАНОВЛЕН
	if(pAVR->engine.status == ENGINE_STOPPED)
		return;
			
	// если статус двигателя	ЗАПУСК	
	else if(pAVR->engine.status == ENGINE_START)
	{
		// если двигатель в ошибке
		if(checkErrEngine()){		
			RELE_STARTER_OFF();
			RELE_PODSOS_OFF();
			releZajigOnOff(RESET);
			// статус двигателя - ОСТАНОВЛЕН
			pAVR->engine.status = ENGINE_STOPPED;
			recLog("Новый статус двигателя - ОСТАНОВЛЕН");
			return;
		}
		// если не крутит стартером 
		else if(!HAL_GPIO_ReadPin(RELE_STARTER_GPIO_Port, RELE_STARTER_Pin))
		{
			// включить зажигание
			if(!releZajigOnOff(SET))
			{
				// статус двигателя - ОСТАНОВЛЕН
				pAVR->engine.status = ENGINE_STOPPED;
				recLog("Новый статус двигателя - ОСТАНОВЛЕН");
				return;
			}
			// если необходимо вкл подсос и не включен
			if((pAVR->v_t.t_cpu < WINTER_MODE_TEMPERATURE) &&
					(!HAL_GPIO_ReadPin(RELE_PODSOS_GPIO_Port, RELE_PODSOS_Pin)))
			{
				RELE_PODSOS_ON();
				recLog("Подсос включен");
				HAL_Delay(10);
			}
			// если включена зарядка - выключить
			if(pAVR->avr_states.flagCharge){
				charge_ON_OFF(RESET);
				HAL_Delay(5);
			}
			//засечь время начала работы стартераpAVR
			pAVR->engine.starterRotationTime = realToUnix();
			// включить стартер
			RELE_STARTER_ON();
			recLog("Стартер включен");
			return;
		}
		// если крутит больше чем надо
		else if(HAL_GPIO_ReadPin(RELE_STARTER_GPIO_Port, RELE_STARTER_Pin) &&
						((realToUnix() - pAVR->engine.starterRotationTime) >= MAX_STARTER_OPERATING_TIME))
		{
			RELE_STARTER_OFF();
			RELE_PODSOS_OFF();
			releZajigOnOff(RESET);
			pAVR->engine.starterRotationTime = 0;		// сбросить счетчик кручения стартером
			pAVR->engine.starterTimeoutTime = realToUnix();	// засечь время начала отдыха стартера
			pAVR->engine.launchAttempts++;				// инкрементировать попытку последнего запуска
			pAVR->sdParams.numLaunchAttempt++;		// инкрементировать попытку запуска всего
			// если ичерпано кол-во попыток запуска
			if(pAVR->sdParams.numLaunchAttempt >= MAX_LAUNCH_ATTEMPTS)
			{
				pAVR->engine.starterRotationTime = 0;	// сбросить счетчик кручения стартером
				// статус двигателя - ОСТАНОВЛЕН
				pAVR->engine.status = ENGINE_STOPPED;
				recLog("Новый статус двигателя - ОСТАНОВЛЕН");
				setErr(ERR_MAX_LAUNCH_ATTEMP);	// двигатель в ошибку
				return;
			}
			// статус - ОТДЫХ СТАРТЕРА
			pAVR->engine.status = ENGINE_STARTER_REST;
			recLog("Новый статус двигателя - ОТДЫХ СТАРТЕРА");
			return;
		}
		// если крутит стартером 
		else if(HAL_GPIO_ReadPin(RELE_STARTER_GPIO_Port, RELE_STARTER_Pin))
		{
			// если завелся	
			if(pAVR->v_t.v_motor >= START_DETECT_VOLTAGE)
			{
				RELE_STARTER_OFF();
				RELE_PODSOS_OFF();
				pAVR->engine.starterRotationTime = 0;	// сбросить счетчик кручения стартером
				pAVR->engine.launchAttempts = 0;			// сбросить счетчик попыток последнего запуска
				pAVR->sdParams.numSuccessLaunch++;		// инкрементировать удачную попытку запуска
				pAVR->sdParams.numLaunchAttempt++;		// инкрементировать попытку запуска всего
				pAVR->engine.warmUpTime = realToUnix();	// засечь время прогрева
				pAVR->engine.workingTime = realToUnix();// засечь время работы
				// статус - ПРОГРЕВ
				pAVR->engine.status = ENGINE_WARM_UP;
				recLog("Новый статус двигателя - ПРОГРЕВ");
				return;
			}
		}
	}
	// если статус двигателя	ОТДЫХ СТАРТЕРА	
	else if(pAVR->engine.status == ENGINE_STARTER_REST)
	{
		// если прошло 3с
		if((realToUnix() - pAVR->engine.starterTimeoutTime) >= MAX_STARTER_BREAK_TIME)
		{
			pAVR->engine.starterTimeoutTime = 0; // сбросить переменную перерыва
			// статус ЗАПУСК
			pAVR->engine.status = ENGINE_START;
			recLog("Новый статус двигателя - ЗАПУСК");
			return;
		}
	}
	// если статус двигателя	ПРОГРЕВ
	else if(pAVR->engine.status == ENGINE_WARM_UP)
	{
		// если двигатель заглох
		if(pAVR->v_t.v_motor < STOP_DETECT_VOLTAGE)
		{
			RELE_STARTER_OFF();
			RELE_PODSOS_OFF();
			releZajigOnOff(RESET);
			pAVR->engine.warmUpTime = 0;		// сбросить переменную "время прогрева"
			setErr(ERR_ENGINE_STALLED);			// двигатель в ошибку
			incrementEngineHours();
			pAVR->engine.workingTime = 0;		// обнулить переменную времени работы
			// статус - ОСТАНОВЛЕН
			pAVR->engine.status = ENGINE_STOPPED;
			recLog("Новый статус двигателя - ОСТАНОВЛЕН");
			return;
		}
		// если прогревается 3 мин
		if((realToUnix() - pAVR->engine.warmUpTime) >= WARM_UP_TIME)
		{
			pAVR->engine.warmUpTime = 0;	// сбросить переменную "время прогрева"
			// статус - ЗАПУЩЕН
			pAVR->engine.status = ENGINE_WORK;
			recLog("Новый статус двигателя - ЗАПУЩЕН");
			return;
		}
	}
	// если статус двигателя	ЗАПУЩЕН
	else if(pAVR->engine.status == ENGINE_WORK)
	{
		// если двигатель заглох
		if(pAVR->v_t.v_motor < STOP_DETECT_VOLTAGE)
		{
			RELE_STARTER_OFF();
			RELE_PODSOS_OFF();
			releZajigOnOff(RESET);
			setErr(ERR_ENGINE_STALLED);			// двигатель в ошибку
			incrementEngineHours();
			pAVR->engine.workingTime = 0;		// обнулить переменную времени работы
			// статус - ОСТАНОВЛЕН
			pAVR->engine.status = ENGINE_STOPPED;
			recLog("Новый статус двигателя - ОСТАНОВЛЕН");
			return;
		}
		// если работает больше 4 часов
		if((realToUnix() - pAVR->engine.workingTime) >= ENGINE_CONTINUOUS_TIME)
		{
			// если можно делать перерывы
			if(pAVR->engine.flagTimeout)
			{
				// засечь "время охлаждения"
				pAVR->engine.collingTime = realToUnix();
				// статус - ОХЛАЖДЕНИЕ
				pAVR->engine.status = ENGINE_COOLING;
				recLog("Новый статус двигателя - ОХЛАЖДЕНИЕ");
				
				// если нет внешнего питания - силовой рубильник в 0
				if(pAVR->avr_states.extPowerSupply != EXT_POWER_OFF)
					switchPowerCircuitBreaker(POWER_IS_OFF);
				else if((pAVR->avr_states.extPowerSupply == EXT_POWER_OFF) &&
								(pAVR->avr_states.power_grid_mode != EXTERNAL_POWER))
					switchPowerCircuitBreaker(EXTERNAL_POWER);
				return;
			
			}
			// если нельзя делать перерывы
			else
			{
				// ворнинг - необходим отдых ДВС
				setWarn(WARN_BREAK_ENGINE);
				return;
			}
		}
	}
	// если статус двигателя	ОХЛАЖДЕНИЕ
	else if(pAVR->engine.status == ENGINE_COOLING)
	{
		// если двигатель заглох
		if(pAVR->v_t.v_motor < STOP_DETECT_VOLTAGE)
		{
			pAVR->engine.collingTime = 0;// сбросить переменную "время охлаждения"
			RELE_STARTER_OFF();
			RELE_PODSOS_OFF();
			releZajigOnOff(RESET);
			setErr(ERR_ENGINE_STALLED);			// двигатель в ошибку
			incrementEngineHours();					// приплюсовать моточасы после ТО
			pAVR->engine.workingTime = 0;		// обнулить переменную времени работы
			// статус - ОСТАНОВЛЕН
			pAVR->engine.status = ENGINE_STOPPED;
			recLog("Новый статус двигателя - ОСТАНОВЛЕН");
			return;
		}
		// если охлаждается больше 3мин
		if((realToUnix() - pAVR->engine.collingTime) >= COLDING_TIME)
		{
			pAVR->engine.collingTime = 0;// сбросить переменную "время охлаждения"
			RELE_STARTER_OFF();
			RELE_PODSOS_OFF();
			releZajigOnOff(RESET);
			pAVR->engine.startTimeStop = realToUnix();	//записать время в переменную "начала остановки"
			// статус - ОСТАНОВКА
			pAVR->engine.status = ENGINE_STOP;
			recLog("Новый статус двигателя - ОСТАНОВКА");
			return;
		}
	}
	// если статус двигателя	ОСТАНОВКА
	else if(pAVR->engine.status == ENGINE_STOP)
	{
		// если останавливается больше 3 секунд
		if((realToUnix() - pAVR->engine.startTimeStop) >= 3)
		{
			pAVR->engine.startTimeStop = 0; // сбросить переменную "начала остановки"
			incrementEngineHours();					// приплюсовать моточасы после ТО
			
			// если работает больше 4 часов и можно делать перерывы
			if(((realToUnix() - pAVR->engine.workingTime) >= ENGINE_CONTINUOUS_TIME) &&
					(pAVR->engine.flagTimeout))
			{
				// засечь переменную "начала перерыва"
				pAVR->engine.engineTimeoutTime = realToUnix();
				// статус - ПЕРЕРЫВ
			pAVR->engine.status = ENGINE_TIMEOUT;
			recLog("Новый статус двигателя - ПЕРЕРЫВ");
			}
			else
			{
				// статус - ОСТАНОВЛЕН
				pAVR->engine.status = ENGINE_STOPPED;
				recLog("Новый статус двигателя - ОСТАНОВЛЕН");
			}
			pAVR->engine.workingTime = 0;		// обнулить переменную времени работы
			
//			// если не остановился - двигатель в ошибку
//			if(pAVR->v_t.v_motor > STOP_DETECT_VOLTAGE)
//				setErr(хз);
				
			return;
		}
	}
	// если статус двигателя	ПЕРЕРЫВ		
	else if(pAVR->engine.status == ENGINE_TIMEOUT)
	{
		// если перерыв больше 30 мин
		if((realToUnix() - pAVR->engine.engineTimeoutTime) >= TIME_BREAK_ENGINE)
		{
			// сброс переменной "начала перерыва"
			pAVR->engine.engineTimeoutTime = 0;
			// статус - ОСТАНОВЛЕН
			pAVR->engine.status = ENGINE_STOPPED;
			recLog("Новый статус двигателя - ОСТАНОВЛЕН");
			return;
		}
	}
}


uint8_t checkErrEngine(void)
{
	if(pAVR->err.array_flags[ERR_MAX_LAUNCH_ATTEMP] ||
					pAVR->err.array_flags[ERR_STARTER_RELE_SHUTDOWN] ||
					pAVR->err.array_flags[ERR_STARTER_RELE_ACTIVATION] ||
					pAVR->err.array_flags[ERR_ENGINE_STALLED] ||
					pAVR->err.array_flags[ERR_LOW_VOLTAGE_AKB])
					return 1;
					
					
	return 0;
}

uint8_t releZajigOnOff(uint8_t status)
{
	if(status == SET)
	{
		if(!HAL_GPIO_ReadPin(RELE_OBSH_GPIO_Port, RELE_ZAJIG_Pin))
		{
			RELE_ZAJIG_ON();
			HAL_Delay(50);
			// запуск ацп
			HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&AVR.adc, ADC_CHANELS);	
			HAL_Delay(5);
			float voltage = (pAVR->adc.ravADC[3] / FULL_RANGE_f * OPORA_ADC) / K_REL_STARTER;
			// если не включилось 
			if(voltage < 9.0f){
				RELE_ZAJIG_OFF();
				setErr(ERR_STARTER_RELE_ACTIVATION);
				return 0;
			}
			else{
				recLog("Включено зажигание");
				return 1;
			}
		}
		else
			return 1;
	}
	else
	{
		if(HAL_GPIO_ReadPin(RELE_OBSH_GPIO_Port, RELE_ZAJIG_Pin))
		{
			RELE_ZAJIG_OFF();
			HAL_Delay(150);
			// запуск ацп
			HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&AVR.adc, ADC_CHANELS);	
			HAL_Delay(5);
			float voltage = (pAVR->adc.ravADC[3] / FULL_RANGE_f * OPORA_ADC) / K_REL_STARTER;
			// если не выключилось 
			if(voltage > 7.0f){
				setErr(ERR_STARTER_RELE_SHUTDOWN);
				return 0;
			}
			else{
				pAVR->engine.engineTimeoutTime = realToUnix();
				recLog("Выключено зажигание");
				return 1;
			}
		}
		else
			return 1;
	
	}
}

// инкремент моточасов
void incrementEngineHours(void)
{
	// pAVR->engine.workingTime
	
//	uint32_t engineHoursTotal;						// моточасы всего

//	uint32_t engineHoursTO;								// моточасы после ТО
//	uint32_t engineMinutesTO;							// мотоминуты после ТО
//	uint32_t hoursBeforeTO;								// моточасы до ТО
//	uint32_t minutesBeforeTO;							// мотоминуты до ТО



			//??? // приплюсовать моточасы после ТО
			// минусовать моточасы до ТО
			// приплюсовать моточасы всего
	
	
	// прибавить к общему счетчику моточасов
//	volatile uint32_t temp = pAVR->engine.workingTime / 3600;	// вычисляю часы
//	uint32_t temp1 = 
	
	
	
//	pAVR->sdParams.engineHoursTotal += temp;
//	pAVR->sdParams.minutesALLWithoutElectric += (uint32_t)powerOutageTime - ((uint32_t)temp * 3600);	// минуты
}



