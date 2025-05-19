/*
 * Proyecto02.c
 *
 * Created: 30/04/2025 07:25:47 p. m.
 * Author : ang50
 */ 

/*********************************************************************************************************************************************************************************************************************/
// Libraries
#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "m328padc.h"
#include "m328pusart.h"
#include "m328ptims8b.h"
#include "m328ptim1.h"

/*********************************************************************************************************************************************************************************************************************/

/*********************************************************************************************************************************************************************************************************************/
/*
=======================================================
=                  SYSTEM OVERVIEW                    =
=======================================================

This system controls 8 servo motors using a 3-to-8 decoder. Only 4 control pins are needed!
The current motor pulse width (OCR1A) is updated periodically using TIM0 and TIM1 interrupts.
Control values for each motor are stored and managed in structured buffers, depending on the selected operation mode.

Available operation modes:
 - MANUAL: Values are taken from ADC or fixed arrays (Motors.Manual[]).
 - ADAFRUIT: Values are received via UART (Motors.Adafruit[]).
 - EEPROM: Values are loaded from persistent EEPROM memory (Motors.Usable[] = position N).

EEPROM supports:
 - 4 positions (presets), each with 8 motor values (1 byte per motor).
 - Storage and retrieval are done using eeprom_update_block() and eeprom_read_block().
 - EEPROM_Store_Flag is used to trigger a write from within the main loop, avoiding writes inside ISRs.

User input interface:
 - Rotary encoder KY-040 is used for selection, scrolling, and confirming actions.
 - Encoder SW (push) triggers mode selection, EEPROM selection, or EEPROM storing.
 - Long press (2s) activates EEPROM store mode; short press handles mode switching.

Visual interface:
 - A single 7-segment display shows active mode or EEPROM position.
 - Blinking is managed with Blink_State and TIM0 for visual feedback during selection phases.

Main functional flags:
 - Operation_Mode_Selection: Indicates if the user is actively choosing a mode.
 - EEPROM_Selection_Mode: Indicates if the user is selecting which EEPROM preset to load.
 - EEPROM_Store_Arrange_Mode: Indicates if the user is selecting where to store a preset.
 - EEPROM_Store_Flag: Triggers a one-time EEPROM write in the main loop.

All interaction logic is non-blocking and state-driven, allowing continuous PWM generation and UART reception.

Further information is given in them explanatory documents!
*/
/*********************************************************************************************************************************************************************************************************************/

/*********************************************************************************************************************************************************************************************************************/
// Variables, constants and data structures

// Encoder variables
#define				ENABLED					   1			// A "define" that'll come handy for the "Operation_Mode_Selection" variable
#define				DISABLED				   0			// A "define" that'll come handy for the "Operation_Mode_Selection" variable
uint8_t				Operation_Mode_Selection = 0;			// Used for letting the user switch the operation mode
volatile uint8_t	ENCODER_SW_Last			 = 1;			// Used for storing the encoder's switch last read value
															// Because of using pull-up, it's initial value shall be 1
														
uint8_t				ENCODER_SW_Timer_Enable	 = DISABLED;	// A flag for knowing if a pushing-time timer is needed
uint16_t			ENCODER_SW_Push_Time	 = 0;			// Used for keeping track of how much time the encoder's switch have been pushed


// Display list's											// Used for mapping them LED's correct sequences
uint8_t DISP7SEG_NUMS_PD[10] = {0xFC, 0x18, 0x6C,		
								0x3C, 0x98, 0xB4, 
								0xF0, 0x1C, 0xFC, 
								0x9C};
uint8_t DISP7SEG_NUMS_PC[10] = {0,    0,    1, 
								1,    1,    1,
								1,    0,    1, 
								1};	
								
uint8_t DISP7SEG_MODES_PD[3] = {0xD8, 0xDC, 0xE4};			// [0] = "H"; [1] = "A"; [2] = "E";
uint8_t DISP7SEG_MODES_PC[3] = {1, 1, 1}; 


// ADC variables
uint8_t ADC_Count				= 0;						// Used for selecting which channel the ADC should read
uint8_t ADC_Lec					= 0;						// Used for keeping the last ADC Lecture


