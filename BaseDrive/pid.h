#ifndef FUZZY_PID_H
#define FUZZY_PID_H

#include "user.h"

extern float PID_P(float e,float ec);
extern float PID_I(float e,float ec);
extern float PID_D(float e,float ec);

#endif
