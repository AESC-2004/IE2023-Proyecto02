/*
 * Prueba de displays.c
 *
 * Created: 10/05/2025 12:42:23 a. m.
 * Author : ang50
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "m328padc.h"
#include "m328pusart.h"
#include "m328ptims8b.h"
#include "m328ptim1.h"

uint8_t DISP7SEG_PD[10] = {0xFC, 0x18, 0x6C, 0x3C, 0x98, 0xB4, 0xF0, 0x1C, 0xFC, 0x9C};
uint8_t DISP7SEG_PB[10] = {0, 0, 1, 1, 1, 1, 1, 0, 1, 1};
uint8_t Count = 1;

int main(void)
{
	DDRD	|= (1 << DDD7) | (1 << DDD6)| (1 << DDD5) | (1 << DDD4) | (1 << DDD3)| (1 << DDD2);
	DDRB	|= (1 << DDB1) | (1 << DDB0);
	PORTB	|= (1 << DDB1);
	

	tim1_init(TIM1_CHANNEL_A, TIM1_PRESCALE_256, TIM1_MODE_NORMAL, 0xFFFF, TIM1_COM_OC1x_DISCONNECTED, 3036, TIM1_OC1x_DISABLE);
	tim1_ovf_interrupt_enable();
	sei();
	
    /* Replace with your application code */
    while (1) 
    {
    }
}

ISR(TIMER1_OVF_vect)
{
	if(Count == 10) Count = 0;
	PORTD	&= ~(0xFC);
	PORTD	|= DISP7SEG_PD[Count];
	PORTB	&= ~(0x01);
	PORTB	|= DISP7SEG_PB[Count];
	Count++;
	tim1_tcnt_value(3036);

}
