/*
 * m328ptims8b.c
 *
 * Created: 30/04/2025 07:28:30 p. m.
 *  Author: ang50
 */ 

/*********************************************************************************************************************************************/
// Libraries

#include <avr/io.h>
#include <stdint.h>
#include "m32u4tims16b.h"
/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
//Functions

void tim_8b_init(tim_8b_num_t TIM_number,
tim_8b_channel_t TIM_channel,
tim_8b_prescaler_t TIM_prescaler,
tim_8b_mode_t TIM_waveform_mode,
uint8_t TIM_OCRA_value,
tim_8b_com_t TIM_COM_mode,
uint8_t TIM_TCNT_inital_value,
tim_8b_ocnx_t TIM_OCnx_DDRn_ENABLING)
{
	tim_8b_reset(TIM_number);
	tim_8b_prescaler(TIM_number, TIM_prescaler);
	tim_8b_waveform_mode(TIM_number, TIM_waveform_mode);
	tim_8b_compare_output_mode(TIM_number, TIM_channel, TIM_COM_mode);
	if (TIM_OCnx_DDRn_ENABLING == TIM_8b_OCnx_ENABLE)
	{
		tim_8b_ocnx_enable(TIM_number, TIM_channel);
	}
	else if (TIM_OCnx_DDRn_ENABLING == TIM_8b_OCnx_DISABLE)
	{
		tim_8b_ocnx_disable(TIM_number, TIM_channel);
	}
	tim_8b_ocr_value(TIM_number, TIM_8b_CHANNEL_A, TIM_OCRA_value);
	tim_8b_tcnt_value(TIM_number, TIM_TCNT_inital_value);
}

void tim0_init(tim_8b_channel_t TIM_channel,
tim0_prescaler_t TIM0_prescaler,
tim_8b_mode_t TIM_waveform_mode,
uint8_t TIM_OCRA_value,
tim_8b_com_t TIM_COM_mode,
uint8_t TIM_TCNT_inital_value,
tim_8b_ocnx_t TIM_OCnx_DDRn_ENABLING)
{
	tim_8b_reset(TIM_8b_NUM_0);
	tim0_prescaler(TIM0_prescaler);
	tim_8b_waveform_mode(TIM_8b_NUM_0, TIM_waveform_mode);
	tim_8b_compare_output_mode(TIM_8b_NUM_0, TIM_channel, TIM_COM_mode);
	if (TIM_OCnx_DDRn_ENABLING == TIM_8b_OCnx_ENABLE)
	{
		tim_8b_ocnx_enable(TIM_8b_NUM_0, TIM_channel);
	}
	else if (TIM_OCnx_DDRn_ENABLING == TIM_8b_OCnx_DISABLE)
	{
		tim_8b_ocnx_disable(TIM_8b_NUM_0, TIM_channel);
	}
	tim_8b_ocr_value(TIM_8b_NUM_0, TIM_8b_CHANNEL_A, TIM_OCRA_value);
	tim_8b_tcnt_value(TIM_8b_NUM_0, TIM_TCNT_inital_value);
}

void tim2_init(tim_8b_channel_t TIM_channel,
tim2_prescaler_t TIM2_prescaler,
tim_8b_mode_t TIM_waveform_mode,
uint8_t TIM_OCRA_value,
tim_8b_com_t TIM_COM_mode,
uint8_t TIM_TCNT_inital_value,
tim_8b_ocnx_t TIM_OCnx_DDRn_ENABLING)
{
	tim_8b_reset(TIM_8b_NUM_2);
	tim2_prescaler(TIM2_prescaler);
	tim_8b_waveform_mode(TIM_8b_NUM_2, TIM_waveform_mode);
	tim_8b_compare_output_mode(TIM_8b_NUM_2, TIM_channel, TIM_COM_mode);
	if (TIM_OCnx_DDRn_ENABLING == TIM_8b_OCnx_ENABLE)
	{
		tim_8b_ocnx_enable(TIM_8b_NUM_2, TIM_channel);
	}
	else if (TIM_OCnx_DDRn_ENABLING == TIM_8b_OCnx_DISABLE)
	{
		tim_8b_ocnx_disable(TIM_8b_NUM_2, TIM_channel);
	}
	tim_8b_ocr_value(TIM_8b_NUM_2, TIM_8b_CHANNEL_A, TIM_OCRA_value);
	tim_8b_tcnt_value(TIM_8b_NUM_2, TIM_TCNT_inital_value);
}

