/*
 * Prueba encoder.c
 *
 * Created: 10/05/2025 01:46:42 a. m.
 * Author : ang50
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "m328padc.h"
#include "m328pusart.h"
#include "m328ptims8b.h"
#include "m328ptim1.h"

#define AUTO		1 
#define MANUAL		0 

#define ENABLED		1
#define DISABLED	0

uint8_t DISP7SEG_NUMS_PD[10] = {0xFC, 0x18, 0x6C, 0x3C, 0x98, 0xB4, 0xF0, 0x1C, 0xFC, 0x9C};
uint8_t DISP7SEG_MODES_PD[3] = {0XD8, 0XDC};
uint8_t DISP7SEG_NUMS_PB[10] = {0, 0, 1, 1, 1, 1, 1, 0, 1, 1};
uint8_t DISP7SEG_MODES_PB[3] = {1, 1};

uint8_t Count			= 1;
uint8_t Count_Mode		= MANUAL;
uint8_t Selection_Mode	= DISABLED;

volatile uint8_t	ENCODER_SW_Last			= 1;			// SW starts as 1 (Pull-up)
uint8_t				ENCODER_SW_Push_Time	= 0;

uint8_t Blink_Count		= 0;




int main(void)
{
	// Display
	DDRD	|= (1 << DDD7) | (1 << DDD6) | (1 << DDD5) | (1 << DDD4) | (1 << DDD3)| (1 << DDD2);
	DDRB	|= (1 << DDB1) | (1 << DDB0);
	PORTB	|= (1 << DDB1);
	
	// Encoder
	DDRC	&= ~((1 << DDC3) | (1 << DDC2) | (1 << DDC1));
	PORTC	|= (1 << DDC3);							// Pull-up enabled for SW
	PORTC	&= ~((1 << DDC2) | (1 << DDC1));		// Pull-up disabled for DATA and CLK
	PCICR	|= (1 << PCIE1);
	PCMSK1	|= (1 << PCINT11) | (1 << PCINT9);		// Masked SW and DATA only
	
	// TIM0 for blinking purposes
	tim0_init(TIM_8b_CHANNEL_A, TIM0_PRESCALE_256, TIM_8b_MODE_NORMAL, 0, TIM_8b_COM_OCnx_DISCONNECTED, 0, TIM_8b_OCnx_DISABLE);
	tim_8b_ovf_interrupt_enable(TIM_8b_NUM_0);
	
	// TIM1 for counting 1 sec.
	tim1_init(TIM1_CHANNEL_A, TIM1_PRESCALE_256, TIM1_MODE_NORMAL, 0xFFFF, TIM1_COM_OC1x_DISCONNECTED, 3036, TIM1_OC1x_DISABLE);
	tim1_ovf_interrupt_enable();
	
	// TIM2 for counting up to 2 secs. when needed
	tim2_init(TIM_8b_CHANNEL_A, TIM2_PRESCALE_1024, TIM_8b_MODE_NORMAL, 0, TIM_8b_COM_OCnx_DISCONNECTED, 0, TIM_8b_OCnx_DISABLE);
	sei();
	
	
	/* Replace with your application code */
	while (1)
	{
		if (Selection_Mode == ENABLED)
		{
			if (Blink_Count == 122)
			{
				PORTB ^= (1 << PORTB1);
				Blink_Count = 0;
			}
			PORTD	&= ~(0xFC);
			PORTD	|= DISP7SEG_MODES_PD[Count_Mode];
			PORTB	&= ~(0x01);
			PORTB	|= DISP7SEG_MODES_PB[Count_Mode];
		} else
		{
			PORTD	&= ~(0xFC);
			PORTD	|= DISP7SEG_NUMS_PD[Count];
			PORTB	&= ~(0x01);
			PORTB	|= DISP7SEG_NUMS_PB[Count];
			PORTB	|= (1 << PORTB1);
		}
	}
}

ISR(PCINT1_vect)
{
	cli();
	
	// SW logic
	// PINC3 value is saved
	uint8_t SW_State = (PINC & (1 << PINC3)) ? 1 : 0;
	// If SW is being pushed, and if it was not being pressed before, TIM2 starts
	if ((SW_State == 0) && ENCODER_SW_Last == 1)			// Falling edge detected: SW pushed
	{
		tim_8b_tcnt_value(TIM_8b_NUM_2, 0);
		tim_8b_ovf_interrupt_enable(TIM_8b_NUM_2);
	}
	// If SW is not pushed anymore (If being pressed before), and if the total pushed time
	// is less than 2secs, Selection_Mode is updated
	else if (SW_State == 1 && ENCODER_SW_Last == 0)			// Rising edge detected: SW liberated
	{
		tim_8b_ovf_interrupt_disable(TIM_8b_NUM_2);
		if (ENCODER_SW_Push_Time < 122)
		{
			ENCODER_SW_Push_Time = 0;
			if (Selection_Mode == DISABLED) 
			{
				Selection_Mode = ENABLED; 
				tim_8b_ovf_interrupt_enable(TIM_8b_NUM_0);
			} else 
			{
				Selection_Mode = DISABLED;
				tim_8b_ovf_interrupt_disable(TIM_8b_NUM_0);
				Blink_Count = 0;
			} 
		}
	}
	ENCODER_SW_Last = SW_State;								// ENCODER_SW_Last updated
	
	// DATA and CLK logic
	// PINC2,1 values are saved
	uint8_t DATA_State = (PINC & (1 << PINC2));
	uint8_t CLK_State = (PINC & (1 << PINC1));
	if (!CLK_State)		// Falling edge detected: Encoder spin
	{
		if (Selection_Mode == DISABLED)
		{
			if (Count_Mode == MANUAL)
			{
				if (DATA_State)
				{
					Count++;
					if (Count == 10) Count = 0;
					
					} else if (!DATA_State){
					Count--;
					if (Count == 255) Count = 9;
				}
			}
		} else if (Selection_Mode == ENABLED)
		{
			if (Count_Mode == AUTO) Count_Mode = MANUAL;
			else Count_Mode = AUTO;
		}
		
	}
	
	sei();
}

ISR(TIMER0_OVF_vect)
{
	
	cli();
	
		Blink_Count++;
	
	sei();

}

ISR(TIMER1_OVF_vect)
{
	
	cli();
	
	// If the count mode is automatic (Count_Mode = 1), Count is updated
	if (Count_Mode == AUTO)
	{
		Count++;
		if(Count == 10) Count = 0;
	}
	
	tim1_tcnt_value(3036);
	
	sei();

}

ISR(TIMER2_OVF_vect)
{
	cli();
	
	ENCODER_SW_Push_Time++;
	
	// If 2secs. are reached WHILE PRESSING (!(PINC & (1 << PINC3))), the count mode is updated
	if ((ENCODER_SW_Push_Time >= 122) && !(PINC & (1 << PINC3))) 
	{
		ENCODER_SW_Push_Time = 255;			// So that when liberating SW, no logic issues are presented
		tim_8b_ovf_interrupt_disable(TIM_8b_NUM_2);
		if (Count_Mode == MANUAL) Count_Mode = AUTO;
		else Count_Mode = MANUAL;
	}

	sei();

}
