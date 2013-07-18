//Nikita Batov

#include "inc/hw_memmap.h"
#include "usci_b_i2c.h"
#include "wdt_a.h"
#include "gpio.h"

#define FirstByte (Engine == 0)
#define ResetOptions {Engine = 0; Dir = 0;}
#define SLAVE_ADDRESS 0x48

#define First 1
#define Second 2
#define Third 3
#define Fourth 4
#define Forward 2
#define Back 1
#define Neutral 0
#define Block 3

char GoToBSL = 0;
char getter = 0;
char Engine = 0;
char Dir = 0;

int HS = 0;
int Period = 400;

#define Duty (Power)*(Period-60 )*0.01 //magic...just magic...
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
	GDir[Engine] = Dir;
	GPower[Engine] = Power;

	if ((Power == 0) && (Dir != Block)) //hack for power off
		Dir = 0;
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

int EnablePower(int Engine) // check hardware protection and activate engines
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
	__delay_cycles(50);
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
		EnablePower(First);
		EnablePower(Second);
		EnablePower(Third);
		EnablePower(Fourth);
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

void main()
{
	//Stop WDT
	WDT_A_hold(WDT_A_BASE);

	//Assign I2C pins to USCI_B0
	GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
			GPIO_PIN1 + GPIO_PIN0);

	//Initialize I2C as a slave device
	USCI_B_I2C_slaveInit(USCI_B0_BASE, SLAVE_ADDRESS);

	//Specify transmit +receive mode
	USCI_B_I2C_setMode(USCI_B0_BASE,
			USCI_B_I2C_RECEIVE_MODE + USCI_B_I2C_TRANSMIT_MODE);

	//Enable I2C Module to start operations
	USCI_B_I2C_enable(USCI_B0_BASE);

	//Enable interrupts
	USCI_B_I2C_clearInterruptFlag(USCI_B0_BASE,
			USCI_B_I2C_RECEIVE_INTERRUPT + USCI_B_I2C_TRANSMIT_INTERRUPT
					+ USCI_B_I2C_STOP_INTERRUPT);
	USCI_B_I2C_enableInterrupt(USCI_B0_BASE,
			USCI_B_I2C_RECEIVE_INTERRUPT + USCI_B_I2C_TRANSMIT_INTERRUPT
					+ USCI_B_I2C_STOP_INTERRUPT);

	while (1)
	{
		//Enter low power mode 0 with interrupts enabled.
		// __no_operation();
//		if (HS >= 20)
//		{
//			EnablePower(First);
//			EnablePower(Second);
//			EnablePower(Third);
//			EnablePower(Fourth);
//			HS = 0;
//		}
		__bis_SR_register(LPM0_bits + GIE);
	}
}

//******************************************************************************
//
//This is the USCI_B0 interrupt vector service routine.
//
//******************************************************************************
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
{
	switch (__even_in_range(UCB0IV, 12))
	{
	case USCI_I2C_UCRXIFG:
	{
		getter = USCI_B_I2C_slaveDataGet(USCI_B0_BASE); //double interrupt: first time = first byte;

		if (getter == 0xFF)
		{
			if (GoToBSL)
			{
				__disable_interrupt();
				((void (*)()) 0x1000)();
			}
			GoToBSL = 1;
		}
		else if (FirstByte)
		{
			Engine = getter >> 2; //parsing first byte ( read bytes format)
			Dir = getter & 3;
		}
		else
		{
			//	EnablePower(Engine); // check hardware protection and activate engines
			SetEngineMode(Engine, Dir, getter);
			ResetOptions

		}

		break;
	}
	case USCI_I2C_UCTXIFG:
	{
		//	USCI_B_I2C_slaveDataPut(USCI_B0_BASE, getter);
		//interrupt for transmit
		break;
	}
	case USCI_I2C_UCSTPIFG:
	{

		//stop interrupt
		__bic_SR_register_on_exit(LPM0_bits);
		break;
	}
	}
}