void tim_8b_tcnt_value(tim_8b_num_t TIM_number, uint8_t TIM_TCNT_value)
{
	switch (TIM_number)
	{
		case TIM_8b_NUM_0: TCNT0 = TIM_TCNT_value; break;
		case TIM_8b_NUM_2: TCNT2 = TIM_TCNT_value; break;
		default: break;
	}
}

void tim_8b_ocr_value(tim_8b_num_t TIM_number, tim_8b_channel_t TIM_channel, uint8_t TIM_OCR_value)
{
	switch (TIM_number)
	{
		case TIM_8b_NUM_0:
		switch (TIM_channel)
		{
			case TIM_8b_CHANNEL_A: OCR0A = TIM_OCR_value; break;
			case TIM_8b_CHANNEL_B: OCR0B = TIM_OCR_value; break;
			default: break;
		}
		break;
		case TIM_8b_NUM_2:
		switch (TIM_channel)
		{
			case TIM_8b_CHANNEL_A: OCR2A = TIM_OCR_value; break;
			case TIM_8b_CHANNEL_B: OCR2B = TIM_OCR_value; break;
			default: break;
		}
		break;
	}
}

void tim_8b_ovf_interrupt_enable(tim_8b_num_t TIM_number) {
	switch (TIM_number) {
		case TIM_8b_NUM_0: TIMSK0 |= (1 << TOIE0); break;
		case TIM_8b_NUM_2: TIMSK2 |= (1 << TOIE2); break;
		default: break;
	}
}

void tim_8b_oc_interrupt_enable(tim_8b_num_t TIM_number, tim_8b_channel_t TIM_channel) {
	switch (TIM_number) {
		case TIM_8b_NUM_0:
		switch (TIM_channel)
		{
			case TIM_8b_CHANNEL_A: TIMSK0 |= (1 << OCIE0A); break;
			case TIM_8b_CHANNEL_B: TIMSK0 |= (1 << OCIE0B); break;
			default: break;
		}
		break;
		case TIM_8b_NUM_2:
		switch (TIM_channel)
		{
			case TIM_8b_CHANNEL_A: TIMSK2 |= (1 << OCIE2A); break;
			case TIM_8b_CHANNEL_B: TIMSK2 |= (1 << OCIE2B); break;
			default: break;
		}
		break;
		default: break;
	}
}

void tim_8b_ovf_interrupt_disable(tim_8b_num_t TIM_number) {
	switch (TIM_number) {
		case TIM_8b_NUM_0: TIMSK0 &= ~(1 << TOIE0); break;
		case TIM_8b_NUM_2: TIMSK2 &= ~(1 << TOIE2); break;
		default: break;
	}
}

void tim_8b_oc_interrupt_disable(tim_8b_num_t TIM_number, tim_8b_channel_t TIM_channel) {
	switch (TIM_number) {
		case TIM_8b_NUM_0:
		switch (TIM_channel)
		{
			case TIM_8b_CHANNEL_A: TIMSK0 &= ~(1 << OCIE0A); break;
			case TIM_8b_CHANNEL_B: TIMSK0 &= ~(1 << OCIE0B); break;
			default: break;
		}
		break;
		case TIM_8b_NUM_2:
		switch (TIM_channel)
		{
			case TIM_8b_CHANNEL_A: TIMSK2 &= ~(1 << OCIE2A); break;
			case TIM_8b_CHANNEL_B: TIMSK2 &= ~(1 << OCIE2B); break;
			default: break;
		}
		break;
		default: break;
	}
}

