


#include "warn_err.h"
#include "popUpWindow.h"
#include "fatfs.h"


void checkWarn(void)
{
	//WARN_NECESSITY_TECH_INSP	// необходимо провести тех. осмотр - проверяется раз в сутки по срабатыванию будильника

	// Включен ручной режим работы
	if((pAVR->avr_states.powerAutoManual == AVR_MANUAL) && 
			(!pAVR->warn.array_flags[WARN_MANUAL_CONTROL_EN]))
			setWarn(WARN_MANUAL_CONTROL_EN);

	// необходимо зарядить акб - выставить предупреждение
	if((pAVR->v_t.v_bat <= U_AKB_MIN_1) && 
			(!pAVR->warn.array_flags[WARN_CHARGE_AKB]))
			setWarn(WARN_CHARGE_AKB);
	// удалить предупреждение
	if((pAVR->v_t.v_bat > U_AKB_MIN_1) && 
			(pAVR->warn.array_flags[WARN_CHARGE_AKB]))
			delWarn(WARN_CHARGE_AKB);
}


void checkErr(void)
{
//    ERR_MAX_LAUNCH_ATTEMP = 0,				// превышено максимальное кол-во попыток запуска
//		ERR_STARTER_RELE_SHUTDOWN,				// ошибка отключения реле стартера
//		ERR_STARTER_RELE_ACTIVATION,			// ошибка включения реле стартера
//		ERR_LOW_VOLTAGE_AKB,							// низкое напряжение акб
//		ERR_HIGHT_VOLTAGE_AKB,						// высокое напряжение акб
//		ERR_CHARG_CIRCUIT,								// неисправность цепи зарядки
//		ERR_HARD_RESET,										// был хард ресет
//		ERR_WATCH_DOG,										// был сброс по вачдогу
//		ERR_ENGINE_STALLED,								// двигатель неуправляемо остановлен (заглох)

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
				notification("Предупреждение", "Включен ручной режим работы", 10, pAVR->avr_states.menu_state);
				recLog("Пользователь - переход в ручной режим работы"); 
			}
			else menuChangeState(pAVR->avr_states.menu_state);	
			break;
		// необходимо провести тех. осмотр
		case WARN_NECESSITY_TECH_INSP:
			if(!pAVR->warn.array_flags[WARN_NECESSITY_TECH_INSP]) {
				pAVR->warn.array_flags[WARN_NECESSITY_TECH_INSP] = SET;
				pAVR->warn.counter++;
				notification("Предупреждение", "Необходимо провести тех. осмотр", 10, pAVR->avr_states.menu_state);
				recLog("Предупреждение! Необходимо провести тех. осмотр"); 
			}	
			else menuChangeState(pAVR->avr_states.menu_state);	
			break;
		// необходимо зарядить акб
		case WARN_CHARGE_AKB:
			if(!pAVR->warn.array_flags[WARN_CHARGE_AKB]) {
				pAVR->warn.array_flags[WARN_CHARGE_AKB] = SET;
				pAVR->warn.counter++;
				notification("Предупреждение", "Необходимо зарядить акб", 10, pAVR->avr_states.menu_state);
				recLog("Предупреждение! Необходимо зарядить акб"); 
			}	
			else menuChangeState(pAVR->avr_states.menu_state);	
			break;
		default:
			notification("Предупреждение", "Ошибка вывода предупреждения", 10, pAVR->avr_states.menu_state);
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
			else menuChangeState(pAVR->avr_states.menu_state);	
			break;
		// ошибка отключения реле стартера
		case ERR_STARTER_RELE_SHUTDOWN:
			if(!pAVR->err.array_flags[ERR_STARTER_RELE_SHUTDOWN]) {
				pAVR->err.array_flags[ERR_STARTER_RELE_SHUTDOWN] = SET;
				pAVR->err.counter++;
				notification("Ошибка!!!", "Отключения реле стартера", 30, pAVR->avr_states.menu_state);
				recLog("Ошибка!!! Отключения реле стартера");
			}	
			else menuChangeState(pAVR->avr_states.menu_state);	
			break;
		// ошибка включения реле стартера
		case ERR_STARTER_RELE_ACTIVATION:
			if(!pAVR->err.array_flags[ERR_STARTER_RELE_ACTIVATION]) {
				pAVR->err.array_flags[ERR_STARTER_RELE_ACTIVATION] = SET;
				pAVR->err.counter++;
				notification("Ошибка!!!", "Включения реле стартера", 30, pAVR->avr_states.menu_state);
				recLog("Ошибка!!! Включения реле стартера");
			}	
			else menuChangeState(pAVR->avr_states.menu_state);	
			break;
		// низкое напряжение акб
		case ERR_LOW_VOLTAGE_AKB:
			if(!pAVR->err.array_flags[ERR_LOW_VOLTAGE_AKB]) {
				pAVR->err.array_flags[ERR_LOW_VOLTAGE_AKB] = SET;
				pAVR->err.counter++;
				notification("Ошибка!!!", "Низкое напряжение акб", 30, pAVR->avr_states.menu_state);
				recLog("Ошибка!!! Низкое напряжение акб");
			}	
			else menuChangeState(pAVR->avr_states.menu_state);	
			break;
		// высокое напряжение акб
		case ERR_HIGHT_VOLTAGE_AKB:
			if(!pAVR->err.array_flags[ERR_HIGHT_VOLTAGE_AKB]) {
				pAVR->err.array_flags[ERR_HIGHT_VOLTAGE_AKB] = SET;
				pAVR->err.counter++;
				notification("Ошибка!!!", "Высокое напряжение акб", 30, pAVR->avr_states.menu_state);
				recLog("Ошибка!!! Высокое напряжение акб");
			}	
			else menuChangeState(pAVR->avr_states.menu_state);	
			break;
		// неисправность цепи зарядки
		case ERR_CHARG_CIRCUIT:
			if(!pAVR->err.array_flags[ERR_CHARG_CIRCUIT]) {
				pAVR->err.array_flags[ERR_CHARG_CIRCUIT] = SET;
				pAVR->err.counter++;
				notification("Ошибка!!!", "Неисправность цепи зарядки", 30, pAVR->avr_states.menu_state);
				recLog("Ошибка!!! Неисправность цепи зарядки");
			}	
			else menuChangeState(pAVR->avr_states.menu_state);	
			break;
		// был хард ресет
		case ERR_HARD_RESET:
			if(!pAVR->err.array_flags[ERR_HARD_RESET]) {
				pAVR->err.array_flags[ERR_HARD_RESET] = SET;
				pAVR->err.counter++;
				notification("Ошибка!!!", "Был хард ресет", 30, pAVR->avr_states.menu_state);
				recLog("Ошибка!!! Был хард ресет");
			}	
			else menuChangeState(pAVR->avr_states.menu_state);	
			break;
		// был сброс по вачдогу
		case ERR_WATCH_DOG:
			if(!pAVR->err.array_flags[ERR_WATCH_DOG]) {
				pAVR->err.array_flags[ERR_WATCH_DOG] = SET;
				pAVR->err.counter++;
				notification("Ошибка!!!", "Был сброс по вачдогу", 30, pAVR->avr_states.menu_state);
				recLog("Ошибка!!! Был сброс по вачдогу");
			}	
			else menuChangeState(pAVR->avr_states.menu_state);	
			break;
		// двигатель неуправляемо остановлен (заглох)
		case ERR_ENGINE_STALLED:
			if(!pAVR->err.array_flags[ERR_ENGINE_STALLED]) {
				pAVR->err.array_flags[ERR_ENGINE_STALLED] = SET;
				pAVR->err.counter++;
				notification("Ошибка!!!", "ДВС неуправляемо остановлен (заглох)", 30, pAVR->avr_states.menu_state);
				recLog("Ошибка!!! ДВС неуправляемо остановлен (заглох)");
			}	
			else menuChangeState(pAVR->avr_states.menu_state);	
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
			else menuChangeState(pAVR->avr_states.menu_state);	
			break;
		// закончилось место на sd карте
		case ERR_SD_FREE_SPACE_NULL:
			if(!pAVR->err.array_flags[ERR_SD_FREE_SPACE_NULL]) {
				pAVR->err.array_flags[ERR_SD_FREE_SPACE_NULL] = SET;
				pAVR->err.counter++;
//				notification("Ошибка!!!", "sd карта заполненна", 30, pAVR->avr_states.menu_state);
				recLog("Ошибка!!! sd карта заполненна");
			}	
			else menuChangeState(pAVR->avr_states.menu_state);	
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
		// необходимо зарядить акб //??? сделать отмену предупреждения
		case WARN_CHARGE_AKB:
			if(pAVR->warn.array_flags[WARN_CHARGE_AKB]) {
				pAVR->warn.array_flags[WARN_CHARGE_AKB] = RESET;
				pAVR->warn.counter--;
				recLog("Удалено предупреждение о разряде АКБ");
			}	
			break;
		default:
			notification("Предупреждение", "Ошибка отмены предупреждения", 10, pAVR->avr_states.menu_state);
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
