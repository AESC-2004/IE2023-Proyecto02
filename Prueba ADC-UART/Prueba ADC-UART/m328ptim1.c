/*
 * m328ptim1.c
 *
 * Created: 05/05/2025 10:29:16 p. m.
 *  Author: ang50
 */ 

/*********************************************************************************************************************************************/
// m328ptim1.c - Timer 1 library for ATmega328P
/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Libraries
#include <avr/io.h>
#include <stdint.h>
#include "m328ptim1.h"
/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Timers initiation function

void	tim1_init(tim1_channel_t		TIM1_channel,
				  tim1_prescaler_t		TIM1_prescaler,
				  tim1_mode_t			TIM1_waveform_mode,
				  uint16_t				TIM1_TOP_value,
				  tim1_com_t			TIM1_COM_mode,
				  uint16_t				TIM1_TCNT_inital_value,
				  tim1_oc1x_t			TIM1_OC1x_DDR1_ENABLING)
{
	tim1_reset();
	tim1_prescaler(TIM1_prescaler);
	tim1_waveform_mode(TIM1_waveform_mode);
	tim1_compare_output_mode(TIM1_channel, TIM1_COM_mode);
	if (TIM1_OC1x_DDR1_ENABLING == TIM1_OC1x_ENABLE)
	{
		tim1_oc1x_enable(TIM1_channel);
	}
	else if (TIM1_OC1x_DDR1_ENABLING == TIM1_OC1x_DISABLE)
	{
		tim1_oc1x_disable(TIM1_channel);
	}
	tim1_top_value(TIM1_TOP_value);
	tim1_tcnt_value(TIM1_TCNT_inital_value);
}

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Timers-Counter values functions

void	tim1_tcnt_value(uint16_t TIM1_TCNT_value)
{

	TCNT1 = TIM1_TCNT_value;

}

void	tim1_ocr_value(tim1_channel_t TIM1_channel, uint16_t TIM1_OCR_value)
{
	switch (TIM1_channel)
	{
		case TIM1_CHANNEL_A: OCR1A = TIM1_OCR_value; break;
		case TIM1_CHANNEL_B: OCR1B = TIM1_OCR_value; break;
		default: break;
	}

}

void	tim1_top_value(uint16_t TIM1_TOP_value)
{
	uint8_t WGM_bits = 0;
	WGM_bits = (TCCR1B & ((1 << WGM13) | (1 << WGM12)));
	WGM_bits |= (TCCR1A & ((1 << WGM11) | (1 << WGM10)));
	if ((WGM_bits == 0b00010000) || (WGM_bits == 0b00010010) || (WGM_bits == 0b00011000) || (WGM_bits == 0b00011010))
	{
		ICR1 = TIM1_TOP_value;
	}
	else if ((WGM_bits == 0b00001000) || (WGM_bits == 0b00010001) || (WGM_bits == 0b00010011) || (WGM_bits == 0b00011011))
	{
		OCR1A = TIM1_TOP_value;
	}

}

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Timers interrupt enabling functions

void	tim1_ovf_interrupt_enable()
{
	TIMSK1 |= (1 << TOIE1);

}

void	tim1_oc_interrupt_enable(tim1_channel_t TIM1_channel)
{

	switch (TIM1_channel)
	{
		case TIM1_CHANNEL_A: TIMSK1 |= (1 << OCIE1A); break;
		case TIM1_CHANNEL_B: TIMSK1 |= (1 << OCIE1B); break;
		default: break;
	}

}

void	tim1_ovf_interrupt_disable()
{

	TIMSK1 &= ~(1 << TOIE1);

}

void	tim1_oc_interrupt_disable(tim1_channel_t TIM1_channel)
{

	switch (TIM1_channel)
	{
		case TIM1_CHANNEL_A: TIMSK1 &= ~(1 << OCIE1A); break;
		case TIM1_CHANNEL_B: TIMSK1 &= ~(1 << OCIE1B); break;
		default: break;
	}

}

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Timers prescaler function

void	tim1_prescaler(tim1_prescaler_t TIM1_prescaler)
{

	TCCR1B = (TCCR1B & 0xF8) | (TIM1_prescaler);
	
}

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Timers waveform and compare output mode functions