void tim_8b_prescaler(tim_8b_num_t TIM_number, tim_8b_prescaler_t TIM_prescaler)
{
	switch (TIM_number)
	{
		case TIM_8b_NUM_0: 
			switch (TIM_prescaler)
			{
				case TIM_8b_NO_CLOCK_SOURCE:	TCCR0B = (TCCR0B & 0xF8) | (0 << CS02) | (0 << CS01) | (0 << CS00); break;
				case TIM_8b_PRESCALE_1:			TCCR0B = (TCCR0B & 0xF8) | (0 << CS02) | (0 << CS01) | (1 << CS00); break;	
				case TIM_8b_PRESCALE_8:			TCCR0B = (TCCR0B & 0xF8) | (0 << CS02) | (1 << CS01) | (0 << CS00); break;
				case TIM_8b_PRESCALE_64:		TCCR0B = (TCCR0B & 0xF8) | (0 << CS02) | (1 << CS01) | (1 << CS00); break;
				case TIM_8b_PRESCALE_256:		TCCR0B = (TCCR0B & 0xF8) | (1 << CS02) | (0 << CS01) | (0 << CS00); break;
				case TIM_8b_PRESCALE_1024:		TCCR0B = (TCCR0B & 0xF8) | (1 << CS02) | (0 << CS01) | (1 << CS00); break;
				default: break;
			}
			break;
		case TIM_8b_NUM_2:
			switch (TIM_prescaler)
			{
				case TIM_8b_NO_CLOCK_SOURCE:	TCCR2B = (TCCR2B & 0xF8) | (0 << CS22) | (0 << CS21) | (0 << CS20); break;
				case TIM_8b_PRESCALE_1:			TCCR2B = (TCCR2B & 0xF8) | (0 << CS22) | (0 << CS21) | (1 << CS20); break;
				case TIM_8b_PRESCALE_8:			TCCR2B = (TCCR2B & 0xF8) | (0 << CS22) | (1 << CS21) | (0 << CS20); break;
				case TIM_8b_PRESCALE_64:		TCCR2B = (TCCR2B & 0xF8) | (1 << CS22) | (0 << CS21) | (0 << CS20); break;
				case TIM_8b_PRESCALE_256:		TCCR2B = (TCCR2B & 0xF8) | (1 << CS22) | (1 << CS21) | (0 << CS20); break;
				case TIM_8b_PRESCALE_1024:		TCCR2B = (TCCR2B & 0xF8) | (1 << CS22) | (1 << CS21) | (1 << CS20); break;
				default: break;
			}
			break;
		default: break;
	}
	
}

void tim0_prescaler(tim0_prescaler_t TIM0_prescaler)
{
	TCCR0B = (TCCR0B & 0xF8) | (TIM0_prescaler);
}

void tim2_prescaler(tim2_prescaler_t TIM2_prescaler)
{
	TCCR2B = (TCCR2B & 0xF8) | (TIM2_prescaler);
}

void tim_8b_waveform_mode(tim_8b_num_t TIM_number, tim_8b_mode_t TIM_waveform_mode)
{
	uint8_t WGM10_bits	 = 0;
	uint8_t WGM2_bit	 = 0;
	switch (TIM_waveform_mode)
	{
		case TIM_8b_MODE_NORMAL:							WGM2_bit = (0 << WGM02); WGM10_bits = (0 << WGM01) | (0 << WGM00); break;
		case TIM_8b_MODE_PHASE_CORRECT_PWM_0xFF:			WGM2_bit = (0 << WGM02); WGM10_bits = (0 << WGM01) | (1 << WGM00); break;
		case TIM_8b_MODE_CTC_OCRA:							WGM2_bit = (0 << WGM02); WGM10_bits = (1 << WGM01) | (0 << WGM00); break;
		case TIM_8b_MODE_FAST_PWM_0xFF:						WGM2_bit = (0 << WGM02); WGM10_bits = (1 << WGM01) | (1 << WGM00); break;
		case TIM_8b_MODE_PHASE_CORRECT_PWM_OCRA:			WGM2_bit = (1 << WGM02); WGM10_bits = (0 << WGM01) | (1 << WGM00); break;
		case TIM_8b_MODE_FAST_PWM_OCRnA:					WGM2_bit = (1 << WGM02); WGM10_bits = (1 << WGM01) | (1 << WGM00); break;
		default: break;
	}

	switch (TIM_number)
	{
		case TIM_8b_NUM_0:
			TCCR0A &= ~((1 << WGM01) | (1 << WGM00));
			TCCR0A |= (WGM10_bits);
			TCCR0B &= ~(1 << WGM02);
			TCCR0B |= (WGM2_bit);
			break;
		case TIM_8b_NUM_2:
			TCCR2A &= ~((1 << WGM21) | (1 << WGM20));
			TCCR2A |= (WGM10_bits);
			TCCR2B &= ~(1 << WGM22);
			TCCR2B |= (WGM2_bit);
			break;
		default: break;
	}
}

