
#ifndef TOUCH_H_
#define TOUCH_H_

#include "main.h"


// вариатны тача
enum TUCH_WARIANTS {
    NONE = 0,				// ничего
		PRESS,					// нажат
		NO_PRESS,				// отпущен
		LONG_PRESS,			// длительное нажатие
		NO_LONG_PRESS,	// отпущен после длительного нажатия
};


uint8_t getTouch (void);



#endif /* TOUCH_H_ */