void	tim1_waveform_mode(tim1_mode_t TIM1_waveform_mode)
{
	uint8_t WGM10_bits = 0;
	uint8_t WGM32_bits = 0;
	switch (TIM1_waveform_mode)
	{
		case TIM1_MODE_NORMAL:								WGM32_bits = (0 << WGM13) | (0 << WGM12); WGM10_bits = (0 << WGM11) | (0 << WGM10); break;
		case TIM1_MODE_PHASE_CORRECT_PWM_8b:				WGM32_bits = (0 << WGM13) | (0 << WGM12); WGM10_bits = (0 << WGM11) | (1 << WGM10); break;
		case TIM1_MODE_PHASE_CORRECT_PWM_9b:				WGM32_bits = (0 << WGM13) | (0 << WGM12); WGM10_bits = (1 << WGM11) | (0 << WGM10); break;
		case TIM1_MODE_PHASE_CORRECT_PWM_10b:				WGM32_bits = (0 << WGM13) | (0 << WGM12); WGM10_bits = (1 << WGM11) | (1 << WGM10); break;
		case TIM1_MODE_CTC_OCR1A:							WGM32_bits = (0 << WGM13) | (1 << WGM12); WGM10_bits = (0 << WGM11) | (0 << WGM10); break;
		case TIM1_MODE_FAST_PWM_8b:							WGM32_bits = (0 << WGM13) | (1 << WGM12); WGM10_bits = (0 << WGM11) | (1 << WGM10); break;
		case TIM1_MODE_FAST_PWM_9b:							WGM32_bits = (0 << WGM13) | (1 << WGM12); WGM10_bits = (1 << WGM11) | (0 << WGM10); break;
		case TIM1_MODE_FAST_PWM_10b:						WGM32_bits = (0 << WGM13) | (1 << WGM12); WGM10_bits = (1 << WGM11) | (1 << WGM10); break;
		case TIM1_MODE_PHASE_AND_FREQ_CORRECT_PWM_ICR1:		WGM32_bits = (1 << WGM13) | (0 << WGM12); WGM10_bits = (0 << WGM11) | (0 << WGM10); break;
		case TIM1_MODE_PHASE_AND_FREQ_CORRECT_PWM_OCR1A:	WGM32_bits = (1 << WGM13) | (0 << WGM12); WGM10_bits = (0 << WGM11) | (1 << WGM10); break;
		case TIM1_MODE_PHASE_CORRECT_PWM_ICR1:				WGM32_bits = (1 << WGM13) | (0 << WGM12); WGM10_bits = (1 << WGM11) | (0 << WGM10); break;
		case TIM1_MODE_PHASE_CORRECT_PWM_OCR1A:				WGM32_bits = (1 << WGM13) | (0 << WGM12); WGM10_bits = (1 << WGM11) | (1 << WGM10); break;
		case TIM1_MODE_CTC_ICR1:							WGM32_bits = (1 << WGM13) | (1 << WGM12); WGM10_bits = (0 << WGM11) | (0 << WGM10); break;
		case TIM1_MODE_FAST_PWM_ICR1:						WGM32_bits = (1 << WGM13) | (1 << WGM12); WGM10_bits = (1 << WGM11) | (0 << WGM10); break;
		case TIM1_MODE_FAST_PWM_OCR1A:						WGM32_bits = (1 << WGM13) | (1 << WGM12); WGM10_bits = (1 << WGM11) | (1 << WGM10); break;
		default: break;
	}

	TCCR1A &= ~((1 << WGM11) | (1 << WGM10));
	TCCR1A |= (WGM10_bits);
	TCCR1B &= ~((1 << WGM13) | (1 << WGM12));
	TCCR1B |= (WGM32_bits);

}

void	tim1_compare_output_mode(tim1_channel_t TIM1_channel, tim1_com_t TIM1_COM_mode)
{
	uint8_t COM_bits = 0;
	switch (TIM1_channel)
	{
		case TIM1_CHANNEL_A:
			switch (TIM1_COM_mode)
			{
				case TIM1_COM_OC1x_DISCONNECTED:	COM_bits = (0 << COM1A1) | (0 << COM1A0); break;
				case TIM1_COM_TOGGLE_OC1x:			COM_bits = (0 << COM1A1) | (1 << COM1A0); break;
				case TIM1_COM_NON_INVERT_OC1x:		COM_bits = (1 << COM1A1) | (0 << COM1A0); break;
				case TIM1_COM_INVERT_OC1x:			COM_bits = (1 << COM1A1) | (1 << COM1A0); break;
				default: break;
			}
			
			TCCR1A	&= ~((1 << COM1A1) | (1 << COM1A0)); TCCR1A |= COM_bits;
			break;
		case TIM1_CHANNEL_B:
			switch (TIM1_COM_mode)
			{
				case TIM1_COM_OC1x_DISCONNECTED:	COM_bits = (0 << COM1B1) | (0 << COM1B0); break;
				case TIM1_COM_TOGGLE_OC1x:			COM_bits = (0 << COM1B1) | (1 << COM1B0); break;
				case TIM1_COM_NON_INVERT_OC1x:		COM_bits = (1 << COM1B1) | (0 << COM1B0); break;
				case TIM1_COM_INVERT_OC1x:			COM_bits = (1 << COM1B1) | (1 << COM1B0); break;
				default: break;
			}

			TCCR1A	&= ~((1 << COM1B1) | (1 << COM1B0)); TCCR1A |= COM_bits;
			break;
		default: break;
	}
}

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Timers pin enabling functions

void	tim1_oc1x_enable(tim1_channel_t TIM1_channel)
{

	switch (TIM1_channel)
	{
		case TIM1_CHANNEL_A: DDRB |= (1 << PORTB1); break;
		case TIM1_CHANNEL_B: DDRB |= (1 << PORTB2); break;
		default: break;
	}

}

void	tim1_oc1x_disable(tim1_channel_t TIM1_channel)
{

	switch (TIM1_channel)
	{
		case TIM1_CHANNEL_A: DDRB &= ~(1 << PORTB1); break;
		case TIM1_CHANNEL_B: DDRB &= ~(1 << PORTB2); break;
		default: break;
	}

}

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Timers resetting function

void	tim1_reset()
{

	TCCR1A = 0; TCCR1B = 0; TCCR1C = 0; TCNT1 = 0; OCR1A = 0; OCR1B = 0; ICR1 = 0; TIMSK1 = 0;
	
}

/*********************************************************************************************************************************************/