/*
2. Termometr 1
Wykorzystaæ LM35 jako czujnik temperatury. ZnaleŸæ notê katalogow¹ LM35 i
zaproponowaæ sposób pod³¹czenia do LPC2138. Przygotowaæ program wyœwietlaj¹cy na
ALCD i wysy³aj¹cy wyniki na UART. Program powinien obs³ugiwaæ 2 przyciski:
MANUAL/AUTO i POMIAR. W trybie AUTO– wykonywane s¹ pomiary cyklicznie, co 1s,
w trybie MANUAL, pomiar jest dokonywany po naciœniêciu klawisza POMIAR

Mikrokontroler:					ATmega328p 
Czujnik temperatury:			cyfrowy DS18B20 1-Wire
Wyœwietlacz:					LCD ze sterownikiem HD44780
Potencjometr:					5kOhm
Rezystory:						3 x 2.2kOhm
Przyciski:						2
Kondensator:					100nF


*/
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include "LCD.h"
#include "one_wire_pk.h"
#include "uart_pk.h"


#define MANUAL_AUTO	PD3
#define POMIAR		PD2
#define RS			PD4
#define RW			PD5
#define E			PD6

/*
PC0 - D4
PC1 - D5
PC2 - D6
PC3 - D7

pc4 onewire port 

PD3 - PCINT19 INT1
*/


volatile bool auto_manual = true; // true = manual, false = false
int result[8];
char result_buf[8];


void wykonajPomiar(void);


int main(void)
{
	
	EICRA |= (1 << ISC11) | (0 << ISC10); // przerwanie 
	EIMSK |= (1 << INT1); // odblokuj przerwania na pinie INT1
	DDRD &= ~(1 << MANUAL_AUTO);
	DDRD &= ~(1 << POMIAR);


	uart_init();
	lcd_init();
	sei();
    while (1) 
    {
		
		
		if(!auto_manual)
		{
			wykonajPomiar();
			
			lcd_cls();
			lcd_goto(0,0);
			lcd_puttext("AUTO");
			lcd_goto(0,1);
			uart_transmit_string("[AUTO MODE] Temperatura: ");
			for(int i=0; i < 2; i++)
			{
				sprintf(result_buf, "%d", result[i]);
				lcd_puttext(result_buf);
				
				uart_transmit_string(result_buf);
				
				if(i == 0)
				{
					lcd_puttext(".");
					uart_transmit_string(".");
					
				}
			}
			
			uart_transmit_string("'C");
			lcd_puttext("'C");
			uart_transmit_char('\r');
			uart_transmit_char('\n');
			_delay_ms(1000);
			
		}
		
		
		
		if(auto_manual)
		{
			lcd_goto(0,0);
			lcd_puttext("MANUAL");
			if( (PIND & (1 << POMIAR)) == 0)
			{
				
				_delay_ms(200);
				wykonajPomiar();
				lcd_cls();
				lcd_goto(0,1);
				uart_transmit_string("[MANUAL MODE] Temperatura: ");
				for(int i=0; i < 2; i++)
				{
					sprintf(result_buf, "%d", result[i]);
					lcd_puttext(result_buf);
					uart_transmit_string(result_buf);
					if(i == 0)
					{
					lcd_puttext(".");
					uart_transmit_string(".");
						
					}
				}
				
				lcd_puttext("'C");
				uart_transmit_string("'C");
				uart_transmit_char('\r');
				uart_transmit_char('\n');
			}
		
		}
	}
}


ISR(INT1_vect)
{
	if(auto_manual) auto_manual = false;
	else			auto_manual = true;
	
}

void wykonajPomiar(){
	
	if(one_wire_reset())
	{
		one_wire_send_byte(0xCC); // skip rom
		one_wire_send_byte(0x44); // rozpoczecie odczytu temperatury
		_delay_ms(750);			  // oczekiwanie na koniec pomiaru
		one_wire_reset();		  // nowe zapytanie
		one_wire_send_byte(0xCC); // skip rom
		one_wire_send_byte(0xbe);  // odczyt z pamieci
		uint16_t LSB = one_wire_read_byte(); // odczyt LSB pierwszy bajt pamieci
		uint16_t MSB = one_wire_read_byte(); // odczyt MSB drugi bajt pamieci
		one_wire_reset();
		uint16_t resultTemp = ((MSB & 0x07) << 8) + LSB;
		//dla ujemnej temperatury
		if(MSB & 0xF0)
		{
			resultTemp = 2048 - resultTemp;
		}
		uint16_t temperature = resultTemp >> 4;
		// ulamek
		uint16_t frac = ((((uint32_t)resultTemp) & 0x0F) * 10);
		result[0] = temperature;
		result[1] = frac;
	}
}