/*
 * m328padc.h
 *
 * Created: 5/05/2025 17:04:52
 *  Author: ang50
 */ 

/*********************************************************************************************************************************************/
// m328padc.h - ADC library for ATmega328P
/*********************************************************************************************************************************************/

#ifndef M328PADC_H_
#define M328PADC_H_

/*********************************************************************************************************************************************/
// External libraries
#include <avr/io.h>
#include <stdint.h>
/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Enumerations

// ADC_REF enumeration
typedef enum {
	ADC_REF_AREF			= (0 << REFS1) | (0 << REFS0),
	ADC_REF_AVCC			= (0 << REFS1) | (1 << REFS0),
	ADC_REF_INTERNAL_2V56	= (1 << REFS1) | (1 << REFS0)
} adc_ref_t;

// ADC_PRESCALE enumeration
typedef enum {
	ADC_PRESCALE_2		= (0 << ADPS2) | (0 << ADPS1) | (1 << ADPS0),
	ADC_PRESCALE_4		= (0 << ADPS2) | (1 << ADPS1) | (0 << ADPS0),
	ADC_PRESCALE_8		= (0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0),
	ADC_PRESCALE_16		= (1 << ADPS2) | (0 << ADPS1) | (0 << ADPS0),
	ADC_PRESCALE_32		= (1 << ADPS2) | (0 << ADPS1) | (1 << ADPS0),
	ADC_PRESCALE_64		= (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0),
	ADC_PRESCALE_128	= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)
} adc_prescaler_t;

// ADC_TRIGGER enumeration
typedef enum {
	ADC_TRIGGER_FREE_RUNNING			= (0 << ADTS2) | (0 << ADTS1) | (0 << ADTS0),
	ADC_TRIGGER_ANALOG_COMPARATOR		= (0 << ADTS2) | (0 << ADTS1) | (1 << ADTS0),
	ADC_TRIGGER_EXT_INT0				= (0 << ADTS2) | (1 << ADTS1) | (0 << ADTS0),
	ADC_TRIGGER_TIMER0_COMPARE_MATCH_A	= (0 << ADTS2) | (1 << ADTS1) | (1 << ADTS0),
	ADC_TRIGGER_TIMER0_OVERFLOW			= (1 << ADTS2) | (0 << ADTS1) | (0 << ADTS0),
	ADC_TRIGGER_TIMER1_COMPARE_MATCH_B	= (1 << ADTS2) | (0 << ADTS1) | (1 << ADTS0),
	ADC_TRIGGER_TIMER1_OVERFLOW			= (1 << ADTS2) | (1 << ADTS1) | (0 << ADTS0),
	ADC_TRIGGER_TIMER1_CAPTURE_EVENT	= (1 << ADTS2) | (1 << ADTS1) | (1 << ADTS0),
} adc_trigger_t;

// ADC_CHANNEL enumeration
typedef enum {
	ADC_CHANNEL_ADC0			= (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0),
	ADC_CHANNEL_ADC1			= (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (1 << MUX0),
	ADC_CHANNEL_ADC2			= (0 << MUX3) | (0 << MUX2) | (1 << MUX1) | (0 << MUX0),
	ADC_CHANNEL_ADC3			= (0 << MUX3) | (0 << MUX2) | (1 << MUX1) | (1 << MUX0),
	ADC_CHANNEL_ADC4			= (0 << MUX3) | (1 << MUX2) | (0 << MUX1) | (0 << MUX0),
	ADC_CHANNEL_ADC5			= (0 << MUX3) | (1 << MUX2) | (0 << MUX1) | (1 << MUX0),
	ADC_CHANNEL_ADC6			= (0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0),
	ADC_CHANNEL_ADC7			= (0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0),
	ADC_CHANNEL_ADC8			= (1 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0),
	ADC_CHANNEL_1V1				= (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0),
	ADC_CHANNEL_GND				= (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0),
} adc_channel_t;

// ADC_AUTO_TRIGGER_ENABLE enumeration
typedef enum {
	ADC_AUTO_TRIGGER_ENABLE		= (1 << ADATE),
	ADC_AUTO_TRIGGER_DISABLE	= (0 << ADATE)
} adc_auto_trigger_enable_t;

// ADC_INTERRUPT_ENABLE enumeration
typedef enum {
	ADC_INTERRUPT_ENABLE		= (1 << ADIE),
	ADC_INTERRUPT_DISABLE		= (0 << ADIE)
} adc_interrupt_enable_t;

// ADC_ADJUST enumeration
typedef enum {
	ADC_LEFT_ADJUST				= (1 << ADLAR),
	ADC_RIGHT_ADJUST			= (0 << ADLAR)
} adc_adjust_t;

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Function prototypes

// Initializing the ADC
void	adc_init(adc_ref_t					ADC_reference,
				 adc_prescaler_t			ADC_prescaler,
				 adc_adjust_t				ADC_adjust,
				 adc_channel_t				ADC_channel,
				 adc_interrupt_enable_t		ADC_interrupt_enable,
				 adc_auto_trigger_enable_t  ADC_auto_trigger_enable,
				 adc_trigger_t				ADC_trigger_source);

// ADC enabling				(Without changing other settings)
void	adc_enable();
void	adc_disable();

// ADC start conversion		(Without changing other settings)
void	adc_start_conversion();

// Channel selection		(Without changing other settings)
void	adc_channel(adc_channel_t ADC_channel);

// Reference source			(Without changing other settings)
void	adc_ref(adc_ref_t ADC_reference);

// Prescaler selection		(Without changing other settings)
void	adc_prescaler(adc_prescaler_t ADC_prescaler);

// Auto trigger enabling		(Without changing other settings)
void	adc_auto_trigger_enable();
void	adc_auto_trigger_disable();

// Auto trigger source		(Without changing other settings)
void	adc_auto_trigger_source(adc_trigger_t ADC_trigger_source);

// Interrupt enabling		(Without changing other settings)
void	adc_interrupt_enable();
void	adc_interrupt_disable();

// Adjust mode (L or R)		(Without changing other settings)
void	adc_adjust(adc_adjust_t ADC_adjust);

/*********************************************************************************************************************************************/

#endif /* M328PADC_H_ */