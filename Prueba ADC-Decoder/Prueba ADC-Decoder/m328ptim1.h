/*
 * m328ptim1.h
 *
 * Created: 05/05/2025 10:29:04 p. m.
 *  Author: ang50
 */ 

/*********************************************************************************************************************************************/
// m328ptim1.h - Timer 1 library for ATmega328P
/*********************************************************************************************************************************************/


#ifndef M328PTIM1_H_
#define M328PTIM1_H_

/*********************************************************************************************************************************************/
// External libraries
#include <avr/io.h>
#include <stdint.h>
/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Enumerations

// TIM1_CHANNEL enumeration
typedef enum {
	TIM1_CHANNEL_A,
	TIM1_CHANNEL_B
} tim1_channel_t;

// TIM1_MODE enumeration
typedef enum {
	TIM1_MODE_NORMAL,
	TIM1_MODE_PHASE_CORRECT_PWM_8b,
	TIM1_MODE_PHASE_CORRECT_PWM_9b,
	TIM1_MODE_PHASE_CORRECT_PWM_10b,
	TIM1_MODE_CTC_OCR1A,
	TIM1_MODE_FAST_PWM_8b,
	TIM1_MODE_FAST_PWM_9b,
	TIM1_MODE_FAST_PWM_10b,
	TIM1_MODE_PHASE_AND_FREQ_CORRECT_PWM_ICR1,
	TIM1_MODE_PHASE_AND_FREQ_CORRECT_PWM_OCR1A,
	TIM1_MODE_PHASE_CORRECT_PWM_ICR1,
	TIM1_MODE_PHASE_CORRECT_PWM_OCR1A,
	TIM1_MODE_CTC_ICR1,
	TIM1_MODE_FAST_PWM_ICR1,
	TIM1_MODE_FAST_PWM_OCR1A
} tim1_mode_t;

// TIM1_PRESCALER enumeration
typedef enum {
	TIM1_NO_CLOCK_SOURCE							= (0 << CS12) | (0 << CS11) | (0 << CS10),
	TIM1_PRESCALE_1									= (0 << CS12) | (0 << CS11) | (1 << CS10),
	TIM1_PRESCALE_8									= (0 << CS12) | (1 << CS11) | (0 << CS10),
	TIM1_PRESCALE_64								= (0 << CS12) | (1 << CS11) | (1 << CS10),
	TIM1_PRESCALE_256								= (1 << CS12) | (0 << CS11) | (0 << CS10),
	TIM1_PRESCALE_1024								= (1 << CS12) | (0 << CS11) | (1 << CS10),
	TIM1_EXT_SOURCE_FALLING_EDGE					= (1 << CS12) | (1 << CS11) | (0 << CS10),
	TIM1_EXT_SOURCE_RISING_EDGE						= (1 << CS12) | (1 << CS11) | (1 << CS10),
} tim1_prescaler_t;

// TIM1_COM enumeration
typedef enum {
	TIM1_COM_OC1x_DISCONNECTED,
	TIM1_COM_TOGGLE_OC1x,
	TIM1_COM_NON_INVERT_OC1x,
	TIM1_COM_INVERT_OC1x
} tim1_com_t;

// TIM1_OC1x_ENABLING enumeration
typedef enum {
	TIM1_OC1x_ENABLE,
	TIM1_OC1x_DISABLE
} tim1_oc1x_t;
/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Function prototypes

// Initializing a timer
// TOP_VALUE shall only be copied to the correct register when utile to the waveform selected.
void	tim1_init(tim1_channel_t	TIM1_channel,
				  tim1_prescaler_t	TIM1_prescaler,
				  tim1_mode_t		TIM1_waveform_mode,
				  uint16_t			TIM1_TOP_value,
				  tim1_com_t		TIM1_COM_mode,
				  uint16_t			TIM1_TCNT_inital_value,
				  tim1_oc1x_t		TIM1_OC1x_DDR1_ENABLING);

// TCNT Value					(Without changing other settings)
void	tim1_tcnt_value(uint16_t TIM1_TCNT_value);

// OCR	Value					(Without changing other settings)
void	tim1_ocr_value(tim1_channel_t TIM1_channel, uint16_t TIM1_OCR_value);

// TOP Value					(Without changing other settings)
// This function shall look for the WGM bits to see which register to change depending on the waveform mode established!
// If no register controls the TOP value, no action should be done!
void	tim1_top_value(uint16_t TIM1_TOP_value);

// Interrupt enabling			(Without changing other settings)
void	tim1_ovf_interrupt_enable();								// Overflow
void	tim1_ic_interrupt_enable();									// Input capture
void	tim1_oc_interrupt_enable(tim1_channel_t TIM1_channel);		// Output compare
void	tim1_ovf_interrupt_disable();								// Overflow
void	tim1_ic_interrupt_disable();								// Input capture
void	tim1_oc_interrupt_disable(tim1_channel_t TIM1_channel);		// Output compare

// Prescaler selection			(Without changing other settings)
void	tim1_prescaler(tim1_prescaler_t TIM1_prescaler);

// Waveform mode				(Without changing other settings)
void	tim1_waveform_mode(tim1_mode_t TIM1_waveform_mode);

// COM Mode						(Without changing other settings)
void	tim1_compare_output_mode(tim1_channel_t TIM1_channel, tim1_com_t TIM1_COM_mode);

// OC1x enabling				(Without changing other settings)
void	tim1_oc1x_enable(tim1_channel_t TIM1_channel);

// OC1x disabling				(Without changing other settings)
void	tim1_oc1x_disable(tim1_channel_t TIM1_channel);

// TIM1 reseting				(Without changing other settings)
void tim1_reset();

/*********************************************************************************************************************************************/

#endif /* M328PTIM1_H_ */