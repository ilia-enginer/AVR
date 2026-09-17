
#ifndef MENU_MAIN_H_
#define MENU_MAIN_H_

#include <stdint.h>

#define BUF_LEN  (100)


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
