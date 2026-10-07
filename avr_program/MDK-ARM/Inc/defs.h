
#ifndef DEFS_H_
#define DEFS_H_

#include <time.h>
#include "main.h"


/* defines -----------------------------------------------------------*/
// ------- ADC ---------
#define OPORA_ADC						(3.3f)						// напряжение опоры ацп
#define ADC_CHANELS					(6)								// кол-во каналов ацп в массиве
#define FULL_RANGE_f				(4096.0f)
	
#define KOFF_FILTR					(0.1f)						// коэфициент фильтра для пересчета значений ацп
		 

// ------- напряжение -------
#define U_AKB_MIN_1 				(12.4f)						// вольт первый порог низкого напряжения (порог предупреждения)
#define U_AKB_MIN_0 				(12.1f) 					// вольт нулевой порог низкого напряжения (порог ошибки)
#define U_AKB_MAX 					(15.0f)						// верхний порог напряжения	(порог ошибки)
#define U_GYST							(0.1f)						// гистерезис напряжений для проверки
#define U_V_OUT_MIN					(10.0f)						// нижний порог входного напряжения
#define U_V_OUT_NORM				(12.5f)						// нормальный порог входного напряжения (с учетом падения на стабилитроне. по факту 13,2В)


// Полная формула вычисления измеряемого напряжения будет выглядеть так: U= (опорное напряжение*значение АЦП*коэффициент делителя)/число разрядов АЦП
#define R1_EXT_V						(5100.0f)					// Om
#define R2_EXT_V						(910.0f)					// Om
#define	K_EXT_V	(R2_EXT_V/(R1_EXT_V + R2_EXT_V))		// Коэфициент делителя напряжения

#define R1_AKB							(3325.0f)					// Om
#define R2_AKB							(910.0f)					// Om
#define	K_AKB	(R2_AKB/(R1_AKB + R2_AKB))			// Коэфициент делителя напряжения

#define R1_ENGINE						(10000.0f)				// Om
#define R2_ENGINE						(470.0f)					// Om
#define	K_ENGINE	(R2_ENGINE/(R1_ENGINE + R2_ENGINE))		// Коэфициент делителя напряжения
#define V_FALL_DIODE				(1.4f)						// напряжение падения на диодах

#define R1_REL_STARTER			(3300.0f)					// Om
#define R2_REL_STARTER			(910.0f)					// Om
#define	K_REL_STARTER	(R2_REL_STARTER/(R1_REL_STARTER + R2_REL_STARTER))			// Коэфициент делителя напряжения


// ------- ошибки и предупреждения -------
#define MAX_ERR_AND_WARN	(32)							// макс. кол-во ошибок и предупреждений


// ------- для хранения истории настроек на sd карте -------
#define NUM_VARIABLES_HISTORI		(30)						// кол-во переменных в структуре истории
#define BUF_LEN_SD_PARAM				((NUM_VARIABLES_HISTORI*4) + NUM_VARIABLES_HISTORI)						// объем буфера для хранения параметров, считанных из флеш
#define INIT_HISTORY_FILE_SIGNATURE (2092942078)			// признак инициализации файла истории параметров на sd карте

// ТО
#define INTERVAL_DATA_TO_UNIX		(7889229)	// интервал меж ТО в юниксе (3 месяца)
#define INTERVAL_TO_HOURS				(50)			// интервал меж ТО в моточасах

// ------- для экрана -------
#define BUF_LEN  (100)

#define NULL_BRIGHTNESS					(0)				// нулевая яркость экрана
#define MIN_BRIGHTNESS					(100)			// минимальная яркость экрана
#define MAX_BRIGHTNESS					(1000)		// максимальная яркость экрана

#define LONG_PRESS_RESET				(10000)		// 10c если нажатие на экран дольше этого - перезагрузится проц
#define LONG_PRESS_MAIN_MENU		(5000)		// 5c если нажатие на экран дольше этого - переход в главное меню
#define LONG_NO_PRESS_MAIN_MENU	(300000)	// 5мин. если в течение этого времени не было нажатия на экран - перейти в главное меню 
#define LONG_NO_PRESS_LOW_AKB		(15000)		// 15сек. если в течение этого времени не было нажатия на экран - перейти в главное меню (при разряженнном акб)


