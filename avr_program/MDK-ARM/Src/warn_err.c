

#include "warn_err.h"


void resetErrors (void)
{
	for(uint8_t i = 0; i < MAX_ERR_AND_WARN; i++)
		pAVR->err.array_flags[i] = 0;

	pAVR->err.counter = 0;
}


void resetWarning (void)
{
	for(uint8_t i = 0; i < MAX_ERR_AND_WARN; i++)
		pAVR->warn.array_flags[i] = 0;

	pAVR->warn.counter = 0;
}