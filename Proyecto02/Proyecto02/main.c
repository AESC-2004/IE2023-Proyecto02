/*
 * Proyecto02.c
 *
 * Created: 30/04/2025 07:25:47 p. m.
 * Author : ang50
 */ 

/*********************************************************************************************************************************************/
// Libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "m328padc.h"
#include "m328pusart.h"
#include "m328ptims8b.h"
#include "m328ptim1.h"

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Variables and constants

uint8_t Motors_MUX	= 0;
uint8_t TIM0_Millis	= 0;

/*********************************************************************************************************************************************/
// Function prototypes
void SETUP();

/*********************************************************************************************************************************************/
// Main Function
int main(void)
{
	SETUP();
	
	while (1)
	{
	}
}

/*********************************************************************************************************************************************/
// NON-Interrupt subroutines
void SETUP()
{
	// Deactivating interrupts
	cli();
	
	// Reducing global CLK to 1MHZ
	CLKPR	|= (1 << CLKPCE);
	CLKPR	= (0 << CLKPCE) | (0 << CLKPS3) | (1 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0);
	
	// Enabling PB0 as output for them motors
	DDRB	|= (1 << DDB0);
	
	// Enabling PC0,1 as outputs for the motors MUX
	DDRC	|= (1 << DDC0) | (1 << DDC1);
	
	// Establishing a PWM with TIM1 
	tim1_init(TIM1_CHANNEL_A, TIM1_PRESCALE_8, TIM1_MODE_CTC_OCR1A, 0, TIM1_COM_OC1x_DISCONNECTED, 0, TIM1_OC1x_DISABLE);
	//tim1_oc_interrupt_enable(TIM1_CHANNEL_A);
	
	// Initiating TIM0 for MUX standards
	tim0_init(TIM_8b_CHANNEL_A, TIM0_PRESCALE_8, TIM_8b_MODE_CTC_OCRA, 250, TIM_8b_COM_OCnx_DISCONNECTED, 0, TIM_8b_OCnx_DISABLE);
	tim_8b_oc_interrupt_enable(TIM_8b_NUM_0, TIM_8b_CHANNEL_A);
	
	// Activating interrupts
	sei();
}

/*********************************************************************************************************************************************/
// Interrupt routines

ISR(TIMER0_COMPA_vect)
{
	TIM0_Millis++;
	if (TIM0_Millis == 10) TIM0_Millis = 0;
	if (Motors_MUX == 0 && TIM0_Millis == 0)
	{
		PORTC	&= ~(1 << PORTC0);
		tim1_ocr_value(TIM1_CHANNEL_A, 225);
		tim1_tcnt_value(0);
		tim1_oc_interrupt_enable(TIM1_CHANNEL_A);
		PORTC	|= (1 << PORTC1);
		PORTB	|= (1 << PORTB0);
		Motors_MUX	= 1;
		
	} else if (Motors_MUX == 1 && TIM0_Millis == 1)
	{
		PORTC	&= ~(1 << PORTC1);
		tim1_ocr_value(TIM1_CHANNEL_A, 50);
		tim1_tcnt_value(0);
		tim1_oc_interrupt_enable(TIM1_CHANNEL_A);
		PORTC	|= (1 << PORTC0);
		PORTB	|= (1 << PORTB0);
		Motors_MUX	= 0;
	}
}

ISR(TIMER1_COMPA_vect)
{
	
	PORTB	&= ~(1 << PORTB0);
	//tim1_oc_interrupt_disable(TIM1_CHANNEL_A);
	
}
