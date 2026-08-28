
#ifndef MENU_MAIN_H_
#define MENU_MAIN_H_

#include "main.h"



// виды меню
enum menuState {
    MAIN_MENU = 0,		// главное меню
		SERVISE_TOUCH = 1,		// сервисное меню тача
};

void menuChangeState (uint32_t state);
void menuSwich (void);
void menuMain (void);
uint8_t getTouch (void);

void serviseTouch (void);


#endif /* MENU_MAIN_H_ */
