
#ifndef ENGINE_H_
#define ENGINE_H_

#include <stdint.h>

uint8_t startStopEngine(uint32_t status);
void engineWork(void);
uint8_t checkErrEngine(void);
uint8_t releZajigOnOff(uint8_t status);
void incrementEngineHours(void);


#endif /* ENGINE_H_ */
