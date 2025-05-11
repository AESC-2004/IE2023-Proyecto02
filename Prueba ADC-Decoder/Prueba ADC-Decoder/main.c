/*
 * Prueba ADC-Decoder.c
 *
 * Created: 10/05/2025 09:02:30 p. m.
 * Author : ang50
 */ 

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "m328padc.h"
#include "m328pusart.h"
#include "m328ptims8b.h"
#include "m328ptim1.h"

#define PWM_TABLE_SIZE 256

uint8_t ADC_Count		= 0;
uint8_t ADC_Lec			= 0;
uint8_t PWM_Values[4]	= {0, 0, 0, 0};

uint8_t TIM0_Count	= 0;

uint8_t ADCH_to_PWM[PWM_TABLE_SIZE] = {
	8,8,8,8,8,8,8,8,8,9,9,9,9,9,9,9,
	9,10,10,10,10,10,10,10,10,10,11,11,11,11,11,11,
	11,11,12,12,12,12,12,12,12,12,12,13,13,13,13,13,
	13,13,13,14,14,14,14,14,14,14,14,14,15,15,15,15,
	15,15,15,15,16,16,16,16,16,16,16,16,16,17,17,17,
	17,17,17,17,17,18,18,18,18,18,18,18,18,18,19,19,
	19,19,19,19,19,19,20,20,20,20,20,20,20,20,20,21,
	21,21,21,21,21,21,21,22,22,22,22,22,22,22,22,22,
	23,23,23,23,23,23,23,23,24,24,24,24,24,24,24,24,
	24,25,25,25,25,25,25,25,25,26,26,26,26,26,26,26,
	26,26,27,27,27,27,27,27,27,27,28,28,28,28,28,28,
	28,28,28,29,29,29,29,29,29,29,29,30,30,30,30,30,
	30,30,30,30,31,31,31,31,31,31,31,31,32,32,32,32,
	32,32,32,32,32,33,33,33,33,33,33,33,33,34,34,34,
	34,34,34,34,34,34,35,35,35,35,35,35,35,35,36,36,
	36,36,36,36,36,36,36,37,37,37,37,37,37,37,37,38
};


int main(void)
{
	// Deactivating interrupts
	cli();
	
	// Reducing global CLK to 1MHZ
	CLKPR	|= (1 << CLKPCE);
	CLKPR	= (0 << CLKPCE) | (0 << CLKPS3) | (1 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0);
	
	// Decoder
	DDRB	|= (1 << DDB4) | (1 << DDB3) | (1 << DDB2);
	
	// Display
	//DDRD	|= (1 << DDD7) | (1 << DDD6)| (1 << DDD5) | (1 << DDD4) | (1 << DDD3)| (1 << DDD2);
	//PORTD	|= (1 << DDD7) | (1 << DDD6)| (1 << DDD5) | (1 << DDD4) | (1 << DDD3)| (1 << DDD2);
	//DDRB	|= (1 << DDB1) | (1 << DDB0);
	//PORTB	|= (1 << DDB1) | (1 << DDB0);
	
	// ADC
	adc_init(ADC_REF_AVCC, ADC_PRESCALE_8, ADC_LEFT_ADJUST, ADC_CHANNEL_ADC7, ADC_INTERRUPT_ENABLE, ADC_AUTO_TRIGGER_DISABLE, ADC_TRIGGER_FREE_RUNNING);
	DIDR0  |= (1 << ADC5D) | (1 << ADC4D); 
	
	// Initiating TIM0 for DECODING standards
	tim0_init(TIM_8b_CHANNEL_A, TIM0_PRESCALE_64, TIM_8b_MODE_CTC_OCRA, 39, TIM_8b_COM_OCnx_DISCONNECTED, 0, TIM_8b_OCnx_DISABLE);
	tim_8b_oc_interrupt_enable(TIM_8b_NUM_0, TIM_8b_CHANNEL_A);
	
	// Initiating TIM1 for controlling them motors
	tim1_init(TIM1_CHANNEL_A, TIM1_PRESCALE_64, TIM1_MODE_CTC_OCR1A, 0xFFFF, TIM1_COM_OC1x_DISCONNECTED, 0, TIM1_OC1x_DISABLE);
	tim1_oc_interrupt_enable(TIM1_CHANNEL_A);
	
	// Activating interrupts
	sei();
	
	// Initiating the first ADC Lecture
	adc_start_conversion(); 
	
    /* Replace with your application code */
    while (1) 
    {
    }
}


ISR(TIMER0_COMPA_vect)
{
	cli();
	
	TIM0_Count++;
	if (TIM0_Count == 8) TIM0_Count = 0;
	switch (TIM0_Count)
	{
		case 0:
			PORTB	&= ~(0x1C);
			PORTB	|= (0 << PORTB3) | (0 << PORTB3) | (0 << PORTB2);
			tim1_ocr_value(TIM1_CHANNEL_A, (uint16_t)ADCH_to_PWM[PWM_Values[0]]);
			tim1_tcnt_value(0);
			break;
		case 1:
			PORTB	&= ~(0x1C);
			PORTB	|= (0 << PORTB3) | (0 << PORTB3) | (1 << PORTB2);
			tim1_ocr_value(TIM1_CHANNEL_A, (uint16_t)ADCH_to_PWM[PWM_Values[1]]);
			tim1_tcnt_value(0);
			break;
		case 2:
			PORTB	&= ~(0x1C);
			PORTB	|= (0 << PORTB3) | (1 << PORTB3) | (0 << PORTB2);
			tim1_ocr_value(TIM1_CHANNEL_A, (uint16_t)ADCH_to_PWM[PWM_Values[2]]);
			tim1_tcnt_value(0);
			break;
		case 3:
			PORTB	&= ~(0x1C);
			PORTB	|= (0 << PORTB3) | (1 << PORTB3) | (1 << PORTB2);
			tim1_ocr_value(TIM1_CHANNEL_A, (uint16_t)ADCH_to_PWM[PWM_Values[3]]);
			tim1_tcnt_value(0);
			break;
		default: break;
	}
	
	sei();
}


ISR(TIMER1_COMPA_vect)
{
	
	cli();
	
	if (TIM0_Count < 4) PORTB |= (1 << PORTB4);

	//PORTB	^= (1 << PORTB2);
	//PORTB	|= (1 << PORTB4) | (1 << PORTB3) | (1 << PORTB2);
	//tim1_oc_interrupt_disable(TIM1_CHANNEL_A);
	
	sei();
	
}


ISR(ADC_vect)
{
	
	cli();
	
	ADC_Lec = ADCH;
	switch (ADC_Count)
	{
		case 0:
			ADC_Count = 1;
			PWM_Values[0] = ADC_Lec;
			adc_channel(ADC_CHANNEL_ADC6);
			break;
		case 1:
			ADC_Count = 2;
			PWM_Values[1] = ADC_Lec;
			adc_channel(ADC_CHANNEL_ADC5);
			break;
		case 2:
			ADC_Count = 3;
			PWM_Values[2] = ADC_Lec;
			adc_channel(ADC_CHANNEL_ADC4);
			break;
		case 3:
			ADC_Count = 0;
			PWM_Values[3] = ADC_Lec;
			adc_channel(ADC_CHANNEL_ADC7);
			break;
		default: break;
		
	}
	adc_start_conversion();
	
	sei();
}


