/*
 * Prueba UART-Decoder.c
 *
 * Created: 11/05/2025 07:00:50 p. m.
 * Author : ang50
 */ 

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "m328padc.h"
#include "m328pusart.h"
#include "m328ptims8b.h"
#include "m328ptim1.h"

// Decoding
uint8_t PWM_Values[2]	= {230, 230};
uint8_t TIM0_Count	= 0;

// UART
void	UART_ParseAdafruitFeedData();
#define EncodedDataBufferSize 16
char	EncodedData[EncodedDataBufferSize];


// PWM
#define PWM_TABLE_SIZE 256
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
	
	// Initiating TIM0 for DECODING standards
	tim0_init(TIM_8b_CHANNEL_A, TIM0_PRESCALE_64, TIM_8b_MODE_CTC_OCRA, 39, TIM_8b_COM_OCnx_DISCONNECTED, 0, TIM_8b_OCnx_DISABLE);
	tim_8b_oc_interrupt_enable(TIM_8b_NUM_0, TIM_8b_CHANNEL_A);
	
	// Initiating TIM1 for controlling them motors
	tim1_init(TIM1_CHANNEL_A, TIM1_PRESCALE_64, TIM1_MODE_CTC_OCR1A, 0xFFFF, TIM1_COM_OC1x_DISCONNECTED, 0, TIM1_OC1x_DISABLE);
	tim1_oc_interrupt_enable(TIM1_CHANNEL_A);
	
	
	// Initiating UART communication
	// UART 8b, no parity, 1 stop bit, 9600 baud rate
	uart_init(USART_SPEED_DOUBLE, USART_CHARACTER_SIZE_8b, USART_PARITY_MODE_DISABLED, USART_STOP_BIT_1b, USART_MULTIPROCESSOR_COMMUNICATION_MODE_DISABLED, 12);
	//usart_data_register_empty_interrupt_enable();
	usart_rx_interrupt_enable();
	
	// Activating interrupts
	sei();
	
    /* Replace with your application code */
    while (1) 
    {
		UART_ParseAdafruitFeedData();
		
    }
}

void	UART_ParseAdafruitFeedData()
{
	if ((usart_get_received_length() > 0) && (((usart_get_received_byte((uint8_t)(usart_get_received_length() - 1))) == '\r') || ((usart_get_received_byte((uint8_t)(usart_get_received_length() - 1))) == '\n')))
	{
		// Using the EncodedData Receive Buffer for receiving the incoming data
		usart_receive_string(EncodedData);
		
		// Starting the parsing process
		
		// Every DATA incoming shall have the following structure: "#X:VAL;"
		// If the first received character is not "#", or the third one is not ":", the Data is trashed
		if ((EncodedData[0] != '#') || (EncodedData[2] != ':')) {usart_rx_buffer_flush(); return;}
		
		// If the DATA follows the structure, it is needed to know if the ID is correct
		// Then, the ID is extracted from the EncodedData Receive Buffer
		// As in ASCII all characters are correctly listed (A=65, B=66, C=67, ...), the following code shall look if the ID is valid
		// If 7 motors are to be controlled, the only possible ID's are 'A', 'B', 'C', 'D', 'E', 'F' and 'G'
		// If any other character for the ID is used (i.e. 'H' or 'c'), subtracting 'A' from the ID will not give a result from 0 to 7
		// If so, the DATA is trashed
		char id = EncodedData[1];
		uint8_t index = id - 'A';								// If the ID is correct, index should keep values from 0 to 7
		if (index >= 7) {usart_rx_buffer_flush(); return;}
		
		// If the DATA follows and structure AND the ID is correct, VAL is turned into a string
		uint8_t DATA_Value = (uint8_t)atoi(&EncodedData[3]);	// 'atoi' turns VAL into a string up until ';'
		
		// Data is copied to the correct PWM_Values[] index
		switch (index)
		{
			case 0: PWM_Values[0] = DATA_Value;
			case 1: PWM_Values[1] = DATA_Value;
			default: break;
		}
		
		usart_rx_buffer_flush();
	}
}


ISR(USART_RX_vect)
{
	
	usart_receive_bytes();
	
}

ISR(USART_UDRE_vect)
{
	
	usart_load_next_byte();
	
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
		default: break;
	}
	
	sei();
}


ISR(TIMER1_COMPA_vect)
{
	
	cli();
	
	if (TIM0_Count < 4) PORTB |= (1 << PORTB4) | (1 << PORTB3) | (1 << PORTB2);
	
	sei();
	
}


