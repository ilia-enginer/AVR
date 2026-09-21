

#include "touch.h"


// работа с тачем
static uint32_t time_press = 0;
uint8_t getTouch (void)
{
//    NONE = 0,				// ничего
//		PRESS,					// нажат
//		NO_PRESS,				// отпущен
//		LONG_PRESS,			// длительное нажатие
//		NO_LONG_PRESS,	// отпущен после длительного нажатия

	// для задержки опроса
	if(HAL_GetTick() - time_press < 30)	return NONE;
	time_press = HAL_GetTick();
	
	// если нажат и не отпущен
	if(pAVR->touch.flag_press == SET && pAVR->touch.flag_release == RESET)
	{
			// запросить координаты
			pAVR->touch.x = 0;
			pAVR->touch.y = 0;

			TOUCH_CS_UNSELECT;
			DISP_CS_UNSELECT;
			HAL_SPI_DeInit(DISP_SPI_PTR);
			
			hspi1.Instance = SPI1;
			hspi1.Init.Mode = SPI_MODE_MASTER;
			hspi1.Init.Direction = SPI_DIRECTION_2LINES;
			hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
			hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
			hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
			hspi1.Init.NSS = SPI_NSS_SOFT;
			hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
			hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
			hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
			hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
			hspi1.Init.CRCPolynomial = 10;
			HAL_SPI_Init(DISP_SPI_PTR);

			ILI9341_TouchGetCoordinates(&pAVR->touch.x, &pAVR->touch.y);			
			
			HAL_SPI_DeInit(DISP_SPI_PTR);
			hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
			HAL_SPI_Init(DISP_SPI_PTR);

			__HAL_SPI_ENABLE(DISP_SPI_PTR);
			DISP_CS_SELECT;	
			
			// если координаты получены
			if(pAVR->touch.x || pAVR->touch.y)
				return PRESS;
	}
	// если отпущен
	else if(pAVR->touch.flag_release == SET)
	{
		pAVR->touch.flag_press = RESET;
		pAVR->touch.flag_release = RESET;
		time_press = HAL_GetTick();
		return NO_PRESS;
	}
			
	return NONE;
}

