
#include "one_wire_pk.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>

uint8_t one_wire_reset()
{
	DDRC |= (1 << PC4);		// ustawiam na wyjscie
	PORTC &= ~(1 << PC4);	// ustawiam stan niski
	_delay_us(480);			// czekam 480 ms
	DDRC &= ~(1 << PC4);	// ustawiam pin na wejscie, czyli jest stan wysoki
	_delay_us(70);			// czekam 70 ms
	uint8_t result = 1;
	if(PINC & (1 << PC4)) { // odczytuje odpowiedz 
		result = 0;
	}
	
	_delay_us(410);
	return result;
	
}

void one_wire_send_bit(uint8_t bit)
{
	DDRC |= (1 << PC4); //ustawiam na wyjœcie
	PORTC &= ~(1 << PC4); //ustawiam stan niski
	if(bit){ //sprawdzam czy wysy³am 0 czy 1
		_delay_us(6); //dla 1 sygna³ trwa 6us
		DDRC &= ~(1 << PC4); //ustawiam stan na wysoki
		_delay_us(64); //czekam 64us do koñca
		} if(!bit){
		_delay_us(60); //dla zera sygnal 60us
		DDRC &= ~(1 << PC4); //ustawiam stan na wysoki
		_delay_us(10); //czekam 10us do koñca
	}
	
}

void one_wire_send_byte(uint8_t data){
	for(int8_t i =0; i <= 7; i++)
	{
		one_wire_send_bit(data & (1 << i));
		
	}
}

uint8_t one_wire_read_bit()
{
	DDRC |= (1 << PC4); //ustawiam na wyjœcie
	PORTC &= ~(1 << PC4); //ustawiam stan niski
	_delay_us(6); 
	DDRC &= ~(1 << PC4); //ustawiam stan na wysoki
	_delay_us(9);
	uint8_t result = 0;
	if(PINC & (1 << PC4)) { //odczytujê odpowiedŸ
		result = 1;
	}
	_delay_us(55);//czekam do koñca bitu
	return result;
}

uint8_t one_wire_read_byte(){
	uint8_t byte = 0;
	for(int8_t i = 0; i<8; i++)
	{
		byte |= (one_wire_read_bit() << i); // odczytaj bit i przesun o i
	}
	return byte;
}