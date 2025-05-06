/*
 * m328pusart.c
 *
 * Created: 29/04/2025 22:11:05
 *  Author: ang50
 */ 

/*********************************************************************************************************************************************/
// m328pusart.c - USART library for ATmega328P
/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Libraries
#include <avr/io.h>
#include <stdint.h>
#include "m328pusart.h"
/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Internal static variables
volatile static uint8_t USART_tx_buffer[USART_TX_BUFFER_SIZE];
volatile static uint8_t USART_tx_index	= 0;
volatile static uint8_t USART_tx_length = 0;
volatile static uint8_t USART_ready		= 1;

volatile static uint8_t USART_rx_buffer[USART_RX_BUFFER_SIZE];
volatile static uint8_t USART_rx_index = 0;
volatile static uint8_t USART_rx_ready = 0;

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// USART initiation functions 

void	usart_init(usart_mode_t				USART_mode,
				   usart_clock_polarity_t	USART_clock_polarity,
				   usart_speed_t			USART_speed,
				   usart_character_size_t	USART_character_size,
				   usart_parity_mode_t		USART_parity_mode,
				   usart_stop_bit_t			USART_stop_bit,
				   usart_mpcm_t				USART_multiprocessor_mode,
				   uint16_t					USART_baud_value)
{
	// Initial values
	UCSR0B	= 0;
	UCSR0C	= 0;
	// Setting the USART BAUD RATE
	UBRR0	= USART_baud_value;
	// Setting USCR1C
	UCSR0C	|= (USART_mode) | (USART_parity_mode) | (USART_stop_bit);
	// Setting the USART DATA SIZE
	switch (USART_character_size)
	{
		case USART_CHARACTER_SIZE_5b: UCSR0B |= (0 << UCSZ02); UCSR0C |= (0 << UCSZ01) | (0 << UCSZ00); break;
		case USART_CHARACTER_SIZE_6b: UCSR0B |= (0 << UCSZ02); UCSR0C |= (0 << UCSZ01) | (1 << UCSZ00); break;
		case USART_CHARACTER_SIZE_7b: UCSR0B |= (0 << UCSZ02); UCSR0C |= (1 << UCSZ01) | (0 << UCSZ00); break;
		case USART_CHARACTER_SIZE_8b: UCSR0B |= (0 << UCSZ02); UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); break;
		case USART_CHARACTER_SIZE_9b: UCSR0B |= (1 << UCSZ02); UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); break;
		default: break;
	}
	// USART multiprocessor mode
	switch (USART_multiprocessor_mode)
	{
		case USART_MULTIPROCESSOR_COMMUNICATION_MODE_ENABLED:	UCSR0A &= ~(1 << MPCM0); UCSR0A |= (1 << MPCM0); break;
		case USART_MULTIPROCESSOR_COMMUNICATION_MODE_DISABLED:	UCSR0A &= ~(1 << MPCM0); UCSR0A |= (0 << MPCM0); break;
		default: break;
	}
	// Other functions depending on synchronous or asynchronous modes
	switch (USART_mode)
	{
		case USART_MODE_ASYNCHRONOUS:						UCSR0A |= USART_speed;					UCSR0C |= (USART_mode) | (0 << UCPOL0);			break;
		case USART_MODE_SYNCHRONOUS:						UCSR0A |= (0 << U2X0);					UCSR0C |= (USART_mode) | USART_clock_polarity;	break;
		case USART_MODE_MASTER_SPI:							UCSR0C |= (USART_mode);					break;
		default: break;
	}
	
	// Enable receiver and transmitter
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0);
}

void	uart_init(usart_speed_t				USART_speed,
				  usart_character_size_t	USART_character_size,
				  usart_parity_mode_t		USART_parity_mode,
				  usart_stop_bit_t			USART_stop_bit,
				  usart_mpcm_t				USART_multiprocessor_mode,
				  uint16_t					USART_baud_value)
{
	usart_init(USART_MODE_ASYNCHRONOUS, USART_CLOCK_POLARITY_RISING_EDGE, USART_speed, USART_character_size, USART_parity_mode, USART_stop_bit, USART_multiprocessor_mode, USART_baud_value);
}

void	usrt_init(usart_clock_polarity_t	USART_clock_polarity,
				  usart_character_size_t	USART_character_size,
				  usart_parity_mode_t		USART_parity_mode,
				  usart_stop_bit_t			USART_stop_bit,
				  usart_mpcm_t				USART_multiprocessor_mode,
				  uint16_t					USART_baud_value)
{
	usart_init(USART_MODE_SYNCHRONOUS, USART_clock_polarity, USART_SPEED_NORMAL, USART_character_size, USART_parity_mode, USART_stop_bit, USART_multiprocessor_mode, USART_baud_value);
}

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// USART sub-protocoled communication functions

void	usart_multiprocessor_enable()
{
	UCSR0A	|= (1 << MPCM0);
}


