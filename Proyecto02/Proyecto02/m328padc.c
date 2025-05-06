/*
 * m328padc.c
 *
 * Created: 5/05/2025 17:16:30
 *  Author: ang50
 */ 

/*********************************************************************************************************************************************/
// m328padc.c - ADC library for ATmega328P
/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Libraries
#include <avr/io.h>
#include <stdint.h>
#include "m328padc.h"
/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// ADC initiating function

void	adc_init(adc_ref_t				ADC_reference,
			  adc_prescaler_t			ADC_prescaler,
			  adc_adjust_t				ADC_adjust,
			  adc_channel_t				ADC_channel,
			  adc_interrupt_enable_t	ADC_interrupt_enable,
			  adc_auto_trigger_enable_t ADC_auto_trigger_enable,
			  adc_trigger_t				ADC_trigger_source)
{
	// Set ADMUX, ADCSRA and ADCSRB initial values
	ADMUX	= 0; ADCSRA	= 0; ADCSRB	= 0;
	
	adc_ref(ADC_reference);
	adc_adjust(ADC_adjust);
	adc_channel(ADC_channel);
	adc_prescaler(ADC_prescaler);
	switch (ADC_auto_trigger_enable)
	{
		case ADC_AUTO_TRIGGER_ENABLE:	adc_auto_trigger_enable();	break;
		case ADC_AUTO_TRIGGER_DISABLE:	adc_auto_trigger_disable(); break;
		default: break;
	}
	adc_auto_trigger_source(ADC_trigger_source);
	switch (ADC_interrupt_enable)
	{
		case ADC_INTERRUPT_ENABLE:		adc_interrupt_enable();		break;
		case ADC_INTERRUPT_DISABLE:		adc_interrupt_disable();	break;
		default: break;
	}
	// Enable the ADC (Purposely as last step)
	ADCSRA	|=	(1 << ADEN);
}

void	adc_start_conversion()
{
	ADCSRA	|=	(1 << ADSC);
}
/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// ADC enabling functions

void	adc_enable() 
{
	ADCSRA	|=	(1 << ADEN);
}

void	adc_disable() 
{
	ADCSRA	&=	~(1 << ADEN);
}

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// ADC channel and reference functions

void	adc_channel(adc_channel_t ADC_channel) 
{
	ADMUX	= (ADMUX & 0xF0) | (ADC_channel);
}

void	adc_ref(adc_ref_t ADC_reference) 
{
	ADMUX	=	(ADMUX & 0x3F) | (ADC_reference); 
}

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// ADC prescaler function

void	adc_prescaler(adc_prescaler_t ADC_prescaler) 
{
	ADCSRA	=	(ADCSRA & 0xF8) | (ADC_prescaler);
}

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// ADC auto-trigger functions

void	adc_auto_trigger_enable() 
{
	ADCSRA	|=	(1 << ADATE);
}

void	adc_auto_trigger_disable() 
{
	ADCSRA	&=	~(1 << ADATE);
}

void	adc_auto_trigger_source(adc_trigger_t ADC_trigger_source) 
{
	ADCSRB	=	(ADCSRB & 0xF0) | (ADC_trigger_source);
}

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Interrupt enabling functions

void	adc_interrupt_enable() 
{
	ADCSRA	|=	(1 << ADIE);
}

void	adc_interrupt_disable() 
{
	ADCSRA	&=	~(1 << ADIE);
}

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// ADC justification function

void	adc_adjust(adc_adjust_t ADC_adjust) 
{
	ADMUX	=	(ADMUX & ~(1 << ADLAR)) | ADC_adjust;
}

/*********************************************************************************************************************************************/

