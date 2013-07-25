/*
 * EncoderLib.h
 *
 *      Author: batov
 */

#ifndef ENCODERLIB_H_
#define ENCODERLIB_H_
#include "msp430.h"

#define FreqEncoder 10
#define FreqPing 2

int StartConversation(int Wheel);
int GetResult();

#endif /* ENCODERLIB_H_ */
