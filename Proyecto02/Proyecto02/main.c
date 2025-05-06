/*
 * Proyecto02.c
 *
 * Created: 30/04/2025 07:25:47 p. m.
 * Author : ang50
 */ 

/*********************************************************************************************************************************************/
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "m328padc.h"
#include "m328pusart.h"
#include "m328ptims8b.h"
#include "m328ptim1.h"

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Variables y constantes


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
		OCR1A = 250 + ((uint32_t)SERVO1_READ * 1000) / 1023;
		OCR3A = 250 + ((uint32_t)SERVO2_READ * 1000) / 1023;
		OCR0A = LED_READ;
	}
}

/*********************************************************************************************************************************************/
// NON-Interrupt subroutines
void SETUP()
{
	// Desactivamos interrupciones
	cli();
	// Desactivamos UART1
	UCSR1B			= 0x00;
	// Ajustamos el Prescaler global para F_CPU = 1MHz
	CLKPR			= (1 << CLKPCE);
	CLKPR			= (0 << CLKPCE) | (0 << CLKPS3) | (1 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0);
	// Algunas funciones de inicio
	tim_16b_init(TIM_16b_NUM_1, TIM_16b_CHANNEL_A, TIM_16b_PRESCALE_1, TIM_16b_MODE_PHASE_CORRECT_PWM_ICRn, 10000, TIM_16b_COM_NON_INVERT_OCnx, 0, TIM_16b_OCnx_ENABLE);
	tim_16b_init(TIM_16b_NUM_3, TIM_16b_CHANNEL_A, TIM_16b_PRESCALE_1, TIM_16b_MODE_PHASE_CORRECT_PWM_ICRn, 10000, TIM_16b_COM_NON_INVERT_OCnx, 0, TIM_16b_OCnx_ENABLE);
	tim0_init(TIM0_CHANNEL_A, TIM0_PRESCALE_1, TIM0_MODE_FAST_PWM_0xFF, 0, TIM0_COM_NON_INVERT_OC0x, 0, TIM0_OC0x_ENABLE);
	ADCinit();
	ADCSRA |= (1 << ADSC);
	sei();
}

void ADCinit()
{
	// NO_Auto_Trigger, PS_8, ADC6 (Initial), AVCC_REF, ADC_INT_EN
	ADMUX		= (0 << REFS1) | (1 << REFS0)| (0 << ADLAR) | (0 << MUX4) | (0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0);
	ADCSRB		= (0 << MUX5) | (0 << ADTS3) | (0 << ADTS2) | (0 << ADTS1) | (0 << ADTS0);
	ADCSRA		= (1 << ADEN) | (0 << ADSC)| (0 << ADATE) | (0 << ADIF) | (1 << ADIE) | (0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}
/*********************************************************************************************************************************************/
// Interrupt routines

ISR(ADC_vect)
{
	// MUX needed
	if (ADC_MUX == 1)		// SERVO1 &	ADC6	------>		SERVO2 & ADC4
	{
		SERVO1_READ		= ADC;
		ADC_MUX			= 2;
		// ADC update to ADC4:
		ADMUX		=  (0 << REFS1) | (1 << REFS0)| (0 << ADLAR) | (0 << MUX4) | (0 << MUX3) | (1 << MUX2) | (0 << MUX1) | (0 << MUX0);
		ADCSRA |= (1 << ADSC);
	}
	else if (ADC_MUX == 2)	// SERVO2 &	ADC4	------>		LED & ADC5
	{
		SERVO2_READ		= ADC;
		ADC_MUX			= 3;
		// ADC update to ADC5 & ***LEFT ADJUST!!!***:
		ADMUX		=  (0 << REFS1) | (1 << REFS0)| (1 << ADLAR) | (0 << MUX4) | (0 << MUX3) | (1 << MUX2) | (0 << MUX1) | (1 << MUX0);
		ADCSRA |= (1 << ADSC);
	}
	else if (ADC_MUX == 3)	// LED & ADC5		------>		SERVO1 & ADC6
	{
		LED_READ		= ADCH;
		ADC_MUX			= 1;
		// ADC update to ADC6 & ***RETURN TO RIGHT ADJUST!!!***:
		ADMUX		=  (0 << REFS1) | (1 << REFS0)| (0 << ADLAR) | (0 << MUX4) | (0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0);
		ADCSRA |= (1 << ADSC);
	}
}
