
#ifndef DEFS_H_
#define DEFS_H_

#include "main.h"


/* defines -----------------------------------------------------------*/
// ADC
#define ADC_CHANELS				(6)								// кол-во каналов ацп в массиве

// ошибки и предупреждения
#define MAX_ERR_AND_WARN	(32)							// макс. кол-во ошибок и предупреждений

/* Exported types ------------------------------------------------------------*/

// виды меню
typedef enum {
    MAIN_MENU = 0,				// главное меню
		SERVISE_TOUCH,				// сервисное меню тача
		SET_TIME,							// настройка времени
		SET_DATA,							// настройка даты
		GET_ERROR,						// меню просмотра ошибок
		GET_WARNING,					// меню просмотра предупреждений
		SWICH_AVR,						// меню переключения силового автомата
		SECOND_MENU,					// второе меню
		NOTIFICATION,					// уведомление
		
} MENU_STATE;


// вариатны питания дома
typedef enum  {
    POWER_IS_OFF = 0,				// питание откл
		EXTERNAL_POWER,					// питание от внешней сети
		POWERED_BY_GENERATOR,		// питание от генератора
		
}POWER_GRID_MODE;

// вариатны ошибок
typedef enum  {
    ERR_MAX_LAUNCH_ATTEMP = 0,				// превышено максимальное кол-во попыток запуска
		ERR_STARTER_RELE_SHUTDOWN,				// ошибка отключения реле стартера
		ERR_STARTER_RELE_ACTIVATION,			// ошибка включения реле стартера
		ERR_LOW_VOLTAGE_AKB,							// низкое напряжение акб
		ERR_HIGHT_VOLTAGE_AKB,						// высокое напряжение акб
		ERR_CHARG_CIRCUIT,								// неисправность цепи зарядки
		ERR_HARD_RESET,										// был хард ресет
		ERR_WATCH_DOG,										// был сброс по вачдогу
		ERR_ENGINE_STALLED,								// двигатель неуправляемо остановлен (заглох)
		ERR_SD_CARD,											// ошибка sd карты

}ERR_WARIANTS;

// вариатны предупреждений
typedef enum  {
    WARN_MANUAL_CONTROL_EN = 0,				// включен ручной режим работы
		WARN_NECESSITY_TECH_INSP,					// необходимо провести тех. осмотр
		WARN_CHARGE_AKB,									// необходимо зарядить акб

}WARN_WARIANTS;

// вариатны работы устройства(мозгов) 
typedef enum  {
    AVR_AUTO = 0,				// автоматический режим
		AVR_MANUAL,					// ручной режим	
		
}MANAGEMENT_WORK;


// внешнее питание есть / нет
typedef enum  {
    EXT_POWER_OFF = 0,				// нет
		EXT_POWER_ON,							// есть
		
}STATUS_EXT_POWER;

/* variables ---------------------------------------------------------*/

// для работы с дисплеем
typedef struct {
    const uint8_t width;
    uint8_t height;
    const uint16_t *data;
} FontDef;


typedef struct {
    
	uint8_t flag_press;										// флаг нажатия на экран
	uint32_t time_press;									// время удержания
								
	uint8_t	flag_release;									// флаг отпускания
								
	uint8_t flag_hold;										// флаг удержания
	uint32_t timme_hold;									// время удержания
									
	uint16_t x;														// координаты
	uint16_t y;
	
} TouchDef;



// данные ацп
typedef struct ADC_data {
		
		uint16_t ravADC[ADC_CHANELS];				//сырые данные ацп, 
																				// 0 ячейка - контроль внешнего напряжения
																				// 1 ячейка - контроль напряжения батареи
																				// 2 ячейка - контроль работы мотора
																				// 3 ячейка - контроль включения/отключения реле стартера
																				// 4 ячейка - температура проца
																				// 5 ячейка - опорное напряжение проца
} ADC_data;


// посчитанные параметры напряжения, температуры
typedef struct Vparam_Type {
		
		float v_out;												// внешнее напряжение
		float v_bat;												// напряжение акума
		float v_motor;											// напряжение на обмотке мотора
		float v_rele_starter;								// напряжение на выходе реле стартера
		float v_opora;											// температура проца
		float t_cpu;												// опорное напряжение проца
} Vparam_Type;


// автоматы и флаги прибора
typedef struct automats_devices {

		MENU_STATE				menu_state;					// автомат менюшки на экране
		uint8_t						program_state;			// програмный автомат
		POWER_GRID_MODE		power_grid_mode;		// режим работы (нечего / сеть / генератор)
		uint8_t						flagCharge;					// заряжка акб выкл /  вкл
		MANAGEMENT_WORK		powerAutoManual;		// режим работы управления авто / ручной
		uint8_t						statusEngine;				// работа ДВС генератора выключен / включен
		STATUS_EXT_POWER	extPowerSupply;			// наличие внешнего питания
		
} automats_devices;


// структура для ошибок и предупреждений
typedef struct {
    
	uint8_t counter;											// счетчик
	uint8_t array_flags[MAX_ERR_AND_WARN];// массив флагов 
	
} ErrWarnType;



// тип данных для параметров всего прибора
typedef struct Device_Type {

	automats_devices	avr_states;
	ADC_data 					adc;
	Vparam_Type				v_t;
	TouchDef					touch;
	ErrWarnType				err;
	ErrWarnType				warn;
	
} Device_Type;


extern Device_Type AVR;     // прибор с его характеристиками и параметрами
extern Device_Type *pAVR;

extern RTC_TimeTypeDef sTime;
extern RTC_DateTypeDef DateToUpdate;


extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim2;
extern ADC_HandleTypeDef hadc1;
extern RTC_HandleTypeDef hrtc;

#endif /* DEFS_H_ */
