


#include "warn_err.h"
#include "popUpWindow.h"
#include "fatfs.h"


void checkWarn(void)
{
	//WARN_NECESSITY_TECH_INSP	// необходимо провести тех. осмотр - проверяется раз в сутки по срабатыванию будильника

	// Включен ручной режим работы
	if((pAVR->avr_states.powerAutoManual == AVR_MANUAL) && 
			(!pAVR->warn.array_flags[WARN_MANUAL_CONTROL_EN]))
	{
			setWarn(WARN_MANUAL_CONTROL_EN);
	}

	// необходимо зарядить акб и не включена зарядка - выставить предупреждение
	if((pAVR->v_t.v_bat <= U_AKB_MIN_1) && 
			(!pAVR->avr_states.flagCharge) &&
			(!pAVR->warn.array_flags[WARN_CHARGE_AKB]))
	{
			setWarn(WARN_CHARGE_AKB);
	}
	// удалить предупреждение
	if((pAVR->v_t.v_bat > U_AKB_MIN_1) && 
			(pAVR->warn.array_flags[WARN_CHARGE_AKB]))
	{
			delWarn(WARN_CHARGE_AKB);
	}
	
	// необходим перерыв двигателя
	// удалить предупреждение
	if((pAVR->warn.array_flags[WARN_BREAK_ENGINE]) &&
		((realToUnix() - pAVR->engine.engineTimeoutTime) >= TIME_BREAK_ENGINE))
	{
		delWarn(WARN_BREAK_ENGINE);
	}
	
	// если включена непрерывная работа двигателя
	if((!pAVR->engine.flagTimeout) &&
			(!pAVR->warn.array_flags[WARN_NO_BREAK_ENGINE]))
	{
		setWarn(WARN_NO_BREAK_ENGINE);
	}
	// если отключена непрерывная работа двигателя
	if((pAVR->engine.flagTimeout) &&
			(pAVR->warn.array_flags[WARN_NO_BREAK_ENGINE]))
	{
		delWarn(WARN_NO_BREAK_ENGINE);
	}
}


void checkErr(void)
{
	// проверка не долго ли вращается стартер
	if((pAVR->avr_states.powerAutoManual == AVR_MANUAL) &&
			(HAL_GPIO_ReadPin(RELE_STARTER_GPIO_Port, RELE_STARTER_Pin)))
	{
		if((realToUnix() - pAVR->engine.starterRotationTime) >= MAX_STARTER_OPERATING_TIME)
		{
			releStarterOnOff(RESET);
			recLog("Реле стартера отключено автоматикой");
			notification("Уведомление", "Реле стартера отключено автоматикой", 15, MANUAL_RELE_SWITCH);	// перейти в меню переключения реле
		}
	}
	
	// если двигатель заведен - отключить зарядку
	if(pAVR->v_t.v_motor >= START_DETECT_VOLTAGE)
		charge_ON_OFF(RESET);
	
	// низкое напряжение акб
	if((pAVR->v_t.v_bat < U_AKB_MIN_0) &&
			(pAVR->engine.status != ENGINE_START) &&
			(pAVR->engine.status != ENGINE_STARTER_REST))	
		setErr(ERR_LOW_VOLTAGE_AKB);
		
	// высокое напряжение акб
	if(pAVR->v_t.v_bat > U_AKB_MAX)
		setErr(ERR_HIGHT_VOLTAGE_AKB);

	// неисправность цепи зарядки
	if((pAVR->v_t.v_bat < U_AKB_MIN_0) &&
			(pAVR->avr_states.flagCharge) && 
			((realToUnix() - pAVR->avr_states.timeChargeStart) > 120) &&								// после запуска зарядки прошло больше 2 минут
			(pAVR->engine.status != ENGINE_START) &&
			(pAVR->engine.status != ENGINE_STARTER_REST))
			setErr(ERR_CHARG_CIRCUIT);
}



