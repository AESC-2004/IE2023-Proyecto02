/*
 * m328ptims8b.h
 *
 * Created: 30/04/2025 07:28:09 p. m.
 *  Author: ang50
 */ 


#ifndef M328PTIMS8B_H_
#define M328PTIMS8B_H_

/*********************************************************************************************************************************************/
// External libraries
#include <avr/io.h>
#include <stdint.h>
/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Enumerations

// TIM_8b_NUM enumeration
typedef enum {
	TIM_8b_NUM_0,
	TIM_8b_NUM_2
} tim_8b_num_t;

// TIM_8b_CHANNEL enumeration
typedef enum {
	TIM_8b_CHANNEL_A,
	TIM_8b_CHANNEL_B,
} tim_8b_channel_t;

// TIM_8b_MODE enumeration
typedef enum {
	TIM_8b_MODE_NORMAL,
	TIM_8b_MODE_PHASE_CORRECT_PWM_0xFF,
	TIM_8b_MODE_CTC_OCRA,
	TIM_8b_MODE_FAST_PWM_0xFF,
	TIM_8b_MODE_PHASE_CORRECT_PWM_OCRA,
	TIM_8b_MODE_FAST_PWM_OCRnA
} tim_8b_mode_t;

// TIM_8b_PRESCALER enumeration
typedef enum {
	TIM_8b_NO_CLOCK_SOURCE,
	TIM_8b_PRESCALE_1,
	TIM_8b_PRESCALE_8,
	TIM_8b_PRESCALE_64,
	TIM_8b_PRESCALE_256,
	TIM_8b_PRESCALE_1024
} tim_8b_prescaler_t;

// TIM0_PRESCALER enumeration
typedef enum {
	TIM0_NO_CLOCK_SOURCE							= (0 << CS02) | (0 << CS01) | (0 << CS00),
	TIM0_PRESCALE_1									= (0 << CS02) | (0 << CS01) | (1 << CS00),
	TIM0_PRESCALE_8									= (0 << CS02) | (1 << CS01) | (0 << CS00),
	TIM0_PRESCALE_64								= (0 << CS02) | (1 << CS01) | (1 << CS00),
	TIM0_PRESCALE_256								= (1 << CS02) | (0 << CS01) | (0 << CS00),
	TIM0_PRESCALE_1024								= (1 << CS02) | (0 << CS01) | (1 << CS00),
	TIM0_EXT_SOURCE_FALLING_EDGE					= (1 << CS02) | (1 << CS01) | (0 << CS00),
	TIM0_EXT_SOURCE_RISING_EDGE						= (1 << CS02) | (1 << CS01) | (1 << CS00)
} tim0_prescaler_t;

// TIM2_PRESCALER enumeration
typedef enum {
	TIM2_NO_CLOCK_SOURCE							= (0 << CS22) | (0 << CS21) | (0 << CS20),
	TIM2_PRESCALE_1									= (0 << CS22) | (0 << CS21) | (1 << CS20),
	TIM2_PRESCALE_8									= (0 << CS22) | (1 << CS21) | (0 << CS20),
	TIM2_PRESCALE_32								= (0 << CS22) | (1 << CS21) | (1 << CS20),
	TIM2_PRESCALE_64								= (1 << CS22) | (0 << CS21) | (0 << CS20),
	TIM2_PRESCALE_128								= (1 << CS22) | (0 << CS21) | (1 << CS20),
	TIM2_PRESCALE_256								= (1 << CS22) | (1 << CS21) | (0 << CS20),
	TIM2_PRESCALE_1024								= (1 << CS22) | (1 << CS21) | (1 << CS20)
} tim2_prescaler_t;

// TIM_8b_COM enumeration
typedef enum {
	TIM_8b_COM_OCnx_DISCONNECTED,
	TIM_8b_COM_TOGGLE_OCnx,
	TIM_8b_COM_NON_INVERT_OCnx,
	TIM_8b_COM_INVERT_OCnx
} tim_8b_com_t;

