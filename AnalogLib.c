/*
 * EncoderLib.c
 *
 *      Author: batov
 */
#include "AnalogLib.h"
#include "TypesOfRequsts.h"
#include "adc10_a.h"

int TheTypeOfRequset = 0;
int LengthOfSignal = 0;
int StageOfConversation = -1;
int ADCStatus = 0;

int StartConversation(int TypeOfRequest)
{
	TheTypeOfRequset = TypeOfRequest;

	switch (TypeOfRequest)
	{
	case FirstEncoder:
	{
		P4DIR &= !(BIT0 + BIT1);
		TA1CCR0 = FreqEncoder;
		TA1CCTL0 = CCIE;
		TA1CTL = TASSEL_2 + MC_1 + TACLR;
		break;
	}
	case SecondEncoder:
	{
		P2SEL &= !(BIT5 + BIT2);
		P2DIR &= !(BIT5 + BIT2);
		TA1CCR0 = FreqEncoder;
		TA1CCTL0 = CCIE;
		TA1CTL = TASSEL_2 + MC_1 + TACLR;
		break;
	}
	case PING:
	{
		P6DIR |= BIT1;
		P6OUT |= BIT1;
		__delay_cycles(10);
		P6OUT &= ~BIT1;
		P6DIR &= ~BIT1;

		TA1CCR0 = FreqPing;
		TA1CCTL0 = CCIE;
		TA1CTL = TASSEL_2 + MC_1 + TACLR;

		break;
	}
	case HCSR04:
	{

		P6DIR |= BIT1;
		P3DIR |= BIT2;

		P3OUT &= ~BIT2;
		P6OUT &= ~BIT1;
		__delay_cycles(10);

		P6DIR &= ~BIT1;
		P3DIR &= ~BIT2;

		TA1CCR0 = FreqPing;
		TA1CCTL0 = CCIE;
		TA1CTL = TASSEL_2 + MC_1 + TACLR;
		break;
	}
	case Color:
	{
		if (ADCStatus == 0)
		{
			ADC10_A_init(ADC10_A_BASE, ADC10_A_SAMPLEHOLDSOURCE_SC,
					ADC10_A_CLOCKSOURCE_ADC10OSC, ADC10_A_CLOCKDIVIDER_1);

			ADC10_A_enable(ADC10_A_BASE);

			ADC10_A_setupSamplingTimer(ADC10_A_BASE,
					ADC10_A_CYCLEHOLD_16_CYCLES,
					ADC10_A_MULTIPLESAMPLESDISABLE);
			ADC10_A_memoryConfigure(ADC10_A_BASE, ADC10_A_INPUT_A1, //HERE select!!!
					ADC10_A_VREFPOS_AVCC, ADC10_A_VREFNEG_AVSS);
			ADCStatus = 1;
		}

		ADC10_A_startConversion(ADC10_A_BASE, ADC10_A_SINGLECHANNEL);
		break;
	}
	case Sharp:
	{

		if (ADCStatus == 0)
		{
			ADC10_A_init(ADC10_A_BASE, ADC10_A_SAMPLEHOLDSOURCE_SC,
					ADC10_A_CLOCKSOURCE_ADC10OSC, ADC10_A_CLOCKDIVIDER_1);

			ADC10_A_enable(ADC10_A_BASE);

			ADC10_A_setupSamplingTimer(ADC10_A_BASE,
					ADC10_A_CYCLEHOLD_16_CYCLES,
					ADC10_A_MULTIPLESAMPLESDISABLE);
			ADC10_A_memoryConfigure(ADC10_A_BASE, ADC10_A_INPUT_A3, //HERE select!!!
					ADC10_A_VREFPOS_AVCC, ADC10_A_VREFNEG_AVSS);
			ADCStatus = 1;
		}

		ADC10_A_startConversion(ADC10_A_BASE, ADC10_A_SINGLECHANNEL);
		break;
	}
	}

	return 0;
}

int GetResult()
{
	int x = 0;
	switch (TheTypeOfRequset)
	{
	case Sharp:
	{
		ADC10_A_disableConversions(ADC10_A_BASE, ADC10_A_COMPLETECONVERSION);
		x = ADC10_A_getResults(ADC10_A_BASE);

		break;
	}
	case Color:
	{
		ADC10_A_disableConversions(ADC10_A_BASE, ADC10_A_COMPLETECONVERSION);
		x = ADC10_A_getResults(ADC10_A_BASE);

		break;
	}
	case HCSR04:
	{
		x = ((LengthOfSignal) * FreqPing / 2);
		break;
	}
	case PING:
	{
		x = ((LengthOfSignal) * FreqPing / 2);
		break;
	}
	case FirstEncoder:
	{
		x = (LengthOfSignal) * FreqEncoder;
		break;
	}
	case SecondEncoder:
	{
		x = (LengthOfSignal) * FreqEncoder;
		break;
	}
	}
	TheTypeOfRequset = 0;
	LengthOfSignal = 0;
	StageOfConversation = -1;
	return x;
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void) //interrupt for CCR0
{
	switch (TheTypeOfRequset)
	{
	case FirstEncoder:
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

	case SecondEncoder:
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
	case PING:
	{
		if (P6IN & BIT1)
		{
			StageOfConversation = 1;
			LengthOfSignal++;
		}
		else if (StageOfConversation == 1)
		{
			TA1CTL = MC_0;
		}
		break;
	}

	case HCSR04:
	{
		if (P6IN & BIT1)
		{
			StageOfConversation = 1;
			LengthOfSignal++;
		}
		else if (StageOfConversation == 1)
		{
			TA1CTL = MC_0;
		}
		break;
	}
	default:
	{
		break;
	}
	}
}

