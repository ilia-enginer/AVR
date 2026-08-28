
#ifndef DEFS_H_
#define DEFS_H_

#include "main.h"


/* defines -----------------------------------------------------------*/

// ADC
#define ADC_CHANELS				(6)								// кол-во каналов ацп в массиве

// ошибки и предупреждения
#define MAX_ERR_AND_WARN	(32)							// макс. кол-во ошибок и предупреждений


/* Exported types ------------------------------------------------------------*/

// вариатны тача
enum TUCH_WARIANTS {
    NONE = 0,				// ничего
		PRESS,					// нажат
		NO_PRESS,				// отпущен
		LONG_PRESS,			// длительное нажатие
		NO_LONG_PRESS,	// отпущен после длительного нажатия
};

// вариатны питания дома
enum POWER_GRID_MODE {
    POWER_IS_OFF = 0,				// питание откл
		EXTERNAL_POWER,					// питание от внешней сети
		POWERED_BY_GENERATOR,		// питание от генератора
};

// вариатны ошибок
enum ERR_WARIANTS {
    XX = 0,				// 

};

// вариатны предупреждений
enum WARN_WARIANTS {
    YY = 0,				// 

};

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


// автоматы прибора
typedef struct automats_devices {

		uint8_t					menu_state;					// автомат менюшки на экране
		uint8_t					program_state;			// програмный автомат
		uint8_t					power_grid_mode;		// режим работы (сеть / генератор / начего)
		uint8_t					flagCharge;					// режим заряжки вкл / выкл
		
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
