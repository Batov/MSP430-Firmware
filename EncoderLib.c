/*
 * EncoderLib.c
 *
 *      Author: batov
 */
#include "EncoderLib.h"

int TheWheel = 0;
int LengthOfSignal = 0;
int StageOfConversation = -1;

int StartConversation(int Wheel)
{
	P2DIR |= BIT1;
	TheWheel = Wheel;
	StageOfConversation = -1;
	LengthOfSignal = 0;

	switch (Wheel)
	{
	case 1:
	{
		P4DIR &= !(BIT0 + BIT1);
		break;
	}
	case 2:
	{
		P2SEL &= !(BIT5 + BIT2);
		P2DIR &= !(BIT5 + BIT2);
		break;
	}
	}
	TA1CCR0 = Freq;
	TA1CCTL0 = CCIE;
	TA1CTL = TASSEL_2 + MC_1 + TACLR;
	return 0;
}

int GetPeriod()
{
	int x = (LengthOfSignal) * Freq;
	TheWheel = 0;
	LengthOfSignal = 0;
	StageOfConversation = -1;
	return x;
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void) //interrupt for CCR0
{
	switch (TheWheel)
	{
	case 1:
	{
		if (!(P4IN & BIT0))
		{
			if (StageOfConversation == 1)
			{
				TA1CTL = MC_0;
			}
			StageOfConversation = 0;
		}
		else if (StageOfConversation >= 0)
		{
			StageOfConversation = 1;
			LengthOfSignal++;
		}
	}

	case 2:
	{
		if (!(P2IN & BIT5))
		{
			if (StageOfConversation == 1)
			{
				TA1CTL = MC_0;
			}
			StageOfConversation = 0;
		}
		else if (StageOfConversation >= 0)
		{
			StageOfConversation = 1;
			LengthOfSignal++;
		}
		break;
	}
	default:
	{
		break;
	}
	}
}