// TIM_8b_OCnx_ENABLING enumeration
typedef enum {
	TIM_8b_OCnx_ENABLE,
	TIM_8b_OCnx_DISABLE
} tim_8b_ocnx_t;
/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Function prototypes

// Initializing TIM_8b
void	tim_8b_init(tim_8b_num_t TIM_number,
tim_8b_channel_t TIM_channel,
tim_8b_prescaler_t TIM_prescaler,
tim_8b_mode_t TIM_waveform_mode,
uint8_t TIM_OCRA_value,
tim_8b_com_t TIM_COM_mode,
uint8_t TIM_TCNT_inital_value,
tim_8b_ocnx_t TIM_OCnx_DDRn_ENABLING
);

// Initializing TIM0
void	tim0_init(tim_8b_channel_t TIM_channel,
tim0_prescaler_t TIM0_prescaler,
tim_8b_mode_t TIM_waveform_mode,
uint8_t TIM_OCRA_value,
tim_8b_com_t TIM_COM_mode,
uint8_t TIM_TCNT_inital_value,
tim_8b_ocnx_t TIM_OCnx_DDRn_ENABLING
);

// Initializing TIM2
void	tim2_init(tim_8b_channel_t TIM_channel,
tim2_prescaler_t TIM2_prescaler,
tim_8b_mode_t TIM_waveform_mode,
uint8_t TIM_OCRA_value,
tim_8b_com_t TIM_COM_mode,
uint8_t TIM_TCNT_inital_value,
tim_8b_ocnx_t TIM_OCnx_DDRn_ENABLING
);

// TCNT Value					(Without changing other settings)
void	tim_8b_tcnt_value(tim_8b_num_t TIM_number, uint8_t TIM_TCNT_value);

// OCR	Value					(Without changing other settings)
void	tim_8b_ocr_value(tim_8b_num_t TIM_number, tim_8b_channel_t TIM_channel, uint8_t TIM_OCR_value);

// Interrupt enabling			(Without changing other settings)
void	tim_8b_ovf_interrupt_enable(tim_8b_num_t TIM_number);									// Overflow								
void	tim_8b_oc_interrupt_enable(tim_8b_num_t TIM_number, tim_8b_channel_t TIM_channel);		// Output compare

// Interrupt disabling			(Without changing other settings)
void	tim_8b_ovf_interrupt_disable(tim_8b_num_t TIM_number);									// Overflow								
void	tim_8b_oc_interrupt_disable(tim_8b_num_t TIM_number, tim_8b_channel_t TIM_channel);		// Output compare

// Prescaler selection			(Without changing other settings)
void	tim_8b_prescaler(tim_8b_num_t TIM_number, tim_8b_prescaler_t TIM_prescaler);
void	tim0_prescaler(tim0_prescaler_t TIM0_prescaler);
void	tim2_prescaler(tim2_prescaler_t TIM2_prescaler);

// Waveform mode				(Without changing other settings)
void	tim_8b_waveform_mode(tim_8b_num_t TIM_number, tim_8b_mode_t TIM_waveform_mode);

// COM Mode						(Without changing other settings)
void	tim_8b_compare_output_mode(tim_8b_num_t TIM_number, tim_8b_channel_t TIM_channel, tim_8b_com_t TIM_COM_mode);

// OCnx enabling				(Without changing other settings)
void	tim_8b_ocnx_enable(tim_8b_num_t TIM_number, tim_8b_channel_t TIM_channel);

// OCnx disabling				(Without changing other settings)
void	tim_8b_ocnx_disable(tim_8b_num_t TIM_number, tim_8b_channel_t TIM_channel);

// TIM reseting					(Without changing other settings)
void tim_8b_reset(tim_8b_num_t TIM_number);



#endif /* M328PTIMS8B_H_ */