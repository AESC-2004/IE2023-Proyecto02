/*
 * Proyecto02.c
 *
 * Created: 30/04/2025 07:25:47 p. m.
 * Author : ang50
 */ 

/*********************************************************************************************************************************************/
// Libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "m328padc.h"
#include "m328pusart.h"
#include "m328ptims8b.h"
#include "m328ptim1.h"

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Variables, constants and data structures

// ADC variables
uint8_t ADC_Count		= 0;							// Used for selecting which channel the ADC should read
uint8_t ADC_Lec			= 0;							// Used for keeping the last ADC Lecture

// Decoder variables
#define MANUAL			  0								// A "define" that'll come handy for establishing the mode
#define ADAFRUIT		  1								// A "define" that'll come handy for establishing the mode
uint8_t Mode			= MANUAL;						// Used for establishing the motor's positions mode
typedef struct {
	uint8_t	Adafruit[8];
	uint8_t Manual[4];
	uint8_t Usable[8];
} MotorDecoding;
MotorDecoding Motors;									// A struct for organizing them motor's positions depending on the mode
uint8_t TIM0_Count		= 0;							// Used for knowing which Selector's signal to send to the decoder
	
// UART data structure
#define EncodedDataBufferSize 16
char	EncodedData[EncodedDataBufferSize];				// Buffer used for keeping the incoming Adafruit data

// PWM mapping list
#define PWM_TABLE_SIZE 256
uint8_t ADCH_to_PWM[PWM_TABLE_SIZE] = {					// List kept in RAM for accessing the correct OCR1A value to set depending on the ADC Lectures
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
/*********************************************************************************************************************************************/
// Function prototypes
void	SETUP();
void	UART_ParseAdafruitFeedData();

/*********************************************************************************************************************************************/
// Main Function
int main(void)
{
	SETUP();
	
	while (1)
	{
		// Checking for incoming DATA from Adafruit
		UART_ParseAdafruitFeedData();
		
		// Depending on the mode, values are updated to MotorDecoding.Usable[]
		// However, no matter the mode, Motors[5,6,7,8] are always driven by Adafruit
		for (uint8_t i = 4; i < 8; i++) { Motors.Usable[i] = Motors.Adafruit[i];}
		
		switch (Mode)
		{
			case MANUAL:
			{
				for (uint8_t i = 0; i < 4; i++) {
					Motors.Usable[i] = Motors.Manual[i];
				}
				break;
			}
			case ADAFRUIT:
			{
				for (uint8_t i = 0; i < 4; i++) {
					Motors.Usable[i] = Motors.Adafruit[i];
				}
				break;
			}
			default: break;
		}
		
	}
}

/*********************************************************************************************************************************************/
// NON-Interrupt subroutines
void SETUP()
{
	// Deactivating interrupts
	cli();
	
	// Reducing global CLK to 1MHZ
	CLKPR	|= (1 << CLKPCE);
	CLKPR	= (0 << CLKPCE) | (0 << CLKPS3) | (1 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0);
	
	
	// Decoder
	DDRB	|= (1 << DDB5) | (1 << DDB4) | (1 << DDB3) | (1 << DDB2);
	//DDRB	|= (1 << DDB4) | (1 << DDB3) | (1 << DDB2);
	
	// ADC
	adc_init(ADC_REF_AVCC, ADC_PRESCALE_8, ADC_LEFT_ADJUST, ADC_CHANNEL_ADC7, ADC_INTERRUPT_ENABLE, ADC_AUTO_TRIGGER_DISABLE, ADC_TRIGGER_FREE_RUNNING);
	DIDR0  |= (1 << ADC5D) | (1 << ADC4D);
	
	// Initiating TIM0 in CTC mode for DECODING standards
	tim0_init(TIM_8b_CHANNEL_A, TIM0_PRESCALE_64, TIM_8b_MODE_CTC_OCRA, 39, TIM_8b_COM_OCnx_DISCONNECTED, 0, TIM_8b_OCnx_DISABLE);
	tim_8b_oc_interrupt_enable(TIM_8b_NUM_0, TIM_8b_CHANNEL_A);
	
	// Initiating TIM1 in CTC mode for controlling them motors with OCR1A
	tim1_init(TIM1_CHANNEL_A, TIM1_PRESCALE_64, TIM1_MODE_CTC_OCR1A, 0xFFFF, TIM1_COM_OC1x_DISCONNECTED, 0, TIM1_OC1x_DISABLE);
	tim1_oc_interrupt_enable(TIM1_CHANNEL_A);
	
	
	// Initiating UART communication
	// UART 8b, no parity, 1 stop bit, 9600 baud rate
	uart_init(USART_SPEED_DOUBLE, USART_CHARACTER_SIZE_8b, USART_PARITY_MODE_DISABLED, USART_STOP_BIT_1b, USART_MULTIPROCESSOR_COMMUNICATION_MODE_DISABLED, 12);
	usart_rx_interrupt_enable();
	
	// Activating interrupts
	sei();
	
	// Initiating the first ADC Lecture
	adc_start_conversion();
}

// UART_ParseAdafruitFeedData function. The incoming Adafruit data shall be parsed for knowing to which index of the Motors.Adafruit struct should the incoming value
// should be kept. Afterwards, the USART Receive Buffer is flushed for keeping more data.
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
		if (index >= 8) {usart_rx_buffer_flush(); return;}
		
		// If the DATA follows and structure AND the ID is correct, VAL is turned into a string
		uint8_t DATA_Value = (uint8_t)atoi(&EncodedData[3]);	// 'atoi' turns VAL into a string up until ';'
		
		// Data is copied to the correct Adafruit[] index
		switch (index)
		{
			case 0: {Motors.Adafruit[0] = DATA_Value; break;}
			case 1: {Motors.Adafruit[1] = DATA_Value; break;}
			case 2: {Motors.Adafruit[2] = DATA_Value; break;}
			case 3: {Motors.Adafruit[3] = DATA_Value; break;}
			case 4: {Motors.Adafruit[4] = DATA_Value; break;}
			case 5: {Motors.Adafruit[5] = DATA_Value; break;}
			case 6: {Motors.Adafruit[6] = DATA_Value; break;}
			case 7: {Motors.Adafruit[7] = DATA_Value; break;}
			
			default: break;
		}
		
		usart_rx_buffer_flush();
	}
}