// ------- двигатель -------
#define ENGINE_CONTINUOUS_TIME			(4 * 3600)// время непрерывной работы двигателя (в часах)
#define TIME_BREAK_ENGINE						(30 * 60)	// время отдыха генератора в мин
#define MAX_STARTER_OPERATING_TIME	(5)				// максимальное время работы стартера
#define MAX_LAUNCH_ATTEMPTS					(3)				// макс. кол-во попыток запуска
#define WINTER_MODE_TEMPERATURE			(13.0f)		// температура ниже которой переходит на зимний режим
#define MAX_STARTER_BREAK_TIME			(10)			// макс. время перерыва стартера (в сек)
#define START_DETECT_VOLTAGE				(30.0f) 	// напряжение определения запуска
#define STOP_DETECT_VOLTAGE 				(2.0f)		// напряжение определения остановки
#define WARM_UP_TIME								(3 * 60) 	// время прогрева (в мин)
#define COLDING_TIME								(3 * 60)	// время охлаждения генератора в мин


// ------- зарядка -------
#define TIME_CHARGE									(3 * 3600)// время зарядки в часах


/* Exported types ------------------------------------------------------------*/

// ------- виды меню -------
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
		MANUAL_RELE_SWITCH,		// меню ручного переключения реле
		GET_V_MENU,						// меню вывода напряжений
		
} MENU_STATE;


// ------- варианты питания дома -------
typedef enum {
    POWER_IS_OFF = 0,				// питание откл
		EXTERNAL_POWER,					// питание от внешней сети
		POWERED_BY_GENERATOR,		// питание от генератора
		
} POWER_GRID_MODE;

// ------- внешнее питание есть / нет -------
typedef enum  {
    EXT_POWER_OFF = 0,				// нет
		EXT_POWER_ON,							// есть
		
}STATUS_EXT_POWER;

// ------- вариатны ошибок -------
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
		ERR_SD_FREE_SPACE_NULL,						// закончилось место на sd карте

}ERR_WARIANTS;

// ------- вариатны предупреждений -------
typedef enum  {
    WARN_MANUAL_CONTROL_EN = 0,				// включен ручной режим работы
		WARN_NECESSITY_TECH_INSP,					// необходимо провести тех. осмотр
		WARN_CHARGE_AKB,									// необходимо зарядить акб
		WARN_BREAK_ENGINE,								// необходим перерыв двигателя
		WARN_NO_BREAK_ENGINE,							// непрерывная работа двигателя

}WARN_WARIANTS;

// ------- вариатны работы устройства(мозгов) -------
typedef enum  {
    AVR_AUTO = 0,				// автоматический режим
		AVR_MANUAL,					// ручной режим	
		
}MANAGEMENT_WORK;


// ------- статусы двигателя -------
typedef enum  {
    ENGINE_STOPPED = 0,				// остановлен
		ENGINE_START,							// запуск
		ENGINE_STARTER_REST,			// отдых стартера
		ENGINE_WARM_UP,						// прогрев
		ENGINE_WORK,							// работа (запущен)
		ENGINE_COOLING,						// охлаждение
		ENGINE_STOP,							// остановка
		ENGINE_TIMEOUT,						// перерыв
		
}STATUS_ENGINE;

// структура двигателя
typedef struct {
    
	STATUS_ENGINE status;						// статус двигателя			
	time_t starterRotationTime;			// время вращения стартером c
	time_t starterTimeoutTime;			// время отдыха стартера в с
	uint8_t launchAttempts;					// кол-во попыток последнего запуска
	time_t startTimeStop;						// время начала остановки в с
	time_t warmUpTime;							// время прогрева в с
	time_t collingTime;							// время охлаждения в с
	time_t workingTime;							// время работы в с
	time_t engineTimeoutTime;				// время перерыва(охлаждения генератора)
	uint8_t flagTimeout;						// флаг работы с перерывами
	
} EngineDef;



// ------- структура работы с тачем -------
typedef struct {
    
	uint8_t flag_press;										// флаг нажатия на экран							
	uint8_t	flag_release;									// флаг отпускания
								
	uint32_t time_press;									// время нажатия в сис. тиках
									
	uint16_t x;														// координаты
	uint16_t y;
	
} TouchDef;



// ------- данные ацп -------
typedef struct ADC_data {
		
		uint16_t ravADC[ADC_CHANELS];				//сырые данные ацп, 
																				// 0 ячейка - контроль внешнего напряжения
																				// 1 ячейка - контроль напряжения батареи
																				// 2 ячейка - контроль работы мотора
																				// 3 ячейка - контроль включения/отключения реле стартера
																				// 4 ячейка - температура проца
																				// 5 ячейка - опорное напряжение проца
} ADC_data;


// ------- посчитанные параметры напряжения, температуры -------
typedef struct Vparam_Type {
		
		float v_out;												// внешнее напряжение
		float v_bat;												// напряжение акума
		float v_motor;											// напряжение на обмотке мотора
		float v_rele_starter;								// напряжение на выходе реле стартера
		float v_opora;											// опорное напряжение проца 
		float v_cpu;												// напряжение питания проца
		float t_cpu;												// температура проца
} Vparam_Type;


