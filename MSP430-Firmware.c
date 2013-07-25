//Nikita Batov

#include "inc/hw_memmap.h"
#include "usci_b_i2c.h"
#include "wdt_a.h"
#include "gpio.h"

#include "PowerEngineLib.h"
#include "AnalogLib.h"
#include "TypesOfRequsts.h"

uint8_t StageOfSending = 1;
uint8_t ResultPart[3] =
{ 0 };

#define FirstByte (theEngine == 0)
#define ResetOptions theEngine = 0; theDir = 0;

#define SLAVE_ADDRESS 0x48

char Getter = 0;
char theEngine = 0;
char theDir = 0;

int SetI2CConnection()
{
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

	return 0;
}

void main()
{
	//Stop WDT
	WDT_A_hold(WDT_A_BASE);

	SetI2CConnection();


	while (1)
	{
		//Enter low power mode 0 with interrupts enabled.
		__no_operation();
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
		Getter = USCI_B_I2C_slaveDataGet(USCI_B0_BASE); //double interrupt: first time = first byte;

		switch (Getter)
		{
		case 0xE1:
		{
			StartConversation(FirstEncoder);
			break;
		}
		case 0xE2:
		{
			StartConversation(SecondEncoder);
			break;
		}
		case 0xA1:
		{
			StartConversation(PING);
			break;
		}
		case 0xA2:
		{
			StartConversation(HCSR04);
			break;
		}
		case 0xA3:
		{
			StartConversation(Color);
			break;
		}
		case 0xA4:
		{
			StartConversation(Sharp);
			break;
		}
		case 0xFF:
		{
			__disable_interrupt();
			((void (*)()) 0x1000)();
			break;
		}
		default:
		{
			if (FirstByte)
			{
				theEngine = Getter >> 2; //parsing first byte ( read bytes format)
				theDir = Getter & 3;
			}
			else
			{
				SetEngineMode(theEngine, theDir, Getter);
				ResetOptions
			}
		}
		}

		break;
	}
	case USCI_I2C_UCTXIFG:
	{
		if (StageOfSending == 2)
		{
			USCI_B_I2C_slaveDataPut(USCI_B0_BASE, ResultPart[StageOfSending]);
		}
		if (StageOfSending == 1)
		{
			__delay_cycles(10);

			int Result = GetResult();

			ResultPart[StageOfSending] = Result & 0xFF;
			ResultPart[++StageOfSending] = Result >> 8;

			USCI_B_I2C_slaveDataPut(USCI_B0_BASE,
					ResultPart[StageOfSending - 1]);
		}
		break;

	}
	case USCI_I2C_UCSTPIFG:
	{

		StageOfSending = 1;
		__bic_SR_register_on_exit(LPM0_bits);
		break;
	}
	}
}

