/*
 * PowerEngineLib.c
 *
 *
 *      Author: batov
 */
#include "PowerEngineLib.h"

int HS = 0;
short int GDir[10];
short int GPower[10];

int SetEngineMode(int Engine, int Dir, int Power)
{
//all engines use one Timer = A0
//CCR1-4  - Duty 1-4 engines
//CCR0 - Period
//changing gpio pins in interrupts

	P1SEL = 0; // use pins how gpio
	P2SEL = 0;

	if ((Power == 0) && (Dir != Block)) //hack for power off
		Dir = 0;

	GDir[Engine] = Dir;
	GPower[Engine] = Power;


	switch (Engine)
	{
	case First:
	{
		P1DIR |= BIT1 + BIT2;
		switch (Dir)
		{
		case Back:
		{
			P1OUT &= !BIT2; //reset on 0 second pin
			TA0CCR0 = Period; //set compare register
			TA0CCR1 = Duty; //set second compare register
			TA0CCTL0 = CCIE; // enable interrupts on both registers
			TA0CCTL1 = CCIE;
			TA0CTL = TASSEL_2 + MC_1 + TACLR + TAIE; //start timer
			break;
		}
		case Forward:
		{
			P1OUT &= !BIT1; //reset on 0 second pin
			TA0CCR0 = Period;
			TA0CCR1 = Duty;
			TA0CCTL1 = CCIE;
			TA0CCTL0 = CCIE;
			TA0CTL = TASSEL_2 + MC_1 + TACLR;
			break;
		}
		case Neutral:
		{
			P1OUT &= !(BIT1 + BIT2);
			break;
		}
		case Block:
		{
			P1OUT |= BIT1 + BIT2;
			break;
		}

		default:
		{
			return 1;
		}
		}

		break;
	}
	case Second:
	{
		P1DIR |= BIT3 + BIT4;
		switch (Dir)
		{
		case Back:
		{
			P1OUT &= !BIT4; //reset on 0 second pin
			TA0CCR0 = Period;
			TA0CCR2 = Duty;
			TA0CCTL0 = CCIE;
			TA0CCTL2 = CCIE;
			TA0CTL = TASSEL_2 + MC_1 + TACLR;
			break;
		}
		case Forward:
		{
			P1OUT &= !BIT3; //reset on 0 second pin
			TA0CCR0 = Period;
			TA0CCR2 = Duty;
			TA0CCTL0 = CCIE;
			TA0CCTL2 = CCIE;
			TA0CTL = TASSEL_2 + MC_1 + TACLR;
			break;
		}
		case Neutral:
		{
			P1OUT &= !(BIT3 + BIT4);
			break;
		}
		case Block:
		{
			P1OUT |= BIT3 + BIT4;
			break;
		}

		default:
		{
			return 1;
		}
		}

		break;
	}
	case Third:
	{
		P1DIR |= BIT7;
		P2DIR |= BIT0;
		switch (Dir)
		{
		case Back:
		{
			P2OUT &= !BIT0; //reset on 0 second pin
			TA0CCR0 = Period;
			TA0CCR3 = Duty;
			TA0CCTL3 = CCIE;
			TA0CCTL0 = CCIE;
			TA0CTL = TASSEL_2 + MC_1 + TACLR;

			break;
		}
		case Forward:
		{
			P1OUT &= !BIT7; //reset on 0 second pin
			TA0CCR0 = Period;
			TA0CCR3 = Duty;
			TA0CCTL3 = CCIE;
			TA0CCTL0 = CCIE;
			TA0CTL = TASSEL_2 + MC_1 + TACLR;
			break;
		}
		case Neutral:
		{
			P1OUT &= !(BIT7);
			P2OUT &= !(BIT0);
			break;
		}
		case Block:
		{
			P1OUT |= BIT7;
			P2OUT |= BIT0;
			break;
		}

		default:
		{
			return 1;
		}
		}

		break;
	}
	case Fourth:
	{
		P2DIR |= BIT3 + BIT4;

		switch (Dir)
		{
		case Back:
		{
			P2OUT &= !BIT4; //reset on 0 second pin
			TA0CCR0 = Period;
			TA0CCR4 = Duty;
			TA0CCTL4 = CCIE;
			TA0CCTL0 = CCIE;
			TA0CTL = TASSEL_2 + MC_1 + TACLR;

			break;
		}
		case Forward:
		{
			P2OUT &= !BIT3; //reset on 0 second pin
			TA0CCR0 = Period;
			TA0CCR4 = Duty;
			TA0CCTL4 = CCIE;
			TA0CCTL0 = CCIE;
			TA0CTL = TASSEL_2 + MC_1 + TACLR;
			break;
		}
		case Neutral:
		{
			P2OUT &= !(BIT3 + BIT4);
			break;
		}
		case Block:
		{
			P2OUT |= BIT3 + BIT4;
			break;
		}

		default:
		{
			return 1;
		}
		}

		break;
	}
	default:
	{
		return 1;
	}
	}
	return 0;
}

