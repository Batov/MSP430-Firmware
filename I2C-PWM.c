//Nikita Batov
//Use TI driverlib
//msp430f5510 - i2c on p3.0 & p3.1

//Bytes format:
//0x`xx`yy` - HEX(2 bytes)
//xx:
//use the first 4 bits
//first and second bytes set mode of engine
//00 - Neutral
//01 - Back
//10 - Forward
//11 - Block (Not for all engines)
//yy:
//Second bytes set power of engine
//0x64 = 100%
//0xa = 100%
//(there is uncertainty)
// e.g. : 0x4405 = First Back 68%

#include "inc/hw_memmap.h"
#include "usci_b_i2c.h"
#include "wdt_a.h"
#include "gpio.h"
#include "PowerEng.h"

#define FirstByte (Engine == 0)
#define ResetOptions {Engine = 0; Dir = 0;}
#define SLAVE_ADDRESS 0x48

char GoToBSL = 0;
char getter = 0;
char Engine = 0;
char Dir = 0;

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
		__bis_SR_register(LPM0_bits + GIE);
		__no_operation();

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
			EnablePower(); // check hardware protection and activate engines
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

