

#include <avr/io.h>
#include <stdint.h>
#include "uart_pk.h"



void uart_init()
{
	
	// UBRR = (16 000 000 / (16 * 9600)) - 1 = 103
	uint16_t ubrr_value = 103;
	
	UBRR0H = (uint8_t)(ubrr_value >> 8);		// starszy bajt
	UBRR0L = (uint8_t)ubrr_value;				// m�odszy bajt
	
	UCSR0B |= (1 << TXEN0);						// w��czenie nadajnika
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);		// 8 bit�w danych, 1 bit stopu, brak parzysto�ci
	
}

void uart_transmit_char(char data)
{
	// Czekaj, a� bufor nadawczy b�dzie pusty
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
	
}

void uart_transmit_string(char *str)
{
	
	while(*str)
	{
		uart_transmit_char(*str++);
		
	}
	
	
}