// ------- автоматы и флаги прибора -------
typedef struct automats_devices {

		MENU_STATE				menu_state;					// автомат менюшки на экране
		POWER_GRID_MODE		power_grid_mode;		// режим работы (нечего / сеть / генератор)
		uint8_t						flagCharge;					// зарядка акб выкл /  вкл
		time_t						timeChargeStart;		// время начала зарядки
		MANAGEMENT_WORK		powerAutoManual;		// режим работы управления авто / ручной
		STATUS_EXT_POWER	extPowerSupply;			// наличие внешнего питания
		uint8_t						flagSaveInfoSD;			// флаг сохранения информации о работе генератора на sd карту
		uint8_t						oledWork;						// флаг работы экрана
		
} automats_devices;


// ------- структура для ошибок и предупреждений -------
typedef struct {
    
	uint8_t counter;											// счетчик
	uint8_t array_flags[MAX_ERR_AND_WARN];// массив флагов 
	
} ErrWarnType;

// ------- структура параметров которые хранятся на sd карте -------
typedef struct {
  
	// проверка
	uint32_t checkNum;										// проверочное число инициализации
	
	// моточасы				
	uint32_t engineHoursTotal;						// моточасы всего
	uint32_t engineMinutesTotal;					// мотоминуты всего
	uint32_t engineHoursTO;								// моточасы после ТО
	uint32_t engineMinutesTO;							// мотоминуты после ТО
	uint32_t hoursBeforeTO;								// моточасы до ТО
	uint32_t minutesBeforeTO;							// мотоминуты до ТО
					
	// ТО				
	uint32_t hoursLastTO;									// час последнего ТО
	uint32_t minutesLastTO;								// минуты последнего ТО
	uint32_t secondsLastTO;								// секунды последнего ТО
	uint32_t dateLastTO;									// дата последнего ТО
	uint32_t monthLastTO;									// месяц последнего ТО
	uint32_t yearLastTO;									// год последнего ТО
					
	uint32_t hoursNextTO;									// час следующего ТО
	uint32_t minutesNextTO;								// минуты следующего ТО
	uint32_t secondsNextTO;								// секунды следующего ТО
	uint32_t dateNextTO;									// дата следующего ТО
	uint32_t monthNextTO;									// месяц следующего ТО
	uint32_t yearNextTO;									// год следующего ТО
	
	// отключение эл-ва
	uint32_t hoursWithoutElectric;				// час последнего отключения
	uint32_t minutesWithoutElectric;			// минуты последнего отключения
	uint32_t secondsWithoutElectric;			// секунды последнего отключения
	uint32_t dateWithoutElectric;					// дата последнего отключения
	uint32_t monthWithoutElectric;				// месяц последнего отключения
	uint32_t yearWithoutElectric;					// год последнего отключения
	uint32_t minutesLastWithoutElectric;	// минуты без эл-ва за последний раз	
	
	uint32_t hoursALLWithoutElectric;			// общее кол-во часов без эл-ва
	uint32_t minutesALLWithoutElectric;		// общее кол-во минут без эл-ва
	
	// запуск ДВС инфо
	uint32_t numSuccessLaunch;						// кол-во удачных запусков
	uint32_t numLaunchAttempt;						// кол-во попыток запуска
	
} SdParametersType;

		
// ------- тип данных для параметров всего прибора -------
typedef struct Device_Type {

	automats_devices	avr_states;		// автоматы и флаги прибора
	ADC_data 					adc;					// данные ацп
	Vparam_Type				v_t;					// посчитанные параметры напряжения, температуры
	TouchDef					touch;				// структура работы с тачем
	ErrWarnType				err;					// структура ошибок
	ErrWarnType				warn;					// структура предупреждений
	SdParametersType	sdParams;			// структура параметров которые хранятся на sd карте
	EngineDef					engine;				// структура двигателя
	
} Device_Type;


// прибор
extern Device_Type AVR;     // прибор с его характеристиками и параметрами
extern Device_Type *pAVR;

extern char buf[BUF_LEN];		// массив для вывода информации
extern uint32_t FreeSpace;	// хранит остаток места на sd карте

// hal
extern RTC_TimeTypeDef sTime;
extern RTC_DateTypeDef DateToUpdate;

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern TIM_HandleTypeDef htim2;
extern ADC_HandleTypeDef hadc1;
extern IWDG_HandleTypeDef hiwdg;
extern RTC_HandleTypeDef hrtc;



#endif /* DEFS_H_ */