// Decoder variables, buffers and data structures
#define MANUAL					  0							// A "define" that'll come handy for establishing the operation mode
#define ADAFRUIT				  1							// A "define" that'll come handy for establishing the operation mode
#define EEPROM					  2							// A "define" that'll come handy for establishing the operation mode
uint8_t Operation_Mode			= MANUAL;					// Used for establishing the motor's positions mode
uint8_t Next_Operation_Mode		= MANUAL;					// Used for defining the next motor's operation mode if the encoder is used
															// and if selection mode is enabled
typedef struct {
	uint8_t	Adafruit[8];
	uint8_t Manual[4];
	uint8_t Usable[8];
} MotorDecoding;
MotorDecoding Motors;										// A struct for organizing them motor'9s positions depending on the mode
uint8_t TIM0_Count		= 0;								// Used for knowing which Selector's signal to send to the decoder
	
	
// UART buffer
#define EncodedDataBufferSize 16
char	EncodedData[EncodedDataBufferSize];					// Buffer used for keeping the incoming Adafruit data


// PWM mapping list											// List kept in RAM for accessing the correct OCR1A value to set depending on the ADC Lectures
#define PWM_TABLE_SIZE 256
const uint8_t ADCH_to_PWM[256] = {
    10,10,10,10,10,10,10,10,10,10,11,11,11,11,11,11,
    11,11,11,11,12,12,12,12,12,12,12,12,12,12,13,13,
    13,13,13,13,13,13,13,13,14,14,14,14,14,14,14,14,
    14,14,15,15,15,15,15,15,15,15,15,15,16,16,16,16,
    16,16,16,16,16,16,17,17,17,17,17,17,17,17,17,17,
    18,18,18,18,18,18,18,18,18,18,19,19,19,19,19,19,
    19,19,19,19,20,20,20,20,20,20,20,20,20,20,21,21,
    21,21,21,21,21,21,21,21,22,22,22,22,22,22,22,22,
    22,22,23,23,23,23,23,23,23,23,23,23,24,24,24,24,
    24,24,24,24,24,24,25,25,25,25,25,25,25,25,25,25,
    26,26,26,26,26,26,26,26,26,26,27,27,27,27,27,27,
    27,27,27,27,28,28,28,28,28,28,28,28,28,28,29,29,
    29,29,29,29,29,29,29,29,30,30,30,30,30,30,30,30,
    30,30,31,31,31,31,31,31,31,31,31,31,32,32,32,32,
    32,32,32,32,32,32,33,33,33,33,33,33,33,33,33,33,
    34,34,34,34,34,34,34,34,34,34,35,35,35,35,35,35
};

// Deadband-based PWM filtering variables and data structures
#define  PWM_TOLERANCE				  2						// PWM value difference required to trigger an update
uint8_t  PWM_Last_Value[8]			= {0};					// Stores the last value actually sent to OCR1A per motor
#define  PWM_ACTIVE_DURATION_TICKS	  25					// TIM0 ticks required to keep PWM active after a change (~.5s)
uint16_t PWM_Active_Timer[8]		= {0};					// Timer to hold PWM output active per motor after a change


// Blinking variables										// Used for timing them display's blinks
#define ON					  1	
#define OFF					  0							
uint8_t Blink_Count			= 0;
uint8_t Blink_State			= ON;
uint8_t Any_Selection_Mode	= 1;							// Used for establishing blinking mode!


// EEPROM
uint8_t	EEPROM_Store_Arrange_Mode		= DISABLED;			// A flag for knowing if storing an arrange in EEPROM is wanted
uint8_t	EEPROM_Store_Selected_Position	= 0;				// Used for letting the user decide in which EEPROM position to store the arrange
uint8_t EEPROM_Store_Flag				= DISABLED;			// Used for avoiding to store positions at every loop and waste the EEPROM usage!
uint8_t EEPROM_Selection_Mode			= DISABLED;			// A flag for knowing if an EEPROM stored arranged is wanted to be displayed
uint8_t EEPROM_Selected_Position		= 0;				// Used for letting the user decide which EEPROM stored position to display
#define BytesPerArrange					  8					// Number of bytes needed per arrange ("8" as that's the number of motors controlled)
#define StoredArrangements				  4					// Number of positions to store in EEPROM
#define TotalEEPROM						(BytesPerArrange*StoredArrangements)

