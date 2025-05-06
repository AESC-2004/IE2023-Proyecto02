/*
 * m328pusart.h
 *
 * Created: 29/04/2025 22:10:50
 *  Author: ang50
 */ 

/*********************************************************************************************************************************************/
// m32u4usart.h - USART library for ATmega328P
/*********************************************************************************************************************************************/

#ifndef M328PUSART_H_
#define M328PUSART_H_

/*********************************************************************************************************************************************/
// External libraries
#include <avr/io.h>
#include <stdint.h>
/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Config. characteristics
#define USART_TX_BUFFER_SIZE 64
#define USART_RX_BUFFER_SIZE 64
/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Enumerations

// USART_SPEED enumeration
typedef enum {
	USART_SPEED_NORMAL									= (0 << U2X0),
	USART_SPEED_DOUBLE									= (1 << U2X0)
} usart_speed_t;

// USART_MPCM enumeration
typedef enum {
	USART_MULTIPROCESSOR_COMMUNICATION_MODE_ENABLED		= (1 << MPCM0),
	USART_MULTIPROCESSOR_COMMUNICATION_MODE_DISABLED	= (0 << MPCM0),
} usart_mpcm_t;

// USART_CHARACTER_SIZE enumeration
typedef enum {
	USART_CHARACTER_SIZE_5b,
	USART_CHARACTER_SIZE_6b,
	USART_CHARACTER_SIZE_7b,
	USART_CHARACTER_SIZE_8b,
	USART_CHARACTER_SIZE_9b
} usart_character_size_t;

// USART_MODE enumeration
typedef enum {
	USART_MODE_ASYNCHRONOUS								= (0 << UMSEL01) | (0 << UMSEL00),
	USART_MODE_SYNCHRONOUS								= (0 << UMSEL01) | (1 << UMSEL00),
	USART_MODE_MASTER_SPI								= (1 << UMSEL01) | (1 << UMSEL00)
} usart_mode_t;

// USART_PARITY_MODE enumeration
typedef enum {
	USART_PARITY_MODE_DISABLED							= (0 << UPM01) | (0 << UPM00),
	USART_PARITY_MODE_EVEN								= (1 << UPM01) | (0 << UPM00),
	USART_PARITY_MODE_ODD								= (1 << UPM01) | (1 << UPM00)
} usart_parity_mode_t;

// USART_STOP_BIT enumeration
typedef enum {
	USART_STOP_BIT_1b									= (0 << USBS0),
	USART_STOP_BIT_2b									= (1 << USBS0)
} usart_stop_bit_t;

// USART_CLOCK_POLARITY enumeration
typedef enum {
	USART_CLOCK_POLARITY_RISING_EDGE					= (0 << UCPOL0),
	USART_CLOCK_POLARITY_FALLING_EDGE					= (1 << UCPOL0)
} usart_clock_polarity_t;

/*********************************************************************************************************************************************/

/*********************************************************************************************************************************************/
// Function prototypes

// Initializing a USART communication type
void	usart_init(usart_mode_t USART_mode,
usart_clock_polarity_t USART_clock_polarity,
usart_speed_t USART_speed,
usart_character_size_t USART_character_size,
usart_parity_mode_t USART_parity_mode,
usart_stop_bit_t USART_stop_bit,
usart_mpcm_t USART_multiprocessor_mode,
uint16_t USART_baud_value
);

// Initializing a UART communication type
void	uart_init(usart_speed_t USART_speed,
usart_character_size_t USART_character_size,
usart_parity_mode_t USART_parity_mode,
usart_stop_bit_t USART_stop_bit,
usart_mpcm_t USART_multiprocessor_mode,
uint16_t USART_baud_value
);

// Initializing a USRT communication type
void	usrt_init(usart_clock_polarity_t USART_clock_polarity,
usart_character_size_t USART_character_size,
usart_parity_mode_t USART_parity_mode,
usart_stop_bit_t USART_stop_bit,
usart_mpcm_t USART_multiprocessor_mode,
uint16_t USART_baud_value
);

// Polling Transmit				(Without changing other settings)
// The function shall look for the number of bits being transmitted and work as needed.
void	usart_polling_transmit(unsigned char USART_data);
//void	usart_9b_polling_transmit(unsigned int USART_data);

// Interrupt transmit			(Without changing other settings)
void	usart_transmit_bytes(const uint8_t* USART_buffer, uint8_t USART_length);
void	usart_transmit_string(const char* USART_string);
uint8_t usart_load_next_byte();

// Polling Receive				(Without changing other settings)
// The function shall look for the number of bits being received and work as needed.
unsigned char	usart_polling_receive();
//unsigned int	usart_9b_polling_receive();

// Interrupt receive			(Without changing other settings)
void	usart_receive_bytes();
void	usart_receive_string(char* USART_destination_buffer);
uint8_t usart_get_received_byte(uint8_t USART_index);
uint8_t usart_get_received_length();
void	usart_rx_buffer_flush();

// Receive buffer flush			(Without changing other settings)
void	usart_flush();

// Interrupt enabling			(Without changing other settings)
void	usart_rx_interrupt_enable();									// Receive complete
void	usart_tx_interrupt_enable();									// Transmit complete
void	usart_data_register_empty_interrupt_enable();					// Data register empty

// Interrupt disabling			(Without changing other settings)
void	usart_rx_interrupt_disable();									// Receive complete
void	usart_tx_interrupt_disable();									// Transmit complete
void	usart_data_register_empty_interrupt_disable();					// Data register empty

// Frame formats				(Without changing other settings)
void	usart_frame_format(usart_character_size_t USART_character_size, usart_parity_mode_t USART_parity_mode, usart_stop_bit_t USART_stop_bit);
void	usart_character_size(usart_character_size_t USART_character_size);
void	usart_parity_mode(usart_parity_mode_t USART_parity_mode);
void	usart_stop_bit(usart_stop_bit_t USART_stop_bit);

// Sub-protocoled communications	(Without changing other settings)
void	usart_multiprocessor_enable();
void	usart_multiprocessor_disable();

// Pins enabling				(Without changing other settings)
void	usart_rx_enable();
void	usart_tx_enable();
void	usart_rx_disable();
void	usart_tx_disable();

/*********************************************************************************************************************************************/

#endif /* M328PUSART_H_ */