void setWarn(WARN_WARIANTS warn)
{
	switch(warn){
		// включен ручной режим работы
		case WARN_MANUAL_CONTROL_EN:
			if(!pAVR->warn.array_flags[WARN_MANUAL_CONTROL_EN]) {
				pAVR->avr_states.powerAutoManual = AVR_MANUAL;
				pAVR->warn.array_flags[WARN_MANUAL_CONTROL_EN] = SET;
				pAVR->warn.counter++;
				notification("Предупреждение", "Включен ручной режим работы", 15, pAVR->avr_states.menu_state);
				recLog("Пользователь - переход в ручной режим работы"); 
			}
			break;
		// необходимо провести тех. осмотр
		case WARN_NECESSITY_TECH_INSP:
			if(!pAVR->warn.array_flags[WARN_NECESSITY_TECH_INSP]) {
				pAVR->warn.array_flags[WARN_NECESSITY_TECH_INSP] = SET;
				pAVR->warn.counter++;
				notification("Предупреждение", "Необходимо провести тех. осмотр", 15, pAVR->avr_states.menu_state);
				recLog("Предупреждение! Необходимо провести тех. осмотр"); 
			}	
			break;
		// необходимо зарядить акб
		case WARN_CHARGE_AKB:
			if(!pAVR->warn.array_flags[WARN_CHARGE_AKB]) {
				pAVR->warn.array_flags[WARN_CHARGE_AKB] = SET;
				pAVR->warn.counter++;
				notification("Предупреждение", "Необходимо зарядить акб", 15, pAVR->avr_states.menu_state);
				recLog("Предупреждение! Необходимо зарядить акб"); 
			}	
			break;
		// необходим перерыв двигателя
		case	WARN_BREAK_ENGINE:
			if(!pAVR->warn.array_flags[WARN_BREAK_ENGINE]) {
				pAVR->warn.array_flags[WARN_BREAK_ENGINE] = SET;
				pAVR->warn.counter++;
				notification("Предупреждение", "Необходим отдых ДВС", 15, pAVR->avr_states.menu_state);
				recLog("Предупреждение! Необходим отдых ДВС"); 
			}	
			break;
		// непрерывная работа двигателя
		case	WARN_NO_BREAK_ENGINE:
			if(!pAVR->warn.array_flags[WARN_NO_BREAK_ENGINE]) {
				pAVR->warn.array_flags[WARN_NO_BREAK_ENGINE] = SET;
				pAVR->warn.counter++;
				notification("Предупреждение", "Включена непрерывная работа двигателя", 15, pAVR->avr_states.menu_state);
				recLog("Предупреждение! Включена непрерывная работа двигателя"); 
			}	
			break;
		default:
			notification("Предупреждение", "Ошибка вывода предупреждения", 15, pAVR->avr_states.menu_state);
			recLog("Предупреждение! Ошибка вывода предупреждения"); 
			break;
	}
}

