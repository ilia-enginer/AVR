
#ifndef RECALCCULATING_H_
#define RECALCCULATING_H_

#include <stdint.h>
#include <time.h>
//#include "defs.h"


float exponentialRunningAverage(float value, float valueNew, float koff);

time_t realToUnix(void);


#endif /* RECALCCULATING_H_ */
