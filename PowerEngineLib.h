/*
 * PowerEngineLib.h
 *
 *
 *      Author: batov
 */

#ifndef POWERENGINELIB_H_
#define POWERENGINELIB_H_

#include "msp430.h"


#define First 1
#define Second 2
#define Third 3
#define Fourth 4

#define Forward 2
#define Back 1
#define Neutral 0
#define Block 3

#define Period 400

#define Duty (Power)*(Period-60 )*0.01 //magic...just magic...

int SetEngineMode(int Engine, int Dir, int Power);
int HardwareDefense(int Engine);


#endif /* POWERENGINELIB_H_ */
