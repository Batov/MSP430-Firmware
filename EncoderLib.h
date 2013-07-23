/*
 * EncoderLib.h
 *
 *      Author: batov
 */

#ifndef ENCODERLIB_H_
#define ENCODERLIB_H_
#include "msp430.h"

#define Freq 10

int StartConversation(int Wheel);
int GetPeriod();

#endif /* ENCODERLIB_H_ */