/*********************************************************************************************************************************************/
// Interrupt routines

// UART receive interrupt routine. "receive_bytes" function is used.
ISR(USART_RX_vect)
{
	
	usart_receive_bytes();
	
}

// TIM0 OC0A interrupt routine. "TIM0_Count" is incremented, and depending it's value, the decoder's selector bits are correctly
// established, and the correct OCR1A value is uploaded.
ISR(TIMER0_COMPA_vect)
{
	cli();
	
	TIM0_Count++;
	if (TIM0_Count == 8) TIM0_Count = 0;
	switch (TIM0_Count)
	{
		case 0:
			PORTB	|= (1 << PORTB5) | (0 << PORTB4) | (0 << PORTB3) | (0 << PORTB2);
			/*
			PORTB	&= ~(0x1C);
			PORTB	|= (0 << PORTB4) | (0 << PORTB3) | (0 << PORTB2);
			*/
			tim1_ocr_value(TIM1_CHANNEL_A, (uint16_t)ADCH_to_PWM[Motors.Usable[0]]);
			tim1_tcnt_value(0);
			break;
		case 1:
			PORTB	|= (1 << PORTB5) | (0 << PORTB4) | (0 << PORTB3) | (1 << PORTB2);
			/*
			PORTB	&= ~(0x1C);
			PORTB	|= (0 << PORTB4) | (0 << PORTB3) | (1 << PORTB2);
			*/
			tim1_ocr_value(TIM1_CHANNEL_A, (uint16_t)ADCH_to_PWM[Motors.Usable[1]]);
			tim1_tcnt_value(0);
			break;
		case 2:
			PORTB	|= (1 << PORTB5) | (0 << PORTB4) | (1 << PORTB3) | (0 << PORTB2);
			/*
			PORTB	&= ~(0x1C);
			PORTB	|= (0 << PORTB4) | (1 << PORTB3) | (0 << PORTB2);
			*/
			tim1_ocr_value(TIM1_CHANNEL_A, (uint16_t)ADCH_to_PWM[Motors.Usable[2]]);
			tim1_tcnt_value(0);
			break;
		case 3:
			PORTB	|= (1 << PORTB5) | (0 << PORTB4) | (1 << PORTB3) | (1 << PORTB2);
			/*
			PORTB	&= ~(0x1C);
			PORTB	|= (0 << PORTB4) | (1 << PORTB3) | (1 << PORTB2);
			*/
			tim1_ocr_value(TIM1_CHANNEL_A, (uint16_t)ADCH_to_PWM[Motors.Usable[3]]);
			tim1_tcnt_value(0);
			break;
		case 4:
			PORTB	|= (1 << PORTB5) | (1 << PORTB4) | (0 << PORTB3) | (0 << PORTB2);
			/*
			PORTB	&= ~(0x1C);
			PORTB	|= (1 << PORTB4) | (0 << PORTB3) | (0 << PORTB2);
			*/
			tim1_ocr_value(TIM1_CHANNEL_A, (uint16_t)ADCH_to_PWM[Motors.Usable[4]]);
			tim1_tcnt_value(0);
			break;
		case 5:
			PORTB	|= (1 << PORTB5) | (1 << PORTB4) | (0 << PORTB3) | (1 << PORTB2);
			/*
			PORTB	&= ~(0x1C);
			PORTB	|= (1 << PORTB4) | (0 << PORTB3) | (1 << PORTB2);
			*/
			tim1_ocr_value(TIM1_CHANNEL_A, (uint16_t)ADCH_to_PWM[Motors.Usable[5]]);
			tim1_tcnt_value(0);
			break;
		case 6:
			PORTB	|= (1 << PORTB5) | (1 << PORTB4) | (1 << PORTB3) | (0 << PORTB2);
			/*
			PORTB	&= ~(0x1C);
			PORTB	|= (1 << PORTB4) | (1 << PORTB3) | (0 << PORTB2);
			*/
			tim1_ocr_value(TIM1_CHANNEL_A, (uint16_t)ADCH_to_PWM[Motors.Usable[6]]);
			tim1_tcnt_value(0);
			break;
		case 7: 
			PORTB	|= (1 << PORTB5) | (1 << PORTB4) | (1 << PORTB3) | (1 << PORTB2);
			tim1_ocr_value(TIM1_CHANNEL_A, (uint16_t)ADCH_to_PWM[Motors.Usable[7]]);
			tim1_tcnt_value(0);
			break;
		default: break;
	}
	
	sei();
}

