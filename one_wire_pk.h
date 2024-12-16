/*
 * one_wire_pk.h
 *
 * Created: 15.12.2024 15:22:09
 *  Author: remig
 */ 

#include <stdint.h>

#ifndef ONE_WIRE_PK_H_
#define ONE_WIRE_PK_H_


uint8_t one_wire_reset(void);
void one_wire_send_bit(uint8_t bit);
void one_wire_send_byte(uint8_t data);
uint8_t one_wire_read_bit(void);
uint8_t one_wire_read_byte(void);
	

#endif /* ONE_WIRE_PK_H_ */