/*********************************************************************************************************************************************************************************************************************/
// Function prototypes

void	SETUP();
void	UART_ParseAdafruitFeedData();
void	EEPROM_StoreMotorsArrange(uint8_t Position_Number, const uint8_t* Arrange);
void	EEPROM_ReadMotorsArrange(uint8_t Position_Number, uint8_t* Destination_Location);
void	UpdatePWM_IfNeeded(uint8_t Motor_Index);

/*********************************************************************************************************************************************************************************************************************/
// Main Function

int main(void)
{
	SETUP();
	
	while (1)
	{
		
		// Checking for incoming DATA from Adafruit
		UART_ParseAdafruitFeedData();
		
		// Checking if a new operation mode is required. If so, the operation mode is updated
		if ((Operation_Mode_Selection == DISABLED) && (Next_Operation_Mode != Operation_Mode)) Operation_Mode = Next_Operation_Mode;
		
		// Checking if a storage in EEPROM is required
		if (EEPROM_Store_Flag == ENABLED)
		{
			EEPROM_StoreMotorsArrange(EEPROM_Store_Selected_Position, Motors.Usable);
			EEPROM_Store_Flag = DISABLED;
		}
		
		// Updating if any selection mode is enabled
		if ((Operation_Mode_Selection == ENABLED) || (EEPROM_Selection_Mode == ENABLED) || (EEPROM_Store_Arrange_Mode == ENABLED)) Any_Selection_Mode = ENABLED;
		else Any_Selection_Mode = DISABLED;
		
		// Depending on the mode, values are updated to MotorDecoding.Usable[]
		// No matter if MANUAL mode is required, Motors[5,6,7,8] are driven by Adafruit
		switch (Operation_Mode)
		{
			case MANUAL:
				for (uint8_t i = 0; i < 4; i++) {Motors.Usable[i] = Motors.Manual[i];}
				for (uint8_t i = 4; i < 8; i++) {Motors.Usable[i] = Motors.Adafruit[i];}
				break;
			case ADAFRUIT:
				for (uint8_t i = 0; i < 8; i++) {Motors.Usable[i] = Motors.Adafruit[i];}
				break;
			case EEPROM:
				switch (EEPROM_Selected_Position)
				{
					case 0: {EEPROM_ReadMotorsArrange(0, Motors.Usable); break;}
					case 1: {EEPROM_ReadMotorsArrange(1, Motors.Usable); break;}
					case 2: {EEPROM_ReadMotorsArrange(2, Motors.Usable); break;}
					case 3: {EEPROM_ReadMotorsArrange(3, Motors.Usable); break;}
				}
				break;
			default: break;
		}
		
		// Updating the display value
		// Checking if ANY selection mode is enabled. If so, the display's blink state is revised
		// If not, the LED's mapping is disposed depending the mode
		if (Any_Selection_Mode == ENABLED)
		{
			if ((Operation_Mode_Selection == ENABLED) && (EEPROM_Selection_Mode == DISABLED) && (EEPROM_Store_Arrange_Mode == DISABLED))
			{
				if (Blink_State == ON)
				{
					PORTD	&= ~(0xFC);
					PORTD	|= DISP7SEG_MODES_PD[Next_Operation_Mode];
					PORTC	&= ~(0x01);
					PORTC	|= DISP7SEG_MODES_PC[Next_Operation_Mode];
				} else
				{
					PORTD	&= ~(0xFC);
					PORTC	&= ~(0x01);
				}
			} else if ((EEPROM_Selection_Mode == ENABLED) && (EEPROM_Store_Arrange_Mode == DISABLED))
			{
				if (Blink_State == ON)
				{
					PORTD	&= ~(0xFC);
					PORTD	|= DISP7SEG_NUMS_PD[EEPROM_Selected_Position+1];
					PORTC	&= ~(0x01);
					PORTC	|= DISP7SEG_NUMS_PC[EEPROM_Selected_Position+1];
				} else
				{
					PORTD	&= ~(0xFC);
					PORTC	&= ~(0x01);
				}
			} else if ((EEPROM_Selection_Mode == DISABLED) && (EEPROM_Store_Arrange_Mode == ENABLED))
			{
				if (Blink_State == ON)
				{
					PORTD	&= ~(0xFC);
					PORTD	|= DISP7SEG_NUMS_PD[EEPROM_Store_Selected_Position+1];
					PORTC	&= ~(0x01);
					PORTC	|= DISP7SEG_NUMS_PC[EEPROM_Store_Selected_Position+1];
				} else
				{
					PORTD	&= ~(0xFC);
					PORTC	&= ~(0x01);
				}
			}
			
		} else 
		{
			PORTD	&= ~(0xFC);								
			PORTD	|= DISP7SEG_MODES_PD[Operation_Mode];
			PORTC	&= ~(0x01);
			PORTC	|= DISP7SEG_MODES_PC[Operation_Mode];
		}
	}
}