int HardwareDefense(int Engine) // check hardware protection and activate engines
{
	P5DIR &= !(BIT4 + BIT5);
	P3DIR &= !(BIT3 + BIT4);
	switch (Engine)
	{
	case First:
	{
		if (P5IN & BIT4)
		{
			PJDIR |= BIT0;
			PJOUT |= BIT0;
		}
		break;
	}
	case Second:
	{
		if (P5IN & BIT5)
		{
			PJDIR |= BIT1;
			PJOUT |= BIT1;
		}
		break;
	}
	case Third:
	{
		if (P3IN & BIT3)
		{
			PJDIR |= BIT2;
			PJOUT |= BIT2;
		}
		break;
	}
	case Fourth:
	{
		if (P3IN & BIT4)
		{
			PJDIR |= BIT3;
			PJOUT |= BIT3;
		}
		break;

	}
	}
	__delay_cycles(10);
	PJOUT &= !(BIT0 + BIT1 + BIT2 + BIT3);
	__delay_cycles(10);
	SetEngineMode(Engine, GDir[Engine], GPower[Engine]);
	return 0;

}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void) //interrupt for CCR0
{
	if (GDir[First] == Forward)
		P1OUT |= BIT1;
	else if (GDir[First] == Back)
		P1OUT |= BIT2;

	if (GDir[Second] == Forward)
		P1OUT |= BIT3;
	else if (GDir[Second] == Back)
		P1OUT |= BIT4;

	if (GDir[Third] == Forward)
		P1OUT |= BIT7;
	else if (GDir[Third] == Back)
		P2OUT |= BIT0;

	if (GDir[Fourth] == Forward)
		P2OUT |= BIT3;
	else if (GDir[Fourth] == Back)
		P2OUT |= BIT4;
	HS++;
	if (HS >= 200)
	{
		HardwareDefense(First);
		HardwareDefense(Second);
		HardwareDefense(Third);
		HardwareDefense(Fourth);
		HS = 0;
	}
}
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void) //interrupt for CCR1-4
{
	switch (__even_in_range(TA0IV, TA0IV_TAIFG))
	{
	case TA0IV_TACCR1:
	{
		if (GDir[First] == Forward)
			P1OUT &= !BIT1;
		else if (GDir[First] == Back)
			P1OUT &= !BIT2;
		break;
	}
	case TA0IV_TACCR2:
	{
		if (GDir[Second] == Forward)
			P1OUT &= !BIT3;
		else if (GDir[Second] == Back)
			P1OUT &= !BIT4;
		break;
	}
	case TA0IV_TACCR3:
	{
		if (GDir[Third] == Forward)
			P1OUT &= !BIT7;
		else if (GDir[Third] == Back)
			P2OUT &= !BIT0;
		break;
	}
	case TA0IV_TACCR4:
	{
		if (GDir[Fourth] == Forward)
			P2OUT &= !BIT3;
		else if (GDir[Fourth] == Back)
			P2OUT &= !BIT4;
		break;
	}
	default:
		break;
	}

}

