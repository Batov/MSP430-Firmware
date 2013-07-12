#include <msp430.h> 
#include "Power.h"

#define MAXSPEED 100
#define Forward 2
#define Back 1
#define Neutral 0
#define Block 3

int Period = 400;

#define Duty (Power)*(Period-70)*0.01 //magic...just magic...

struct tEngines //global struct for direction of each engines
{
	struct FirstEngine
	{
		short int Dir;
	} FirstEngine;

	struct SecondEngine
	{
		short int Dir;
	} SecondEngine;
	struct ThirdEngine
	{
		short int Dir;
	} ThirdEngine;
	struct FourthEngine
	{
		short int Dir;
	} FourthEngine;
} tEngines;

struct tEngines Engines;

int EnablePower() // check hardware protection and activate engines
{
	P5DIR &= !(BIT4+ BIT5);
		P3DIR &= !(BIT3+ BIT4);

		if (P5IN & BIT4)
		{
			PJDIR |= BIT0;
			PJOUT |= BIT0;
		}
		if (P5IN & BIT5)
		{
			PJDIR |= BIT1;
			PJOUT |= BIT1;
		}
		if (P3IN & BIT3)
		{
			PJDIR |= BIT2;
			PJOUT |= BIT2;
		}
		if (P3IN & BIT4)
		{
			PJDIR |= BIT3;
			PJOUT |= BIT3;
		}
		__delay_cycles(10);

		PJOUT &= !(BIT0 + BIT1 + BIT2 + BIT3);
		return 0;

}

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
	switch (Engine)
	{
	case First:
	{
		P1DIR |= BIT1 + BIT2;
		Engines.FirstEngine.Dir = Dir;
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
			TA0CTL = MC_0;
			P1OUT &= !(BIT1 + BIT2);
			break;
		}
		case Block:
		{
			TA0CTL = MC_0;
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
		Engines.SecondEngine.Dir = Dir;

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
			TA0CTL = MC_0;
			P1OUT &= !(BIT3 + BIT4);
			break;
		}
		case Block:
		{
			TA0CTL = MC_0;
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
		Engines.ThirdEngine.Dir = Dir;
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
			TA1CTL = MC_0;
			P1OUT &= !(BIT7);
			P2OUT &= !(BIT0);
			break;
		}
		case Block:
		{
			TA0CTL = MC_0;
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
		Engines.FourthEngine.Dir = Dir;

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
			TA0CTL = MC_0;
			P2OUT &= !(BIT3 + BIT4);
			break;
		}
		case Block:
		{
			TA0CTL = MC_0;
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




#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void) //interrupt for CCR0
{
	if (Engines.FirstEngine.Dir == Forward)
		P1OUT |= BIT1;
	else
		P1OUT |= BIT2;

	if (Engines.SecondEngine.Dir == Forward)
		P1OUT |= BIT3;
	else
		P1OUT |= BIT4;

	if (Engines.ThirdEngine.Dir == Forward)
		P1OUT |= BIT7;
	else
		P2OUT |= BIT0;

	if (Engines.FourthEngine.Dir == Forward)
		P2OUT |= BIT3;
	else
		P2OUT |= BIT4;
}
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void) //interrupt for CCR1-4
{
	switch (__even_in_range(TA0IV, TA0IV_TAIFG))
	{
	case TA0IV_TACCR1:
	{
		if (Engines.FirstEngine.Dir == Forward)
			P1OUT &= !BIT1;
		else
			P1OUT &= !BIT2;
		break;
	}
	case TA0IV_TACCR2:
	{
		if (Engines.SecondEngine.Dir == Forward)
			P1OUT &= !BIT3;
		else
			P1OUT &= !BIT4;
		break;
	}
	case TA0IV_TACCR3:
	{
		if (Engines.ThirdEngine.Dir == Forward)
			P1OUT &= !BIT7;
		else
			P2OUT &= !BIT0;
		break;
	}
	case TA0IV_TACCR4:
	{
		if (Engines.FourthEngine.Dir == Forward)
			P2OUT &= !BIT3;
		else
			P2OUT &= !BIT4;
		break;
	}
	default:
		break;
	}

}

