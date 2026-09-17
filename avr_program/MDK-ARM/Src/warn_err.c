


#include "warn_err.h"
#include "popUpWindow.h"


void checkWarn(void)
{


//WARN_NECESSITY_TECH_INSP,					// необходимо провести тех. осмотр
//WARN_CHARGE_AKB,									// необходимо зарядить акб
}


void checkErr(void)
{


}



void setWarn(WARN_WARIANTS warn)
{
	switch(warn){
		// включен ручной режим работы
		case WARN_MANUAL_CONTROL_EN:
			if(!pAVR->warn.array_flags[WARN_MANUAL_CONTROL_EN]) {
				pAVR->warn.array_flags[WARN_MANUAL_CONTROL_EN] = SET;
				pAVR->warn.counter++;
				notification("Предупреждение", "Включен ручной режим работы", 10, pAVR->avr_states.menu_state);
				recLog("Пользователь - переход в ручной режим работы"); 
			}	
			break;
		// необходимо провести тех. осмотр
		case WARN_NECESSITY_TECH_INSP:
			if(!pAVR->warn.array_flags[WARN_NECESSITY_TECH_INSP]) {
				pAVR->warn.array_flags[WARN_NECESSITY_TECH_INSP] = SET;
				pAVR->warn.counter++;
				notification("Предупреждение", "Необходимо провести тех. осмотр", 10, pAVR->avr_states.menu_state);
				recLog("Предупреждение! Необходимо провести тех. осмотр"); 
			}	
			break;
		// необходимо зарядить акб
		case WARN_CHARGE_AKB:
			if(!pAVR->warn.array_flags[WARN_CHARGE_AKB]) {
				pAVR->warn.array_flags[WARN_CHARGE_AKB] = SET;
				pAVR->warn.counter++;
				notification("Предупреждение", "Необходимо зарядить акб", 10, pAVR->avr_states.menu_state);
				recLog("Предупреждение! Необходимо зарядить акб"); 
			}	
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
			break;
		// ошибка отключения реле стартера
		case ERR_STARTER_RELE_SHUTDOWN:
			if(!pAVR->err.array_flags[ERR_STARTER_RELE_SHUTDOWN]) {
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
//				notification("Ошибка!!!", "sd карты", 30, pAVR->avr_states.menu_state);
				recLog("Ошибка!!! sd карты");
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
