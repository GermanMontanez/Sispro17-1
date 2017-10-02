#ifndef MQ2_H
#define MQ2_H

#include <avr/io.h>

uint8_t MQ2_table[256];
void init_MQ2();
void MQ2_result(uint8_t *res);

#endif 