

#include "menu_servise.h"


// отладка тача
void serviseTouch (void)
{
	char buf[BUF_LEN] = {0,};
	uint8_t status = getTouch();
	static uint8_t flagExit = RESET;
	static uint8_t flagFirst = RESET;		// первый вход в функцию
	
	if(!flagFirst)		// если первый вход в функцию - сразу вывести координаты
	{
		ILI9341_Fill_Screen(MYFON);	// залить экран
		snprintf(buf, BUF_LEN, "X = %d, Y = %d", pAVR->touch.x, pAVR->touch.y); 
		ILI9341_WriteString(75, 10, buf, Font_11x18, WHITE, MYFON);
		ILI9341_Draw_Filled_Circle(pAVR->touch.x, pAVR->touch.y, 10, WHITE);
		flagFirst = SET;
	}
	
	// для подтверждения выхода
	if(flagExit)
	{
		status = confirmClick("Выйти?");

		if(status == YES)
		{
			menuChangeState(SECOND_MENU);
			flagExit = RESET;	
			flagFirst = RESET;
		}
		else if(status == NO)
			flagExit = RESET;
			
		return;
	}
	
	if(status == NO_PRESS || status == NO_LONG_PRESS)
	{	
		snprintf(buf, BUF_LEN, "X = %d, Y = %d", pAVR->touch.x, pAVR->touch.y); 
		ILI9341_Fill_Screen(MYFON);
		ILI9341_WriteString(75, 10, buf, Font_11x18, WHITE, MYFON);
		ILI9341_Draw_Filled_Circle(pAVR->touch.x, pAVR->touch.y, 10, WHITE);
	}
	
	//------------ слева сверху иконка входа в меню -----------------------
	ILI9341_Draw_Rectangle(10, 10, 30, 5, WHITE);
	ILI9341_Draw_Rectangle(10, 20, 30, 5, WHITE);
	ILI9341_Draw_Rectangle(10, 30, 30, 5, WHITE);
	
	//------------ обработка тач -----------------------		
	// если отпущен после нажатия или после длительного нажатия
	if(status == NO_PRESS)
	{
		// если нажатие в области кнопки меню
		if(pAVR->touch.x >= 10 && pAVR->touch.x <= 40 && pAVR->touch.y >= 10 && pAVR->touch.y <= 35) // если нажатие происходит в области этих координат
			flagExit = SET;	
	}
}

