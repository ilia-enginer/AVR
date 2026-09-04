
#ifndef POP_UP_WINDOW_H_
#define POP_UP_WINDOW_H_

#include "main.h"



uint8_t confirmClick (const char* text);		// всплывающее окно подтверждения действия
void notification(const char* header, const char* text, uint32_t time, uint8_t menu);
uint16_t lineAlignment(const char* text, char *buf, uint16_t lenLine);


#endif /* POP_UP_WINDOW_H_ */