void setErr(ERR_WARIANTS err)
{
	switch(err){
		// превышено максимальное кол-во попыток запуска
		case ERR_MAX_LAUNCH_ATTEMP:
			if(!pAVR->err.array_flags[ERR_MAX_LAUNCH_ATTEMP]) {
				pAVR->err.array_flags[ERR_MAX_LAUNCH_ATTEMP] = SET;
				pAVR->err.counter++;
				notification("Ошибка!!!", "Превышено макс. кол-во попыток запуска", 30, pAVR->avr_states.menu_state);
				recLog("Ошибка!!! Превышено макс. кол-во попыток запуска");
			}	
			break;
		// ошибка отключения реле стартера
		case ERR_STARTER_RELE_SHUTDOWN:
			if(!pAVR->err.array_flags[ERR_STARTER_RELE_SHUTDOWN]) {
				RELE_OBSH_OFF();
				pAVR->err.array_flags[ERR_STARTER_RELE_SHUTDOWN] = SET;
				pAVR->err.counter++;
				notification("Ошибка!!!", "Отключения реле стартера", 30, pAVR->avr_states.menu_state);
				recLog("Ошибка!!! Отключения реле стартера");
			}	
			break;
		// ошибка включения реле стартера
		case ERR_STARTER_RELE_ACTIVATION:
			if(!pAVR->err.array_flags[ERR_STARTER_RELE_ACTIVATION]) {
				pAVR->err.array_flags[ERR_STARTER_RELE_ACTIVATION] = SET;
				pAVR->err.counter++;
				notification("Ошибка!!!", "Включения реле стартера", 30, pAVR->avr_states.menu_state);
				recLog("Ошибка!!! Включения реле стартера");
			}		
			break;
		// низкое напряжение акб
		case ERR_LOW_VOLTAGE_AKB:
			if(!pAVR->err.array_flags[ERR_LOW_VOLTAGE_AKB]) {
				pAVR->err.array_flags[ERR_LOW_VOLTAGE_AKB] = SET;
				pAVR->err.counter++;
				notification("Ошибка!!!", "Низкое напряжение акб", 30, pAVR->avr_states.menu_state);
				recLog("Ошибка!!! Низкое напряжение акб");
			}	
			break;
		// высокое напряжение акб
		case ERR_HIGHT_VOLTAGE_AKB:
			if(!pAVR->err.array_flags[ERR_HIGHT_VOLTAGE_AKB]) {
				pAVR->err.array_flags[ERR_HIGHT_VOLTAGE_AKB] = SET;
				pAVR->err.counter++;
				notification("Ошибка!!!", "Высокое напряжение акб", 30, pAVR->avr_states.menu_state);
				recLog("Ошибка!!! Высокое напряжение акб");
			}	
			break;
		// неисправность цепи зарядки
		case ERR_CHARG_CIRCUIT:
			if(!pAVR->err.array_flags[ERR_CHARG_CIRCUIT]) {
				pAVR->err.array_flags[ERR_CHARG_CIRCUIT] = SET;
				pAVR->err.counter++;
				charge_ON_OFF(RESET);
				notification("Ошибка!!!", "Неисправность цепи зарядки", 30, pAVR->avr_states.menu_state);
				recLog("Ошибка!!! Неисправность цепи зарядки");
			}		
			break;
		// был хард ресет
		case ERR_HARD_RESET:
			if(!pAVR->err.array_flags[ERR_HARD_RESET]) {
				pAVR->err.array_flags[ERR_HARD_RESET] = SET;
				pAVR->err.counter++;
				notification("Ошибка!!!", "Был хард ресет", 30, pAVR->avr_states.menu_state);
				recLog("Ошибка!!! Был хард ресет");
			}		
			break;
		// был сброс по вачдогу
		case ERR_WATCH_DOG:
			if(!pAVR->err.array_flags[ERR_WATCH_DOG]) {
				pAVR->err.array_flags[ERR_WATCH_DOG] = SET;
				pAVR->err.counter++;
				notification("Ошибка!!!", "Был сброс по вачдогу", 30, pAVR->avr_states.menu_state);
				recLog("Ошибка!!! Был сброс по вачдогу");
			}		
			break;
		// двигатель неуправляемо остановлен (заглох)
		case ERR_ENGINE_STALLED:
			if(!pAVR->err.array_flags[ERR_ENGINE_STALLED]) {
				pAVR->err.array_flags[ERR_ENGINE_STALLED] = SET;
				pAVR->err.counter++;
				notification("Ошибка!!!", "ДВС неуправляемо остановлен (заглох)", 30, pAVR->avr_states.menu_state);
				recLog("Ошибка!!! ДВС неуправляемо остановлен (заглох)");
			}	
			break;
		// ошибка sd карты
		case ERR_SD_CARD:
			if(!pAVR->err.array_flags[ERR_SD_CARD]) {
				pAVR->err.array_flags[ERR_SD_CARD] = SET;
				pAVR->err.counter++;
				
				// дэинициализация карты, если ошибка
				// slave deselect
				HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET);
				// spi stop
				__HAL_RCC_SPI2_CLK_DISABLE();

				/**SPI2 GPIO Configuration
				PB13     ------> SPI2_SCK
				PB14     ------> SPI2_MISO
				PB15     ------> SPI2_MOSI
				*/
				HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
				MX_FATFS_DeInit(); // Отвязываем драйвер
//				notification("Ошибка!!!", "sd карты", 30, pAVR->avr_states.menu_state);
//				recLog("Ошибка!!! sd карты");
			}		
			break;
		// закончилось место на sd карте
		case ERR_SD_FREE_SPACE_NULL:
			if(!pAVR->err.array_flags[ERR_SD_FREE_SPACE_NULL]) {
				pAVR->err.array_flags[ERR_SD_FREE_SPACE_NULL] = SET;
				pAVR->err.counter++;
//				notification("Ошибка!!!", "sd карта заполненна", 30, pAVR->avr_states.menu_state);
				recLog("Ошибка!!! sd карта заполненна");
			}	
			break;
		default:
			notification("Ошибка!", "Ошибка вывода ошибки", 30, pAVR->avr_states.menu_state);
			recLog("Ошибка!!! Ошибка вывода ошибки");
			break;
	}
}