/*********************************************************************************************************************************************************************************************************************/
// NON-Interrupt subroutines

void	SETUP()
{
	// Deactivating interrupts
	cli();
	
	// Reducing global CLK to 1MHZ
	CLKPR	|= (1 << CLKPCE);
	CLKPR	= (0 << CLKPCE) | (0 << CLKPS3) | (1 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0);
	
	// Display
	DDRD	|= (1 << DDD7) | (1 << DDD6) | (1 << DDD5) | (1 << DDD4) | (1 << DDD3)| (1 << DDD2);
	DDRC	|= (1 << DDC0);
	
	
	// Encoder
	DDRC	&= ~((1 << DDC3) | (1 << DDC2) | (1 << DDC1));
	PORTC	|= (1 << DDC3);									// Pull-up enabled for SW
	PORTC	&= ~((1 << DDC2) | (1 << DDC1));				// Pull-up disabled for DATA and CLK
	PCICR	|= (1 << PCIE1);
	PCMSK1	|= (1 << PCINT11) | (1 << PCINT9);				// Masked SW and DATA only for PC ISR
	
	
	// Decoder
	DDRB	|= (1 << DDB5) | (1 << DDB4) | (1 << DDB3) | (1 << DDB2);
	
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
	//usart_data_register_empty_interrupt_enable();
	
	// Cargando valores iniciales para cada posición de EEPROM (Prueba)
	uint8_t	POS1[8] = {50, 0, 0, 0, 0, 0, 0, 0};
	uint8_t	POS2[8] = {100, 0, 0, 0, 0, 0, 0, 0};
	uint8_t	POS3[8] = {10, 0, 0, 0, 0, 0, 0, 0};
	uint8_t	POS4[8] = {220, 0, 0, 0, 0, 0, 0, 0};
	EEPROM_StoreMotorsArrange(0, POS1);
	EEPROM_StoreMotorsArrange(1, POS2);
	EEPROM_StoreMotorsArrange(2, POS3);
	EEPROM_StoreMotorsArrange(3, POS4);
	
	// Activating interrupts
	sei();
	
	// Initiating the first ADC Lecture
	adc_start_conversion();
}


/* UART_ParseAdafruitFeedData function. The incoming Adafruit data shall be parsed for knowing to which index of the Motors.Adafruit struct should the incoming value
   should be kept. Afterwards, the USART Receive Buffer is flushed for keeping more data. */
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
		// As in ASCII all characters are correctly listed (A=65, B=66, C=67, ...), the following code shall look if the ID is for motor controlling, 
		// for EEPROM saving, or for operation mode changing. If the ID is not valid, the data is thrashed.
		// If 8 motors are to be controlled, the possible ID's for motor controlling are 'A', 'B', 'C', 'D', 'E', 'F', 'G' and 'H'
		// If the data incoming is for EEPROM saving, the ID should be 'S'
		// If the data incoming is for operation mode changing, the ID should be 'M'
		char ID = EncodedData[1];
		if ((ID >= 'A') && (ID <= 'H'))
		{
			uint8_t DATA_Value = (uint8_t)atoi(&EncodedData[3]);		// 'atoi' turns VAL into a string up until ';'
			uint8_t index = ID - 'A';									// This helps to indicate to which motor VAL should be updated 
			Motors.Adafruit[index] = DATA_Value;						// Data is copied to the correct Adafruit[] index
		
		} else if (ID == 'M')
		{
			switch (EncodedData[3])
			{
				case '0': Next_Operation_Mode = MANUAL;		Operation_Mode_Selection = DISABLED; break;
				case '1': Next_Operation_Mode = ADAFRUIT;	Operation_Mode_Selection = DISABLED; break;
				default: break;
			}
		} else if (ID == 'S')
		{
			uint8_t DATA_Value = (uint8_t)atoi(&EncodedData[3]);
			EEPROM_StoreMotorsArrange(DATA_Value - 1, Motors.Usable);		// The actual motors arrange is stored in the selected position
		} else {usart_rx_buffer_flush(); return;}							// If the index is not correct, the data is thrashed

			
		usart_rx_buffer_flush();
	}
}


