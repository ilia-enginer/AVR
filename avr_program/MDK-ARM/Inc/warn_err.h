
#ifndef WARN_ERR_H_
#define WARN_ERR_H_

#include "main.h"

void checkWarn(void);
void checkErr(void);

void setWarn(WARN_WARIANTS warn);
void setErr(ERR_WARIANTS err);

void delWarn(WARN_WARIANTS warn);

void resetErrors (void);
void resetWarning (void);

#endif /* WARN_ERR_H_ */