void delWarn(WARN_WARIANTS warn)
{
	switch(warn){
		// включен ручной режим работы
		case WARN_MANUAL_CONTROL_EN:
			if(pAVR->warn.array_flags[WARN_MANUAL_CONTROL_EN]) {
				pAVR->warn.array_flags[WARN_MANUAL_CONTROL_EN] = RESET;
				pAVR->warn.counter--;
			}	
			break;
		// необходимо провести тех. осмотр
		case WARN_NECESSITY_TECH_INSP:
			if(pAVR->warn.array_flags[WARN_NECESSITY_TECH_INSP]) {
				pAVR->warn.array_flags[WARN_NECESSITY_TECH_INSP] = RESET;
				pAVR->warn.counter--;
			}	
			break;
		// необходимо зарядить акб 
		case WARN_CHARGE_AKB:
			if(pAVR->warn.array_flags[WARN_CHARGE_AKB]) {
				pAVR->warn.array_flags[WARN_CHARGE_AKB] = RESET;
				pAVR->warn.counter--;
				recLog("Удалено предупреждение о разряде АКБ");
			}	
			break;
		// необходим перерыв двигателя 
		case WARN_BREAK_ENGINE:
			if(pAVR->warn.array_flags[WARN_BREAK_ENGINE]) {
				pAVR->warn.array_flags[WARN_BREAK_ENGINE] = RESET;
				pAVR->warn.counter--;
				recLog("Удалено предупреждение перерыве двигателя");
			}	
			break;
		// непрерывная работа двигателя
		case	WARN_NO_BREAK_ENGINE:
			if(pAVR->warn.array_flags[WARN_NO_BREAK_ENGINE]) {
				pAVR->warn.array_flags[WARN_NO_BREAK_ENGINE] = RESET;
				pAVR->warn.counter--;
				recLog("Удалено предупреждение о непрерывной работе двигателя");
			}	
			break;
		default:
			notification("Предупреждение", "Ошибка отмены предупреждения", 15, pAVR->avr_states.menu_state);
			recLog("Предупреждение. Ошибка отмены предупреждения");
			break;
	}
}



void resetErrors (void)
{
	// переинициализация карты, если ошибка
	if(pAVR->err.array_flags[ERR_SD_CARD] == SET)
	{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		__HAL_RCC_SPI2_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI2 GPIO Configuration
    PB13     ------> SPI2_SCK
    PB14     ------> SPI2_MISO
    PB15     ------> SPI2_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		MX_FATFS_Init();
	}
	
	RELE_OBSH_ON();
	
	for(uint8_t i = 0; i < MAX_ERR_AND_WARN; i++)
		pAVR->err.array_flags[i] = 0;

	pAVR->err.counter = 0;
}


void resetWarning (void)
{
	for(uint8_t i = 0; i < MAX_ERR_AND_WARN; i++)
		pAVR->warn.array_flags[i] = 0;

	pAVR->warn.counter = 0;
}