// EEPROM_StoreMotorsArrange function. The position number shall be declared for the storage address to be correctly defined.
void	EEPROM_StoreMotorsArrange(uint8_t Position_Number, const uint8_t* Arrange)
{
	uint16_t Address	= Position_Number*BytesPerArrange;
	eeprom_update_block(Arrange, (void*)Address, BytesPerArrange);
}


// EEPROM_ReadMotorsArrange function. The position number shall be declared for the reading address to be correctly defined.
void	EEPROM_ReadMotorsArrange(uint8_t Position_Number, uint8_t* Destination_Location)
{
	uint16_t Address	= Position_Number*BytesPerArrange;
	eeprom_read_block((void*)Destination_Location, (const void*)Address, BytesPerArrange);
}
	
/* UpdatePWM_IfNeeded function. Jitter is filtered by applying a deadband tolerance. It updates OCR1A only if the difference
   between the new PWM value and the previous value is greater than the defined tolerance. If the difference is within the 
   tolerance , the signal is only repeated if still within the active timeout. If the timeout has expired, no pulse is issued 
   to reduce jitter and unnecessary power draw */
void	UpdatePWM_IfNeeded(uint8_t Motor_Index)
{
	uint8_t NewValue = ADCH_to_PWM[Motors.Usable[Motor_Index]];
	
	// If a significant change is detected, apply it and reset the active timer
	if ((NewValue  > (PWM_Last_Value[Motor_Index] + PWM_TOLERANCE)) || ((NewValue + PWM_TOLERANCE) < PWM_Last_Value[Motor_Index]))
	{
		PWM_Last_Value[Motor_Index]		= NewValue;
		PWM_Active_Timer[Motor_Index]	= PWM_ACTIVE_DURATION_TICKS;
		tim1_ocr_value(TIM1_CHANNEL_A, (uint16_t)NewValue);
		tim1_tcnt_value(0);
	
	// Else, if still within active window, the output is maintained
	} else if (PWM_Active_Timer[Motor_Index] > 0)
	{
		PWM_Active_Timer[Motor_Index]--;
		tim1_ocr_value(TIM1_CHANNEL_A, (uint16_t)PWM_Last_Value[Motor_Index]);
		tim1_tcnt_value(0);
	}
	
	// Otherwise, suppress output (no update)
}

/*********************************************************************************************************************************************************************************************************************/
// Interrupt routines


// UART receive interrupt routine. "receive_bytes" function is used.
ISR(USART_RX_vect)
{
	
	usart_receive_bytes();
	
}



