
#ifndef MENU_MAIN_H_
#define MENU_MAIN_H_

#include "main.h"
#include "menu_servise.h"
#include "touch.h"
#include "settDataTime.h"
#include "menu_second.h"
#include "menu_warn_err.h"


#define BUF_LEN  (96)


// вариатны возвращаемые функцией подтверждения
enum CONFIRM_WARIANTS {
    NOTHING = 0,				
		YES,					
		NO,				
};

void menuChangeState (uint32_t state);
void menuSwich (void);
void menuMain (void);
uint8_t switchAvrAutomatic (void);
uint8_t confirmClick (const char* text);



#endif /* MENU_MAIN_H_ */
