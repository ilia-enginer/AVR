

#include "recalcculating.h"
#include "main.h"


float exponentialRunningAverage(float value, float valueNew, float koff)
{
	return valueNew * koff + value * (1.0 - koff);
}

time_t realToUnix(void)
{
	struct tm tmm = {0}; // Инициализируем нулями
	
	// прочитать дату, время
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN); 				
	HAL_RTC_GetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN);
	
	//------ реальное время ---------
	// дата
	tmm.tm_year = DateToUpdate.Year + 2000 - 1900;	// т.к. хранятся только 2 последние цифры года
	tmm.tm_mon = DateToUpdate.Month-1;
	tmm.tm_mday = DateToUpdate.Date;
	// Заполняем структуру временем 
	tmm.tm_hour = sTime.Hours;
	tmm.tm_min = sTime.Minutes;
	tmm.tm_sec = sTime.Seconds;
	tmm.tm_isdst = -1; // Пусть система решит сама

	// Переводим в Unix-время (в UTC)
	time_t unix_realTime = mktime(&tmm);
	
	return unix_realTime;
}