void	usart_multiprocessor_disable()
{
	UCSR0A	&= ~(1 << MPCM0);
}

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// USART transmit functions

void	usart_polling_transmit(unsigned char USART_data)
{
	// Check how many bits are being sent
	uint8_t UCSZ = 0;
	UCSZ = UCSR0B & (1<<UCSZ02);
	if (UCSZ)		// If DATA = 9b
	{
		// Wait for empty transmit buffer
		while ( !( UCSR0A & (1<<UDRE0)));
		// Copy 9th bit to TXB8
		UCSR0B &= ~(1<<TXB80);
		if ( (unsigned int)USART_data & 0x0100 )
		{
			UCSR0B |= (1<<TXB80);
		}
		// Put data into buffer, sends the data
		UDR0 = (unsigned int)USART_data;
	} else          // If DATA < 9b
	{
		// Wait for empty transmit buffer
		while ( !( UCSR0A & (1<<UDRE0)));
		// Put data into buffer
		UDR0 = USART_data;
	}
}

void	usart_transmit_bytes(const uint8_t* USART_buffer, uint8_t USART_length)
{
	// Transmission will be done ONLY IF the data length is lower than the characteristic transmit buffer stated.
	if (USART_ready && USART_length <= USART_TX_BUFFER_SIZE)
	{
		for (uint8_t i=0; i<USART_length; i++)
		{
			USART_tx_buffer[i] = USART_buffer[i];
		}
		
		// Declaring values for static variables
		USART_tx_index = 0;
		USART_tx_length = USART_length;
		USART_ready = 0;
		
		usart_data_register_empty_interrupt_enable();
	}
}

void	usart_transmit_string(const char* USART_string)
{
	uint8_t length = 0;
	while (USART_string[length] != '\0')
	{
		length++;
	}
	usart_transmit_bytes((const uint8_t*)USART_string, length);
}

uint8_t	usart_load_next_byte()
{
	// USART_ready=0 tells if any transmission is being done.
	// So, if USART_ready=0, the next byte (if any) should be uploaded
	if (!USART_ready)
	{
		// If the last byte has not been sent, then we keep sending bytes.
		if (USART_tx_index < USART_tx_length)
		{
			UDR0 = USART_tx_buffer[USART_tx_index++];
			return 1;
		} else
		{
			// If the last byte has been sent, then we stop sending bytes.
			USART_ready = 1;
			return 0;
		}
	}
	return 0;
}

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// USART receive functions

unsigned char	usart_polling_receive()
{
	// Check how many bits are being sent
	uint8_t UCSZ = 0;
	UCSZ = (UCSR0B & (1<<UCSZ02));
	if (UCSZ)		// If DATA = 9b
	{
		unsigned char status, resh, resl;
		// Wait for data to be received
		while ( !(UCSR0A & (1<<RXC0)) );
		// Get status and 9th bit, then data from buffer
		status = UCSR0A;
		resh = UCSR0B;
		resl = UDR0;
		// If error, return -1
		if ( status & (1<<FE0)|(1<<DOR0)|(1<<UPE0) )
		{
			return (unsigned char)-1;
		}
		else
		{
			// Filter the 9th bit, then return
			resh = (resh >> 1) & 0x01;
			return (unsigned char)((resh << 8) | resl);
		}
	} else
	{
		// Wait for data to be received
		while ( !(UCSR0A & (1<<RXC0)) );
		// Get and return received data from buffer
		return UDR0;
	}
}

void usart_receive_bytes()
{
	// If there are no errors while receiving
	if (!(UCSR0A & ((1<<FE0) | (1<<DOR0) | (1<<UPE0))))
	{
		// If the byte to store still has space inside the RX buffer, it is stored.
		if (USART_rx_index < USART_RX_BUFFER_SIZE)
		{
			USART_rx_buffer[USART_rx_index++] = UDR0;
		}
		// If not, nothing is done
		else{}
		// If there was an error while receiving, the data is discarded
	} else
	{
		uint8_t dummy = UDR0;
	}
}

void	usart_receive_string(char* USART_destination_buffer)
{
	
	// This function shall use the declared buffer to store every character received AND
	// add a "\0" null terminator (So it is, in fact, a string)
	
	for (uint8_t i = 0; (i < usart_get_received_length()) && (i < (USART_RX_BUFFER_SIZE - 1)); i++)
	{
		
		// If either "\r" or "\n" are found, then we break the loop.
		if ((usart_get_received_byte(i) == '\r') || (usart_get_received_byte(i) == '\r'))
		{
			USART_destination_buffer[i] = '\0';
			return;
		} else
		{
			USART_destination_buffer[i] = (char)usart_get_received_byte(i);
		}
	}
	
	// Null-terminate the string explicitly
	USART_destination_buffer[usart_get_received_length()] = '\0';
}