// PCINT interrupt routine. It shall be checked which action is needed (Depending which pin changed)
ISR(PCINT1_vect)
{

	// SW logic
	// PINC3 value is saved
	uint8_t SW_State = (PINC & (1 << PINC3)) ? 1 : 0;
	// If SW is being pushed, and if it was not being pressed before, ENCODER_SW_Push_Time starts
	if ((SW_State == 0) && ENCODER_SW_Last == 1)											// Falling edge detected: SW pushed
	{
		ENCODER_SW_Timer_Enable = ENABLED;
		ENCODER_SW_Push_Time	= 0;
	}
	// If SW was being pushed but not anymore, and the time pushed is less than 2secs, the selection mode is updated depending then actual state!
	if ((SW_State == 1) && (ENCODER_SW_Last == 0) && (ENCODER_SW_Push_Time < 800))			// Rising edge detected: SW liberated
	{
		ENCODER_SW_Timer_Enable = DISABLED;
		switch (EEPROM_Store_Arrange_Mode)
		{
			case ENABLED:
				EEPROM_Store_Arrange_Mode = DISABLED; EEPROM_Store_Flag = ENABLED; break;
			case DISABLED:
				if (Operation_Mode_Selection == DISABLED) Operation_Mode_Selection = ENABLED;
				else if ((Operation_Mode_Selection == ENABLED) && (Next_Operation_Mode != EEPROM) && (EEPROM_Selection_Mode == DISABLED)) Operation_Mode_Selection = DISABLED;
				else if ((Operation_Mode_Selection == ENABLED) && (Next_Operation_Mode == EEPROM) && (EEPROM_Selection_Mode == DISABLED)) {Operation_Mode_Selection = DISABLED; EEPROM_Selection_Mode = ENABLED;}
				else if ((Operation_Mode_Selection == DISABLED) && (EEPROM_Selection_Mode == ENABLED)) {Operation_Mode_Selection = DISABLED; EEPROM_Selection_Mode = DISABLED;}
				else if ((Operation_Mode_Selection == ENABLED) && (EEPROM_Selection_Mode == ENABLED)) {Operation_Mode_Selection = DISABLED; EEPROM_Selection_Mode = DISABLED;}
				break;
			default: break;
		}
		
	}
	ENCODER_SW_Last = SW_State;																// ENCODER_SW_Last updated
	
	// DATA and CLK logic
	// PINC2,1 values are saved
	uint8_t DATA_State = (PINC & (1 << PINC2));
	uint8_t CLK_State = (PINC & (1 << PINC1));
	if (!CLK_State)																			// Falling edge detected: Encoder spin
	{
	// Depending if any selection mode is enabled or not, an action is made
	// If operation mode selection is enabled, the NEXT* operation mode is changed
	// If EEPROM selection mode is enabled, the EEPROM selected position is updated	
	// If EEPROM store arrange mode is enabled, the EEPROM store selected position is updated
		if ((Operation_Mode_Selection == ENABLED) && (EEPROM_Selection_Mode == DISABLED) && (EEPROM_Store_Arrange_Mode == DISABLED))
		{
			if (DATA_State)
			{
				Next_Operation_Mode++;
				if (Next_Operation_Mode == 3) Next_Operation_Mode = MANUAL;
			} else if (!DATA_State) 
			{
				Next_Operation_Mode--;
				if (Next_Operation_Mode == 255) Next_Operation_Mode = EEPROM;
			}
		} else if (EEPROM_Selection_Mode == ENABLED && (EEPROM_Store_Arrange_Mode == DISABLED))
		{
			if (DATA_State)
			{
				EEPROM_Selected_Position++;
				if (EEPROM_Selected_Position == 4) EEPROM_Selected_Position = 0;
			} else if (!DATA_State)
			{
				EEPROM_Selected_Position--;
				if (EEPROM_Selected_Position == 255) EEPROM_Selected_Position = 3;
			}
		} else if (EEPROM_Store_Arrange_Mode == ENABLED)
		{
			if (DATA_State)
			{
				EEPROM_Store_Selected_Position++;
				if (EEPROM_Store_Selected_Position == 4) EEPROM_Store_Selected_Position = 0;
			} else if (!DATA_State)
			{
				EEPROM_Store_Selected_Position--;
				if (EEPROM_Store_Selected_Position == 255) EEPROM_Store_Selected_Position = 3;
			}
		}
	}
	
}