// TIM1 OC1A interrupt routine. The decoder is disabled.
ISR(TIMER1_COMPA_vect)
{
	
	cli();
	
	if (TIM0_Count < 8) PORTB	&= ~(0x3C);
	//if (TIM0_Count < 7) PORTB	|= (1 << PORTB4) | (1 << PORTB3) | (1 << PORTB2);
	
	sei();
	
}

// ADC interrupt routine. "ADC_Lec" is used for storing the lecture of the ADC, and "ADC_Count" tells to which index of the Motors.Manual
// struct should the lecture be kept.
ISR(ADC_vect)
{
	
	cli();
	
	ADC_Lec = ADCH;
	switch (ADC_Count)
	{
		case 0:
		ADC_Count = 1;
		Motors.Manual[0] = ADC_Lec;
		adc_channel(ADC_CHANNEL_ADC6);
		break;
		case 1:
		ADC_Count = 2;
		Motors.Manual[1] = ADC_Lec;
		adc_channel(ADC_CHANNEL_ADC5);
		break;
		case 2:
		ADC_Count = 3;
		Motors.Manual[2] = ADC_Lec;
		adc_channel(ADC_CHANNEL_ADC4);
		break;
		case 3:
		ADC_Count = 0;
		Motors.Manual[3] = ADC_Lec;
		adc_channel(ADC_CHANNEL_ADC7);
		break;
		default: break;
	}
	adc_start_conversion();
	
	sei();
}