uint8_t usart_get_received_byte(uint8_t USART_index)
{
	// If the index (Or address) needed to be shown is shorter than the current receiving index:
	// Then, no info. is returned.
	// (I.e. if it is wanted to look for the last received byte, but a bigger index is used,
	//  other info. may be returned)
	
	if (USART_index < USART_rx_index)
	{
		return USART_rx_buffer[USART_index];
		} else {
		return 0xFF; // Return -1 in signed int nomenclature.
	}
}

uint8_t usart_get_received_length()
{
	return USART_rx_index;
}

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// USART flush received data functions

void	usart_rx_buffer_flush(void)
{
	for (uint8_t i = 0; i < USART_RX_BUFFER_SIZE; i++)
	{
		USART_rx_buffer[i] = 0x00;
	}
	USART_rx_index = 0;
}

void	usart_polling_flush()
{
	unsigned char dummy;
	while ( UCSR0A & (1<<RXC0) )
	{
		dummy = UDR0;
	}
}

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// USART interrupt enabling functions

void	usart_rx_interrupt_enable()
{
	UCSR0B |= (1 << RXCIE0);
}

void	usart_tx_interrupt_enable()
{
	UCSR0B |= (1 << TXCIE0);
}

void	usart_data_register_empty_interrupt_enable()
{
	UCSR0B |= (1 << UDRIE0);
}

void	usart_rx_interrupt_disable()
{
	UCSR0B &= ~(1 << RXCIE0);
}

void	usart_tx_interrupt_disable()
{
	UCSR0B &= ~(1 << TXCIE0);
}

void	usart_data_register_empty_interrupt_disable()
{
	UCSR0B &= ~(1 << UDRIE0);
}

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// USART frame format functions

void	usart_frame_format(usart_character_size_t USART_character_size, usart_parity_mode_t USART_parity_mode, usart_stop_bit_t USART_stop_bit)
{
	UCSR0B &= ~(1 << UCSZ02);
	UCSR0C &= ~((1 << UCSZ01) | (1 << UCSZ00) | (1 << UPM01) | (1 << UPM00) | (1 << USBS0));
	switch (USART_character_size)
	{
		case USART_CHARACTER_SIZE_5b: UCSR0B |= (0 << UCSZ02); UCSR0C |= (0 << UCSZ01) | (0 << UCSZ00) | (USART_parity_mode) | (USART_stop_bit); break;
		case USART_CHARACTER_SIZE_6b: UCSR0B |= (0 << UCSZ02); UCSR0C |= (0 << UCSZ01) | (1 << UCSZ00) | (USART_parity_mode) | (USART_stop_bit); break;
		case USART_CHARACTER_SIZE_7b: UCSR0B |= (0 << UCSZ02); UCSR0C |= (1 << UCSZ01) | (0 << UCSZ00) | (USART_parity_mode) | (USART_stop_bit); break;
		case USART_CHARACTER_SIZE_8b: UCSR0B |= (0 << UCSZ02); UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00) | (USART_parity_mode) | (USART_stop_bit); break;
		case USART_CHARACTER_SIZE_9b: UCSR0B |= (1 << UCSZ02); UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00) | (USART_parity_mode) | (USART_stop_bit); break;
		default: break;
	}
}

void	usart_character_size(usart_character_size_t USART_character_size)
{
	UCSR0B &= ~(1 << UCSZ02);
	UCSR0C &= ~((1 << UCSZ01) | (1 << UCSZ00));
	switch (USART_character_size)
	{
		case USART_CHARACTER_SIZE_5b: UCSR0B |= (0 << UCSZ02); UCSR0C |= (0 << UCSZ01) | (0 << UCSZ00); break;
		case USART_CHARACTER_SIZE_6b: UCSR0B |= (0 << UCSZ02); UCSR0C |= (0 << UCSZ01) | (1 << UCSZ00); break;
		case USART_CHARACTER_SIZE_7b: UCSR0B |= (0 << UCSZ02); UCSR0C |= (1 << UCSZ01) | (0 << UCSZ00); break;
		case USART_CHARACTER_SIZE_8b: UCSR0B |= (0 << UCSZ02); UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); break;
		case USART_CHARACTER_SIZE_9b: UCSR0B |= (1 << UCSZ02); UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); break;
		default: break;
	}
}

void	usart_parity_mode(usart_parity_mode_t USART_parity_mode)
{
	UCSR0C &= ~((1 << UPM01) | (1 << UPM00));
	UCSR0C |= (USART_parity_mode);
}

void	usart_stop_bit(usart_stop_bit_t USART_stop_bit)
{
	UCSR0C &= ~(1 << USBS0);
	UCSR0C |= (USART_stop_bit);
}

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// USART pin enabling functions

void	usart_rx_enable()
{
	UCSR0B |= (1 << RXEN0);
}

void	usart_tx_enable()
{
	UCSR0B |= (1 << TXEN0);
}

void	usart_rx_disable()
{
	UCSR0B &= ~(1 << RXEN0);
}

void	usart_tx_disable()
{
	UCSR0B &= ~(1 << TXEN0);
}

/*********************************************************************************************************************************************/