// TIM0 OC0A interrupt routine. "TIM0_Count" is incremented, and depending it's value, the decoder's selector bits are correctly
// established, and the correct OCR1A value is uploaded. If any blinking mode is enabled, them blinking variables do its job too!
// And, a timer for checking the push duration of the encoder's SW is updated if required.
ISR(TIMER0_COMPA_vect)
{
	cli();

	// If the Encoder SW is being pushed, Encoder_SW_Push_Time is incremented
	// If SW is pushed 2secs, the mode changes
	if (ENCODER_SW_Timer_Enable == ENABLED) ENCODER_SW_Push_Time++;
	if (ENCODER_SW_Push_Time == 800) 
	{
		ENCODER_SW_Timer_Enable = DISABLED;
		ENCODER_SW_Push_Time	= 0xFFFF;								// So that no error happens at the rising edge of SW
		// If any of the operation selection mode OR the EEPROM selection mode are enabled (NOT any selection mode); nothing is done
		// If the operation selection mode AND the EEPROM selection mode are both disabled, AND the operation mode is not EEPROM, AND the EEPROM_Store_Arrange_Mode is disabled, the EEPROM_Store_Arrange_Mode is enabled
		// If the operation selection mode AND the EEPROM selection mode are both disabled, AND the operation mode is not EEPROM, AND the EEPROM_Store_Arrange_Mode is enabled, the EEPROM_Store_Arrange_Mode is disabled
		if ((Operation_Mode_Selection == DISABLED) && (EEPROM_Selection_Mode == DISABLED) && (Operation_Mode != EEPROM) && (EEPROM_Store_Arrange_Mode == DISABLED)) EEPROM_Store_Arrange_Mode = ENABLED;
		else if ((Operation_Mode_Selection == DISABLED) && (EEPROM_Selection_Mode == DISABLED) && (Operation_Mode != EEPROM) && (EEPROM_Store_Arrange_Mode == ENABLED)) EEPROM_Store_Arrange_Mode = DISABLED;
	}
	
	Blink_Count++;
	if (Blink_Count == 122) 
	{
		Blink_Count = 0; 
		if (Blink_State == ON) Blink_State = OFF;
		else Blink_State = ON;
	}
	
	TIM0_Count++;
	if (TIM0_Count == 8) TIM0_Count = 0;
	
	switch (TIM0_Count)
	{
		case 0:
			if (PWM_Active_Timer[0] > 0) PORTB	|= (1 << PORTB5) | (0 << PORTB4) | (0 << PORTB3) | (0 << PORTB2);
			else PORTB = 0;
			UpdatePWM_IfNeeded(0);
			break;
		case 1:
			if (PWM_Active_Timer[1] > 0) PORTB	|= (1 << PORTB5) | (0 << PORTB4) | (0 << PORTB3) | (1 << PORTB2);
			else PORTB = 0;
			UpdatePWM_IfNeeded(1);
			break;
		case 2:
			if (PWM_Active_Timer[2] > 0) PORTB	|= (1 << PORTB5) | (0 << PORTB4) | (1 << PORTB3) | (0 << PORTB2);
			else PORTB = 0;
			UpdatePWM_IfNeeded(2);
			break;
		case 3:
			if (PWM_Active_Timer[3] > 0) PORTB	|= (1 << PORTB5) | (0 << PORTB4) | (1 << PORTB3) | (1 << PORTB2);
			else PORTB = 0;
			UpdatePWM_IfNeeded(3);
			break;
		case 4:
			if (PWM_Active_Timer[4] > 0) PORTB	|= (1 << PORTB5) | (1 << PORTB4) | (0 << PORTB3) | (0 << PORTB2);
			else PORTB = 0;
			UpdatePWM_IfNeeded(4);
			break;
		case 5:
			if (PWM_Active_Timer[5] > 0) PORTB	|= (1 << PORTB5) | (1 << PORTB4) | (0 << PORTB3) | (1 << PORTB2);
			else PORTB = 0;
			UpdatePWM_IfNeeded(5);
			break;
		case 6:
			if (PWM_Active_Timer[6] > 0) PORTB	|= (1 << PORTB5) | (1 << PORTB4) | (1 << PORTB3) | (0 << PORTB2);
			else PORTB = 0;
			UpdatePWM_IfNeeded(6);
			break;
		case 7:
			if (PWM_Active_Timer[7] > 0) PORTB	|= (1 << PORTB5) | (1 << PORTB4) | (1 << PORTB3) | (1 << PORTB2);
			else PORTB = 0;
			UpdatePWM_IfNeeded(7);
			break;
		default: break;
	}
	
	sei();
}


// TIM1 OC1A interrupt routine. The decoder is disabled.
ISR(TIMER1_COMPA_vect)
{
	
	if (TIM0_Count < 8) PORTB = 0;
	
	
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

/*********************************************************************************************************************************************************************************************************************/
