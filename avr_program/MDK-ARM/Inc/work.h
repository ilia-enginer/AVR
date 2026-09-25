
#ifndef WORK_H_
#define WORK_H_

#include <stdint.h>
//#include "defs.h"

void work (void);
void dataCalcADC(void);
float exponentialRunningAverage(float value, float valueNew, float koff);
void switchPowerCircuitBreaker(uint32_t status);

#endif /* WORK_H_ */