void tim_8b_compare_output_mode(tim_8b_num_t TIM_number, tim_8b_channel_t TIM_channel, tim_8b_com_t TIM_COM_mode)
{
	uint8_t COM_bits = 0;
	switch (TIM_channel)
	{
		case TIM_8b_CHANNEL_A:
			switch (TIM_COM_mode)
			{
				case TIM_8b_COM_OCnx_DISCONNECTED:		COM_bits = (0 << COM1A1) | (0 << COM1A0); break;
				case TIM_8b_COM_TOGGLE_OCnx:			COM_bits = (0 << COM1A1) | (1 << COM1A0); break;
				case TIM_8b_COM_NON_INVERT_OCnx:		COM_bits = (1 << COM1A1) | (0 << COM1A0); break;
				case TIM_8b_COM_INVERT_OCnx:			COM_bits = (1 << COM1A1) | (1 << COM1A0); break;
				default: break;
			}
			switch (TIM_number)
			{
				case TIM_8b_NUM_0:						TCCR0A	&= ~((1 << COM0A1) | (1 << COM0A0)); TCCR0A |= COM_bits; break;
				case TIM_8b_NUM_2:						TCCR2A	&= ~((1 << COM2A1) | (1 << COM2A0)); TCCR2A |= COM_bits; break;
				default: break;
			}
			break;
		case TIM_8b_CHANNEL_B:
			switch (TIM_COM_mode)
			{
				case TIM_8b_COM_OCnx_DISCONNECTED:		COM_bits = (0 << COM0B1) | (0 << COM0B0); break;
				case TIM_8b_COM_TOGGLE_OCnx:			COM_bits = (0 << COM0B1) | (1 << COM0B0); break;
				case TIM_8b_COM_NON_INVERT_OCnx:		COM_bits = (1 << COM0B1) | (0 << COM0B0); break;
				case TIM_8b_COM_INVERT_OCnx:			COM_bits = (1 << COM0B1) | (1 << COM0B0); break;
				default: break;
			}
			switch (TIM_number)
			{
				case TIM_8b_NUM_0:						TCCR0A	&= ~((1 << COM0B1) | (1 << COM0B0)); TCCR0A |= COM_bits; break;
				case TIM_8b_NUM_2:						TCCR2A	&= ~((1 << COM2B1) | (1 << COM2B0)); TCCR2A |= COM_bits; break;
				default: break;
			}
			break;
		default: break;
	}
}

void tim_8b_ocnx_enable(tim_8b_num_t TIM_number, tim_8b_channel_t TIM_channel)
{
	switch (TIM_number)
	{
		case TIM_8b_NUM_0:
			switch (TIM_channel)
			{
				case TIM_8b_CHANNEL_A: DDRD |= (1 << DDD6); break;
				case TIM_8b_CHANNEL_B: DDRD |= (1 << DDD5); break;
				default: break;
			}
			break;
		case TIM_8b_NUM_2:
			switch (TIM_channel)
			{
				case TIM_8b_CHANNEL_A: DDRB |= (1 << DDB3); break;
				case TIM_8b_CHANNEL_B: DDRD |= (1 << DDD3); break;
				default: break;
			}
			break;
		default: break;
	}
}

void tim_16b_ocnx_disable(tim_8b_num_t TIM_number, tim_8b_channel_t TIM_channel)
{
	switch (TIM_number)
	{
		case TIM_8b_NUM_0:
		switch (TIM_channel)
		{
			case TIM_8b_CHANNEL_A: DDRD &= ~(1 << DDD6); break;
			case TIM_8b_CHANNEL_B: DDRD &= ~(1 << DDD5); break;
			default: break;
		}
		break;
		case TIM_8b_NUM_2:
		switch (TIM_channel)
		{
			case TIM_8b_CHANNEL_A: DDRB &= ~(1 << DDB3); break;
			case TIM_8b_CHANNEL_B: DDRD &= ~(1 << DDD3); break;
			default: break;
		}
		break;
		default: break;
	}
}

void tim_8b_reset(tim_8b_num_t TIM_number)
{
	switch (TIM_number)
	{
		case TIM_8b_NUM_0:
			TCCR0A = 0;
			TCCR0B = 0;
			TCNT0 = 0;
			OCR0A = 0;
			OCR0B = 0;
			TIMSK0 = 0;
			break;
		case TIM_8b_NUM_2:
			TCCR2A = 0;
			TCCR2B = 0;
			TCNT2 = 0;
			OCR2A = 0;
			OCR2B = 0;
			TIMSK2 = 0;
			break;
		default: break;
	}
}